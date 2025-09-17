#include <locale.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "getch.h"
#include "listdir.h"
#include "render.h"

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
    setlocale(LC_ALL, "");

    screenInfo* screen = scr.init();
    textList* dir = list_dir("~/");
    scr.add(screen, dir, WORDLIST);
    textList* cmds = tl.init();
    scr.add(screen, cmds, WORDLIST);
    while (1) {
        printScrn(screen);
        char chr = getch();
        int cont = 0;
        switch (chr) {
            case '\033':
wasEscape:
                if (moreInp()) {
                    chr = getch();
                    if (chr == '\033') {
                        // Escape key was pressed, but also an escape sequence is starting so go back to beginning of loop
                        goto wasEscape;
                    } else if (chr == '[') {
                        if (!moreInp()) {
                            // Dunno why this would ever occur, but if there's only half an escape code just continue
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
                break;
            case '\b':  // Backspace
                break;
            case '\n':  // Enter
                break;
            default:
                cont = 1;
        }
        if (cont) {
            char fullStr[2] = {chr, '\0'};  // Convert it to a complete string with a null terminator
            tl.add(cmds, fullStr);
        }
    }
}

