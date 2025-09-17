#include <locale.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "getch.h"
#include "listdir.h"
#include "render.h"

int main(void) {
    setlocale(LC_ALL, "");
    screenInfo* screen = scr.init();
    textList* dir = list_dir("~/");
    scr.add(screen, dir, WORDLIST);
    scr.add(screen, dir, WORDLIST);
    scr.add(screen, dir, WORDLIST);
    while (1) {
        printScrn(screen);
        int chr = getch();
        //printf("%d", chr);
    }
}

