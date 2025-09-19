#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <wchar.h>

#include "render.h"
#include "config.h"

const static char* bottomTxt = "Press ? for help";

char* cutoff(char* text, int maxlength) {
    if (strlen(text) <= maxlength) {
        return strdup(text);
    }
    char* newText = malloc(maxlength+1); // +1 for the \0
    if (!newText) { perror("malloc"); exit(EXIT_FAILURE); }
    strncpy(newText, text, maxlength-1);
    newText[maxlength-1] = '>';  // To show there is more
    newText[maxlength] = '\0';
    return newText;
}

void printScrn(screenInfo* screen) {
    wprintf(L"\033[2J\033[H");
    fflush(stdout);
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    if (w.ws_col <= 2*(screen->length)+1 || w.ws_col < strlen(bottomTxt)) {
        perror("Terminal too thin!");
        exit(EXIT_FAILURE);
    }

    int sectIdx = w.ws_col/screen->length;

    int ln = w.ws_col-1;
    wchar_t midLine[ln];
    for (int i = 0; i < ln-1; i++) midLine[i] = L'─';
    midLine[ln-1] = '\0';
    wchar_t topLine[ln];
    wmemcpy(topLine, midLine, ln);
    for (int i = 1; i < screen->length; i++) {
        topLine[sectIdx*i - 1] = L'┬';
    }
    char* hdr = screen->cols[screen->cursorCol].header;
    int base = sectIdx*screen->cursorCol;
    for (int i = 0; i < strlen(hdr); i++) {
        topLine[base + i] = hdr[i];
    }
    wprintf(L"╭%ls╮\n", topLine);


    int ln1 = sectIdx;  // This is 1 character shorter so when I print it I can add an extra space or | where necessary
    wchar_t mtRow[ln1];
    mtRow[0] = L'│';
    for (int i = 1; i < ln1-1; i++) mtRow[i] = ' ';
    mtRow[ln1-1] = '\0';

    int SelectedOffset;
    int cursorX;
    for (int i = 0; i < screen->length; i++) {
        if (screen->cursorCol == i) {
            SC.offset(&screen->cols[i], w.ws_row-2);
            SelectedOffset = *(&screen->cols[i].lastOffset);
            cursorX = screen->cols[i].cursorX;
        }
        SC.init(&screen->cols[i]);
    }
    for (int row = 0; row < w.ws_row-2; row++) {
        for (int part = 0; part < screen->length; part++) {
            char* text = SC.step(&screen->cols[part]);
            if (text == NULL) {
                if (part == screen->length - 1) {
                    int spare = w.ws_col - (sectIdx * screen->length);
                    char extraSpaces[spare+1];
                    for (int i = 0; i < spare; i++) extraSpaces[i] = ' ';
                    extraSpaces[spare] = '\0';
                    wprintf(L"%ls%s│\n", mtRow, extraSpaces);
                } else {
                    wprintf(L"%ls ", mtRow);
                }
            } else {
                int totlen = sectIdx-1;
                wchar_t* xtra = L"";
                if (part == screen->length - 1) {
                    int spare = w.ws_col - (sectIdx * screen->length) - 1;
                    totlen += spare;
                    xtra = L"│\n";
                }
                text = cutoff(text, totlen);
                int xtralen = totlen-strlen(text);
                char extraSpaces[xtralen+1];
                for (int i = 0; i < xtralen; i++) extraSpaces[i] = ' ';
                extraSpaces[xtralen] = '\0';

                wchar_t fmt[20];  // Enough for the maximum length the string can be (arbitrary number, should always work)
                if (screen->cursorCol == part && \
                    screen->cols[part].cursorY-SelectedOffset == row) {
                    wcscpy(fmt, L"│\033[1;7m%s%s\033[0m");  // Add inverse & bold to the selected value
                } else {
                    wcscpy(fmt, L"│%s%s");
                }
                wcscat(fmt, xtra);
                wprintf(fmt, text, extraSpaces);
                free(text);
            }
        }
    }

    for (int i = 1; i < screen->length; i++) {
        midLine[sectIdx*i-1] = L'┴';
    }

    int btlen = strlen(bottomTxt);
    for (int i = 0; i < btlen; i++) {
        midLine[ln-2-i] = bottomTxt[btlen-i-1];
    }
    wprintf(L"╰%ls╯", midLine);

    int cursX = 2 + sectIdx*screen->cursorCol + cursorX;
    wprintf(L"\033[1;%dH", cursX);  // Set cursor position
    fflush(stdout);
}

static char* helpTxt;
static int helpTxtLen;

void init_help() {
    const char* template =
        "HELP\n"
        "  (Press space to scroll this page if required)\n"
        "General keys:\n"
        "  Shift+(left/right) to switch column\n"
        "  ? shows this help screen"
        "File directory keys:\n"
        "  Up/down arrows (or tab/shift-tab keys) change selected folder (../ is go up a directory)\n"
        "  Enter goes into selected directory\n"
        "  Type to filter directory (backspace/delete also works)\n"
        "  Left/right arrow keys to move cursor to aid in inputting in filter\n"

        "\nYour config file is located in: %s\n"
        "Press space to exit\n";
    size_t bufsize = strlen(template) + strlen(confPath) + 1;
    helpTxt = malloc(bufsize);
    if (!helpTxt) { perror("malloc"); exit(EXIT_FAILURE); }
    snprintf(helpTxt, bufsize, template, confPath);

    helpTxtLen = strlen(helpTxt);
}

int printHelp(int idx) {
    wprintf(L"\033[2J\033[H");
    fflush(stdout);
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    if (w.ws_col <= 2) {
        perror("Terminal too thin!");
        exit(EXIT_FAILURE);
    }

    int ln = w.ws_col-1;
    wchar_t horizline[ln];
    for (int i = 0; i < ln-1; i++) horizline[i] = L'─';
    horizline[ln-1] = '\0';
    wprintf(L"╭%ls╮\n", horizline);
    char mtline[ln];
    for (int i = 0; i < ln-1; i++) mtline[i] = ' ';
    mtline[ln-1] = '\0';

    int maxWid = w.ws_col-2;
    int linesLeft = w.ws_row - 2;
    while (linesLeft > 0 && idx < helpTxtLen) {
        linesLeft--;
        const char* nxtNL = strchr(helpTxt + idx, '\n');
        int indexOffs;
        int xtra = 0;
        if (nxtNL) {
            indexOffs = nxtNL - helpTxt - idx;
            if (indexOffs > maxWid) {
                indexOffs = maxWid;
            } else {
                xtra = 1;
            }
        } else {
            indexOffs = maxWid;
        }
        int nIdx = idx + indexOffs;
        if (nIdx > helpTxtLen) {
            nIdx = helpTxtLen;
        }
        int xtraSpace = maxWid - (nIdx - idx);
        char spacing[xtraSpace+1];
        for (int i = 0; i < xtraSpace; i++) spacing[i] = ' ';
        spacing[xtraSpace] = '\0';
        wprintf(L"│%.*s%s│\n", nIdx-idx, helpTxt+idx, spacing);
        idx = nIdx+xtra;
    }
    for (int i = 0; i < linesLeft; i++) {
        wprintf(L"│%s│\n", mtline);
    }
    wprintf(L"╰%ls╯", horizline);
    fflush(stdout);
    if (idx >= helpTxtLen) {
        idx = -1;
    }
    return idx;
}

