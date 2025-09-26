#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
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
    init_terminal();
    wprintf(L"\033[2J\033[H");
    fflush(stdout);

    char* tmpcwd = getcwd(NULL, 0);
    if (!tmpcwd) {
        perror("getcwd failed");
        return 1;
    }
    char* cwd = malloc(strlen(tmpcwd)+2);
    strcpy(cwd, tmpcwd);
    free(tmpcwd);
    strcat(cwd, "/");
    screenInfo* screen = scr.init();

    scr.add(screen, init_config(), WORDLIST, DIRECTORY_SELECT);
    textList* dir = list_dir(cwd);
    tl.sort(dir, tlSort.dirs);
    dl.setup(dir, cwd);
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
            case REGULAR_KEY:
                if (chr->key == '\03') {
                    onExit(screen);
                    return 0;
                }
                if (chr->key == '?') {
                    makeTempCol(screen, strdup(helpTxt), HELPUSE);
                } else {
                    onKeyPress(screen, &screen->cols[screen->cursorCol], chr->key);
                }
            case NOTHING:
                break;
        }
    }
}

