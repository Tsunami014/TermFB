#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#include "getch.h"
#include "listdir.h"
#include "actions.h"
#include "render.h"
#include "config.h"

int main(void) {
    wprintf(L"\033[2J\033[H");
    fflush(stdout);
    init_terminal();

    char* startingPath = expand_tilde("~/");
    screenInfo* screen = scr.init();

    scr.add(screen, init_config(), WORDLIST, DIRECTORY_SELECT);
    textList* dir = list_dir(startingPath);
    tl.sort(dir, tlSort.alphaCIAsc);
    dl.setup(dir, startingPath);
    scr.add(screen, dir, WORDLIST, DIRECTORY_VIEW);
    screen->cursorCol = 1;
    init_help();  // Must come after init_comig
    while (1) {
        printScrn(screen);
        keyReturn* chr = getKey();
        switch (chr->typ) {
            case ARROW_KEY: {
                switch (chr->key) {
                    case 'R':  // Right arrow
                        scr.updCur(screen, 1);
                        break;
                    case 'L':  // Left arrow
                        scr.updCur(screen, -1);
                        break;
                    default:
                        onArrowPress(screen, &screen->cols[screen->cursorCol], chr->key);
                }
                break;
            }
            case ESCAPE_KEY:
                wprintf(L"\033[2J\033[H");
                fflush(stdout);
                exit(EXIT_SUCCESS);
            case REGULAR_KEY:
                if (chr->key == '?') {
                    int i = 0;
                    while (i != -1) {
                        i = printHelp(i);
                        while (getThatCh() != ' ') {}  // Must wait for space
                    }
                } else {
                    onKeyPress(screen, &screen->cols[screen->cursorCol], chr->key);
                }
            case NOTHING:
                break;
        }
    }
}

