#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <wchar.h>

#include "render.h"

void printScrn(screenInfo* screen) {
    wprintf(L"\033[2J\033[H");
    fflush(stdout);
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    if (w.ws_col <= 2*(screen->length+1)) {
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
//     textItem* it = dir->startIt;
    for (int row = 0; row < w.ws_row-2; row++) {
//         if (it == NULL) {
        for (int part = 0; part < screen->length; part++) {
            if (part == screen->length - 1) {
                int spare = w.ws_col - (sectIdx * screen->length);
                char extraSpaces[spare+1];
                for (int i = 0; i < spare; i++) extraSpaces[i] = ' ';
                extraSpaces[spare] = '\0';
                wprintf(L"%ls%s│\n", mtRow, extraSpaces);
            } else {
                wprintf(L"%ls ", mtRow);
            }
        }
        /*} else {
            size_t sze;  // Size of spacing
            char xtra = ' ';
            int tlen = strlen(it->text);
            if (ln1 > tlen+1) {
                sze = (int)(ln1-tlen) - 2;
            } else {
                sze = 0;
                xtra = '>';
            }
            char spacing[sze+1];
            memset(spacing, ' ', sze);
            spacing[sze] = '\0';
            wprintf(L"│%.*s%c%s%ls", ln1-2, it->text, xtra, spacing, mtRowEnd);
            it = it->next;
        }*/
    }

    for (int i = 1; i < screen->length; i++) {
        midLine[sectIdx*i-1] = L'┴';
    }
    wprintf(L"╰%ls╯", midLine);
}
