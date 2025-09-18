#include <sys/ioctl.h>
#include <unistd.h>

#include "getch.h"
#include "listdir.h"
#include "actions.h"
#include "render.h"

int main(void) {
    init_terminal();

    char* startingPath = expand_tilde("~/");
    screenInfo* screen = scr.init();
    init_actions(startingPath);
    textList* dir = list_dir(startingPath);
    tl.sort(dir, tlSort.alphaCIAsc);
    scr.add(screen, dir, WORDLIST, DIRECTORY_VIEW);
    while (1) {
        printScrn(screen);
        keyReturn* chr = getKey();
        switch (chr->typ) {
            case ARROW_KEY: {
                switch (chr->key) {
                    case 'u':  // Up arrow
                        scr.updCur(screen, 0, -1);
                        break;
                    case 'd':  // Down arrow
                        scr.updCur(screen, 0, 1);
                        break;
                    case 'R':  // Right arrow
                        scr.updCur(screen, 1, 0);
                        break;
                    case 'L':  // Left arrow
                        scr.updCur(screen, -1, 0);
                        break;
                    default:
                        onArrowPress(screen, &screen->cols[screen->cursorCol], chr->key);
                }
                break;
            }
            case ESCAPE_KEY:
                break;
            case REGULAR_KEY:
                onKeyPress(screen, &screen->cols[screen->cursorCol], screen->cursorRow, chr->key);
            case NOTHING:
                break;
        }
    }
}

