#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <wchar.h>

#include "render.h"

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
    for (int i = 1; i < screen->length; i++) {
        midLine[sectIdx*i - 1] = L'┬';
    }
    wprintf(L"╭%ls╮\n", midLine);


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
