#include <locale.h>
#include <stdio.h>
#include "getch.h"

#ifdef _WIN32
    #include <conio.h>

    static void reset_terminal(void) {}
    void init_terminal(void) {
        setlocale(LC_ALL, "");
    }
#else
    #include <unistd.h>
    #include <termios.h>
    #include <sys/select.h>
    #include <stdlib.h>  // for atexit

    static struct termios orig_termios;

    // Restore terminal on exit
    static void reset_terminal(void) {
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
    }

    // Initialize terminal in raw mode
    void init_terminal(void) {
        setlocale(LC_ALL, "");
        struct termios newt;
        tcgetattr(STDIN_FILENO, &orig_termios);
        newt = orig_termios;
        newt.c_lflag &= ~(ICANON | ECHO);  // raw mode, no echo
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        atexit(reset_terminal);  // ensure terminal is restored on exit
    }
#endif

// Read one character (non-blocking on Windows / raw mode on Unix)
char getch(void) {
#ifdef _WIN32
    return _getch();
#else
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        return c;
    }
    return 0;
#endif
}

// Check if more input is available (non-blocking)
int moreInp(void) {
#ifdef _WIN32
    return _kbhit();
#else
    struct timeval tv = {0L, 0L};  // zero timeout
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
#endif
}

keyReturn* getKey() {
#ifdef _WIN32
    int ch = _getch();
    if (ch == 0 || ch == 0xE0) {  // special key prefix
        int code = _getch();
        char escKey = '\0';
        switch (_getch()) {
            case 0x48:
                escKey = 'u';
                break;
            case 0x50:
                escKey = 'd';
                break;
            case 0x4D:
                escKey = 'r';
                break;
            case 0x4B:
                escKey = 'l';
                break;
            default:
                goto returnNothing;
        }
        keyReturn* kr = malloc(sizeof(keyReturn));
        if (!kr) { perror("malloc"); exit(EXIT_FAILURE); }
        kr->typ = ARROW_KEY;
        kr->key = escKey;
        return kr;
    }
    keyReturn* kr = malloc(sizeof(keyReturn));
    if (!kr) { perror("malloc"); exit(EXIT_FAILURE); }
    kr->typ = REGULAR_KEY;
    kr->key = ch;
    return kr;
#else
    char chr = getch();
    if (chr == '\033') {
        if (moreInp()) {
            chr = getch();
            if (chr == '[') {
                if (!moreInp()) {
                    // Dunno why this would ever occur, but if there's only half an escape code just ignore the rest of the keys and return nothing
                    while (moreInp()) getch();
                    goto returnNothing;
                }
                chr = getch();
                // Escape sequence!
                char escKey = '\0';
                switch (chr) {
                    case 'A':  // Up arrow
                        escKey = 'u';
                        break;
                    case 'B':  // Down arrow
                        escKey = 'd';
                        break;
                    case 'C':  // Right arrow
                        escKey = 'r';
                        break;
                    case 'D':  // Left arrow
                        escKey = 'l';
                        break;
                    default:  // Unknown escape key; just absorb the rest of the keys and return nothing
                        while (moreInp()) getch();
                        goto returnNothing;
                }
                keyReturn* kr = malloc(sizeof(keyReturn));
                if (!kr) { perror("malloc"); exit(EXIT_FAILURE); }
                kr->typ = ARROW_KEY;
                kr->key = escKey;
                return kr;
            } else if (chr == '\033') {
                // Escape key was pressed, but also an escape sequence is starting so just ignore the rest and return escape
                while (moreInp()) getch();
                goto returnEscape;
            } else {
                // Escape key was pressed, but another key was pressed straight after. So just ignore the next key.
                goto returnEscape;
            }
        } else {
            // Escape key was pressed
            goto returnEscape;
        }
    } else {
        keyReturn* kr = malloc(sizeof(keyReturn));
        if (!kr) { perror("malloc"); exit(EXIT_FAILURE); }
        kr->typ = REGULAR_KEY;
        kr->key = chr;
        return kr;
    }
#endif
returnNothing:
    keyReturn* nothingkr = malloc(sizeof(keyReturn));
    if (!nothingkr) { perror("malloc"); exit(EXIT_FAILURE); }
    nothingkr->typ = NOTHING;
    return nothingkr;
returnEscape:
    keyReturn* esckr = malloc(sizeof(keyReturn));
    if (!esckr) { perror("malloc"); exit(EXIT_FAILURE); }
    esckr->typ = ESCAPE_KEY;
    return esckr;
}

