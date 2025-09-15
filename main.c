#include <stdio.h>
#include <locale.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <wchar.h>
#include "getch.h"

void printScrn(void) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    int ln = w.ws_col-1;
    wchar_t midLine[ln];
    for (int i = 0; i < ln-1; i++) midLine[i] = L'─';
    midLine[ln-1] = '\0';
    wprintf(L"╭%ls╮\n", midLine);


    ln = w.ws_col+2;
    wchar_t mtRow[ln];
    mtRow[0] = L'│';
    for (int i = 1; i < ln-1; i++) mtRow[i] = ' ';
    mtRow[w.ws_col-1] = L'│';
    mtRow[w.ws_col] = '\n';
    mtRow[w.ws_col+1] = '\0';  // Valid string end
    for (int row = 0; row < w.ws_row-2; row++) wprintf(mtRow);

    wprintf(L"╰%ls╯\n", midLine);
}

int main(void) {
    setlocale(LC_ALL, "");
    printScrn();
    return 0;
    /*while (1) {
        int chr = getch();
        printf("%d", chr);
    }*/
}

