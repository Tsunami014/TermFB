#include <locale.h>
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
