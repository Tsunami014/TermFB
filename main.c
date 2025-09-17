#include <sys/ioctl.h>
#include <unistd.h>

#include "getch.h"
#include "listdir.h"
#include "render.h"
#include "actions.h"

int IOK(void) {  // Ignore Other Keys (returns when an escape key was found)
    /* Ignores all other keys pressed by reading them if they exist, stopping at the escape key */
    while (moreInp()) {
        if (getch() == '\033') {
            return 1;
        }
    }
    return 0;
}

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
        char chr = getch();
        int cont = chr != '\033';
        if (!cont) {
wasEscape:
            if (moreInp()) {
                chr = getch();
                if (chr == '\033') {
                    // Escape key was pressed, but also an escape sequence is starting so go back to beginning of loop
                    goto wasEscape;
                } else if (chr == '[') {
                    if (!moreInp()) {
                        // Dunno why this would ever occur, but if there's only half an escape code just ignore the rest of the keys then continue
                        if (IOK()) {
                            goto wasEscape;
                        }
                        continue;
                    }
                    chr = getch();
                    // Escape sequence!
                    switch (chr) {
                        case 'A':  // Up arrow
                            scr.updCur(screen, 0, -1);
                            break;
                        case 'B':  // Down arrow
                            scr.updCur(screen, 0, 1);
                            break;
                        case 'C':  // Right arrow
                            scr.updCur(screen, 1, 0);
                            break;
                        case 'D':  // Left arrow
                            scr.updCur(screen, -1, 0);
                            break;
                        default:  // Unknown escape key; just absorb the rest of the keys
                            if (IOK()) {
                                goto wasEscape;
                            }
                            continue;
                    }
                } else {
                    // Escape key was pressed, but another key was pressed straight after
                    cont = 1;
                }
            } else {
                // Escape key was pressed
            }
        }
        if (cont) {
            onKeyPress(&screen->cols[screen->cursorCol], screen->cursorRow, chr);
        }
    }
}

