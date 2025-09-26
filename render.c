#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#ifdef _WIN32
    #include <windows.h>
    #define A "%hs"
    #define A2 "hs"
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
    #define A "%s"
    #define A2 "s"
#endif

#include "render.h"
#include "config.h"

const static char* bottomTxt = "Press ? for help";

#ifdef _WIN32
struct winsize {
    unsigned short ws_row;
    unsigned short ws_col;
};

void get_terminal_size(struct winsize* w) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    w->ws_col = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    w->ws_row = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}
#else
void get_terminal_size(struct winsize* w) {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, w);
}
#endif

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
    wprintf(L"\033[H");
    fflush(stdout);
    struct winsize w;
    get_terminal_size(&w);
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

    screenCol* selectedCol;
    for (int i = 0; i < screen->length; i++) {
        screenCol* col = &screen->cols[i];
        if (screen->cursorCol == i) {
            SC.offset(col, w.ws_row-2);
            selectedCol = col;
        }
        SC.init(col);
    }
    for (int row = 0; row < w.ws_row-2; row++) {
        for (int part = 0; part < screen->length; part++) {
            screenCol* col = &screen->cols[part];
            int selecting = 0;
            if (screen->cursorCol == part && \
                    col->cursorY-col->lastOffset == row) {
                selecting = 1;
            }
            int totlen = sectIdx-1;
            if (part == screen->length - 1) {
                totlen += w.ws_col - (sectIdx * screen->length) - 1;
            }
            char* text = SC.step(col, totlen);
            if (selecting && col->selectingRow) {
                text = col->selectedTxt;
            }
            if (text == NULL) {
                if (part == screen->length - 1) {
                    int spare = w.ws_col - (sectIdx * screen->length);
                    char extraSpaces[spare+1];
                    for (int i = 0; i < spare; i++) extraSpaces[i] = ' ';
                    extraSpaces[spare] = '\0';
                    wprintf(L"%ls" A L"│\n", mtRow, extraSpaces);
                } else {
                    wprintf(L"%ls ", mtRow);
                }
            } else {
                wchar_t* xtra = L"";
                if (part == screen->length - 1) {
                    xtra = L"│\n";
                }
                if (col->typ != TEMPORARY) {  // We know that the temporary column will never be too long
                    text = cutoff(text, totlen);
                }
                int xtralen = totlen-strlen(text);
                if (xtralen < 0) xtralen = 0;
                char extraSpaces[xtralen+1];
                for (int i = 0; i < xtralen; i++) extraSpaces[i] = ' ';
                extraSpaces[xtralen] = '\0';

                wchar_t fmt[20];  // Enough for the maximum length the string can be (arbitrary number, should always work)
                if (selecting) {
                    wcscpy(fmt, L"│\033[");
                    if (col->selectingRow) {
                        wcscat(fmt, L"100");  // Make background different from inverse
                    } else {
                        wcscat(fmt, L"1;7");  // Add inverse & bold to the selected value
                    }
                    wcscat(fmt, L"m" A A L"\033[0m");
                } else {
                    wcscpy(fmt, L"│" A A);
                }
                wcscat(fmt, xtra);
                wprintf(fmt, text, extraSpaces);
                if (text != col->selectedTxt) {
                    free(text);
                }
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

    int cursY;
    if (selectedCol->selectingRow) {
        cursY = selectedCol->cursorY - selectedCol->lastOffset + 2;
    } else {
        cursY = 0;
    }
    int cursX = 2 + sectIdx*screen->cursorCol + selectedCol->cursorX;
    wprintf(L"\033[%d;%dH", cursY, cursX);  // Set cursor position
    fflush(stdout);
}

char* helpTxt;

void init_help() {
    const char* template =
        "HELP\n"
        "  (Esc to close, any button to see more)\n"
        "  Your config file is located in: " A "\n"
        "General keys:\n"
        "  Shift+(left/right) to switch column\n"
        "  ? shows this help screen\n"
        "  Ctrl+C exits the program\n"
        "Command keys:\n"
        "  PLEASE NOTE:\n"
        "    When using, press enter to confirm or ESC to exit\n"
        "    When enter is pressed it will show a buffer to the side with the result of the command\n"
        "    This uses its own form of command that looks similar to but is NOT like Linux commands\n"
        "    Endings are important; items ending in '/' will be folders, otherwise is a file\n"
        "  Ctrl+r to rename (`rn <orig> <newname>`)\n"
        "  Ctrl+c to copy (`cp <orig> <new>`)\n"
        "  Ctrl+m to move (`mv <orig> <new>`) (can move between directories)\n"
        "  Ctrl+d to delete (`rmy <orig>`) (PLEASE NOTE it defaults to 'rm', you will have to type in the 'y' to confirm)\n"
        "  Ctrl+a to add (`mk <new>`)\n"
        "Action keys:\n"
        "  Up/down arrows (or tab/shift-tab keys) change selected item\n"
        "  Enter goes into selected directory (../ is go up a directory)\n"
        "  Type to filter (backspace/delete also works)\n"
        "  Left/right arrow keys to move cursor to aid in inputting in filter\n"
        ;
    size_t bufsize = strlen(template) + strlen(confPath) + 1;
    helpTxt = malloc(bufsize);
    if (!helpTxt) { perror("malloc"); exit(EXIT_FAILURE); }
    snprintf(helpTxt, bufsize, template, confPath);
}

void onExit(screenInfo* screen) {
    // Clear the screen
    wprintf(L"\033[2J\033[H");
    fflush(stdout);
    char* pth = ((dirViewInfo*)((textList*)screen->cols[1].data)->info)->path;
#ifdef _WIN32
    DWORD requiredSize = GetTempPathA(0, NULL);
    char* tempPath = malloc(requiredSize);
    if (!tempPath) { perror("malloc"); exit(EXIT_FAILURE); }

    DWORD actualSize = GetTempPathA(requiredSize, tempPath);
    if (actualSize == 0 || actualSize >= requiredSize) {
        printf("Failed to get temp path.\n");
        free(tempPath);
        return;
    }

    // Build full path
    const char* filename = "termfb_dir.txt";
    size_t fullLen = strlen(tempPath) + strlen(filename) + 1;
    char* filePath = malloc(fullLen);
    if (!filePath) { perror("malloc"); exit(EXIT_FAILURE); }

    snprintf(filePath, fullLen, "%s%s", tempPath, filename);
    FILE *f = fopen(filePath, "w");
    fprintf(f, "%s\n", pth);
    fclose(f);
    free(filePath);
    free(tempPath);
#else
    // Write to a special file descriptor the output directory for use in other programs
#define fd 3
    write(fd, pth, strlen(pth));
    write(fd, "\n", 1);
#endif
}

