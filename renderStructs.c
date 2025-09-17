#include <stdio.h>
#include <stdlib.h>

#include "renderStructs.h"

screenInfo* scr_init(void) {
    screenInfo* s = malloc(sizeof(screenInfo));
    s->length = 0;
    s->cols = malloc(MAX_SCREEN_COLS * sizeof(screenCol));
    if (!s->cols) { perror("malloc"); exit(EXIT_FAILURE); }
    return s;
}

void scr_add(screenInfo* s, void* col, screenColTypes typ) {
    if (s->length == MAX_SCREEN_COLS) {
        perror("Too many columns!");
        exit(EXIT_FAILURE);
    }
    s->cols[s->length++].typ = typ;
    s->cols[s->length].data = col;
}

void scr_free(screenInfo* s) {
    for (int i = 0; i < s->length; i++) {
        switch (s->cols[i].typ) {
            case WORDLIST:
                tl.free(s->cols[i].data);
        }
        free(s->cols[i].data);
    }
    free(s);
}

void scr_shuffle(screenInfo* s, int* newOrder) {
    screenInfo* tmpInfo = scr_init();
    for (int i = 0; i < s->length; i++) {
        scr_add(tmpInfo, s->cols[newOrder[i]].data, s->cols[newOrder[i]].typ);
    }
    scr_free(s);
    s = tmpInfo;
    // tmp pointer will be deleted, but the data it once pointed to is now in s (the old data in s was freed)
}

const struct scrDefStruct scr = {
    .init = scr_init,
    .add  = scr_add,
    .shuffle = scr_shuffle,
    .free = scr_free
};

