#include <locale.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "getch.h"
#include "listdir.h"
#include "render.h"


int main(void) {
    setlocale(LC_ALL, "");
    textList* dir = list_dir("~/");
//     for (int i = 0; i < dir->length; i++) printf("%s\n", dir->items[i]);
    while (1) {
        if (!printScrn(dir)) {
            exit(EXIT_FAILURE);  // Was unable to print the screen
        }
        int chr = getch();
        //printf("%d", chr);
    }
}

