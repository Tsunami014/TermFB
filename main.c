#include <stdio.h>
#include "getch.h"

int main(void) {
    while (1) {
        int chr = getch();
        printf("%d", chr);
    }
}

