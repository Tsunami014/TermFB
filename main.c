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
    textList* dir = init_actions(startingPath);
    scr.add(screen, dir, WORDLIST, DIRECTORY_VIEW);
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
                break;
            case REGULAR_KEY:
                if (chr->key == '?') {
                    int i = 0;
                    while (i != -1) {
                        i = printHelp(i);
                        while (getch() != ' ') {}  // Must wait for space
                    }

                } else {
                    onKeyPress(screen, &screen->cols[screen->cursorCol], chr->key);
                }
            case NOTHING:
                break;
        }
    }
}

