#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#include "renderStructs.h"
#include "actions.h"


void SC_init(screenCol* col) {
    switch (col->typ) {
        case WORDLIST:
            col->renderData = ((textList*)col->data)->startIt;
            for (int i = 0; i < col->lastOffset; i++) {
                col->renderData = ((textItem*)col->renderData)->next;
            }
            return;
        case TEMPORARY:;
            tmpRendDat* rendDat = col->renderData;
            rendDat->tmpnxt = rendDat->nxt;
            rendDat->rendOffs = 0;
            return;
    }
}


char* SC_step(screenCol* col, int scrWid) {
    switch (col->typ) {
        case WORDLIST:
            if (col->renderData == NULL) {
                return NULL;
            }
            char* ret = ((textItem*)col->renderData)->text;
            col->renderData = ((textItem*)col->renderData)->next;
            return strdup(ret);
        case TEMPORARY:;
            tmpRendDat* rd = col->renderData;
            if (rd->tmpnxt == NULL) {
                return NULL;
            }
            char* txt = rd->tmpnxt->text + rd->rendOffs;
            int len = strlen(txt);
            int xtra = len >= scrWid;
            if (xtra) {
                len = scrWid-1;
                rd->rendOffs += len;
            } else {
                rd->tmpnxt = rd->tmpnxt->next;
                rd->rendOffs = 0;
            }
            char* nstr = malloc(len+xtra+1);
            if (!nstr) { perror("malloc"); exit(EXIT_FAILURE); }
            while (len > 0 && (txt[len-1] == '\r')) len--;
            memcpy(nstr, txt, len);
            if (xtra) {
                nstr[len] = '\\';
                nstr[len + 1] = '\0';
            } else {
                nstr[len] = '\0';
            }
            return nstr;
    }
}

int SC_len(screenCol* col) {
    switch (col->typ) {
        case TEMPORARY:
        case WORDLIST:
            return ((textList*)col->data)->length;
    }
}

void SC_offset(screenCol* col, int maxRows) {
    int hMaxRs = maxRows / 2;
    int offset = col->cursorY - hMaxRs;
    if (offset <= 0) {
        offset = 0;
    } else {
        int len = SC_len(col);
        int maxOffset = len - maxRows;
        if (maxOffset <= 0) {
            offset = 0;
        } else if (offset > maxOffset) {
            offset = maxOffset;
        }
    }
    col->lastOffset = offset;
}

void SC_move_selection(screenCol* col, int chngRows) {
    int new = col->cursorY + chngRows;
    if (new < 0) {
        new = 0;
        if (chngRows < 0) {
            new = SC_len(col) + chngRows;
        }
    } else {
        int max = SC_len(col) - 1;
        if (new > max) {
            if (chngRows > 0) {
                new = chngRows - 1;
            } else {
                new = max;
            }
        }
    }
    col->cursorY = new;
}
void SC_move_Header_Cursor(screenCol* col, int dx) {
    int new = col->cursorX + dx;
    if (new <= 0) {
        new = 0;
    } else {
        int max;
        if (col->selectingRow) {
            max = strlen(col->selectedTxt);
        } else {
            max = strlen(col->header);
        }
        if (new > max) {
            new = max;
        }
    }
    col->cursorX = new;
}

void SC_free(screenCol* col) {
    free(col->header);
    if (col->selectedTxt != NULL) {
        free(col->selectedTxt);
    }
    switch (col->typ) {
        case TEMPORARY:
            free(col->renderData);
        case WORDLIST:
            tl.free(col->data);
            break;
    }
}


const struct SCDefStruct SC = {
    .init = SC_init,
    .step = SC_step,
    .len = SC_len,
    .offset = SC_offset,
    .mvSelect = SC_move_selection,
    .mvHCurs = SC_move_Header_Cursor,
    .free = SC_free
};



screenInfo* scr_init(void) {
    screenInfo* s = malloc(sizeof(screenInfo));
    s->length = 0;
    s->cursorCol = 0;
    s->cols = malloc(MAX_SCREEN_COLS * sizeof(screenCol));
    if (!s->cols) { perror("malloc"); exit(EXIT_FAILURE); }
    return s;
}

void scr_add(screenInfo* s, void* col, screenColTypes typ, screenColUses use) {
    if (s->length == MAX_SCREEN_COLS) {
        perror("Too many columns!");
        exit(EXIT_FAILURE);
    }
    screenCol* ncol = &s->cols[s->length++];
    ncol->typ = typ;
    ncol->use = use;
    ncol->data = col;
    ncol->header = malloc(1);
    if (!ncol->header) { perror("malloc"); exit(EXIT_FAILURE); }
    ncol->header[0] = '\0';
    ncol->lastCursorX = 0;
    ncol->cursorX = 0;
    ncol->cursorY = 0;
    ncol->selectingRow = 0;
    ncol->selectedTxt = NULL;
    ncol->lastOffset = 0;
    if (typ == TEMPORARY) {
        tmpRendDat* newRD = malloc(sizeof(tmpRendDat));
        if (!newRD) { perror("malloc"); exit(EXIT_FAILURE); }
        newRD->nxt = ((textList*)col)->startIt;
        newRD->tmpnxt = ((textList*)col)->startIt;
        ncol->renderData = newRD;
    }
}

void scr_setCur(screenInfo* s, int newCol) {
    if (newCol <= 0) {
        s->cursorCol = 0;
    } else {
        if (newCol >= s->length) {
            s->cursorCol = s->length-1;
        } else {
            s->cursorCol = newCol;
        }
    }
}
void scr_updCur(screenInfo* s, int dc) {
    scr_setCur(s, s->cursorCol + dc);
}

void scr_free(screenInfo* s) {
    for (int i = 0; i < s->length; i++) {
        SC.free(&s->cols[i]);
    }
    free(s);
}

void scr_shuffle(screenInfo* s, int* newOrder) {
    screenInfo* tmpInfo = scr_init();
    for (int i = 0; i < s->length; i++) {
        screenCol col = s->cols[newOrder[i]];
        scr_add(tmpInfo, col.data, col.typ, col.use);
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

void makeTempCol(screenInfo* screen, char* txt, screenColUses use) {
    for (int i = 0; i < screen->length; i++) {
        if (screen->cols[i].use == use) {
            free(txt);
            return;  // No duplicate screens
        }
    }
    textList* outL = tl.init();
    char* tok = strtok(txt, "\n");
    while (tok != NULL) {
        tl.add(outL, tok);
        tok = strtok(NULL, "\n");
    }
    free(txt);

    scr.add(screen, outL, TEMPORARY, use);
    screen->cursorCol = screen->length-1;
}

