#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "renderStructs.h"

screenInfo* scr_init(void) {
    screenInfo* s = malloc(sizeof(screenInfo));
    s->length = 0;
    s->cursorCol = 0;
    s->cursorRow = 0;
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
    s->cols[s->length-1].data = col;
}

void scr_setCur(screenInfo* s, int newCol, int newRow) {
    if (newCol <= 0) {
        s->cursorCol = 0;
    } else {
        if (newCol >= s->length) {
            s->cursorCol = s->length-1;
        } else {
            s->cursorCol = newCol;
        }
    }
    if (newRow <= 0) {
        s->cursorRow = 0;
    } else {
        int columnLen;
        screenCol col = s->cols[s->cursorCol];
        switch (col.typ) {
            case WORDLIST:
                columnLen = ((textList*)col.data)->length;
                break;
        }
        // TODO: When I implement scrolling, do not use this at all
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        if (w.ws_row - 2 < columnLen) {
            columnLen = w.ws_row - 2;
        }
        if (newRow >= columnLen) {
            s->cursorRow = columnLen-1;
        } else {
            s->cursorRow = newRow;
        }
    }
}
void scr_updCur(screenInfo* s, int dc, int dr) {
    scr_setCur(s, s->cursorCol + dc, s->cursorRow + dr);
}

void scr_free(screenInfo* s) {
    for (int i = 0; i < s->length; i++) {
        switch (s->cols[i].typ) {
            case WORDLIST:
                tl.free(s->cols[i].data);
                break;
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
    .setCur = scr_setCur,
    .updCur = scr_updCur,
    .shuffle = scr_shuffle,
    .free = scr_free
};


void initialiseScreenCol(screenCol* col) {
    switch (col->typ) {
        case WORDLIST:
            col->renderData = ((textList*)col->data)->startIt;
            return;
    }
}

char* stepScreenCol(screenCol* col) {
    switch (col->typ) {
        case WORDLIST:
            if (col->renderData == NULL) {
                return NULL;
            }
            char* ret = ((textItem*)col->renderData)->text;
            col->renderData = ((textItem*)col->renderData)->next;
            return ret;
    }
}

