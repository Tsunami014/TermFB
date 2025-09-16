#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <wchar.h>
#include "getch.h"
#include "listdir.h"

void printScrn(directory* dir) {
    wprintf(L"\033[2J\033[H");
    fflush(stdout);
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    int midIdx = w.ws_col/2;

    int ln = w.ws_col-1;
    wchar_t midLine[ln];
    for (int i = 0; i < ln-1; i++) midLine[i] = L'─';
    midLine[ln-1] = '\0';
    midLine[midIdx] = L'┬';
    wprintf(L"╭%ls╮\n", midLine);


    int ln1 = midIdx+1;
    wchar_t mtRowBegin[ln1];
    mtRowBegin[0] = L'│';
    for (int i = 1; i < ln1-2; i++) mtRowBegin[i] = ' ';
    mtRowBegin[ln1-1] = '\0';
    int ln2 = w.ws_col+2 - ln1;
    wchar_t mtRowEnd[ln2];
    mtRowEnd[0] = L'│';
    for (int i = 1; i < ln2-3; i++) mtRowEnd[i] = ' ';
    mtRowEnd[ln2-3] = L'│';
    mtRowEnd[ln2-2] = '\n';
    mtRowEnd[ln2-1] = '\0';
    for (int row = 0; row < w.ws_row-2; row++) {
        if (row >= dir->length) {
            wprintf(L"%ls%ls", mtRowBegin, mtRowEnd);
        } else {
            size_t sze = (int)(ln1-strlen(dir->items[row])) - 1;
            char spacing[sze+1];
            memset(spacing, ' ', sze);
            spacing[sze] = '\0';
            wprintf(L"│%s%s%ls", dir->items[row], spacing, mtRowEnd);
        }
    }

    midLine[midIdx] = L'┴';
    wprintf(L"╰%ls╯", midLine);
}

int main(void) {
    setlocale(LC_ALL, "");
    directory* dir = list_dir("~/");
//     for (int i = 0; i < dir->length; i++) printf("%s\n", dir->items[i]);
    while (1) {
        printScrn(dir);
        int chr = getch();
        //printf("%d", chr);
    }
}

