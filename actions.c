#include "actions.h"
#include "renderStructs.h"  // TRUST GUYS THIS ONE IS USED
#include "listdir.h"
#include <stdlib.h>
#include <string.h>

// NOTE: This is the file where I make assumptions that there is only one of each screenColUses on the screen at a time

typedef struct {
    char* path;
} dirViewInfo;

static dirViewInfo DVI;

void init_actions(char* startingPath) {
    DVI.path = strdup(startingPath);
}


void onDirectoryKeyPress(screenInfo* screen, screenCol* s, char key) {
    switch (key) {
        case '\n': {
            textItem* it = ((textList*)s->data)->startIt;
            for (int i = 0; i < s->cursorY; i++) {
                it = it->next;
            }
            char* fname = it->text;
            if (fname[strlen(fname)-1] == '/') {
                if (strcmp(fname, "./") == 0) {
                    break;
                } else if (strcmp(fname, "../") == 0) {
                    DVI.path[strlen(DVI.path)-1] = '\0';  // Destroy the last /
                    const char *last = strrchr(DVI.path, '/');
                    size_t index;
                    if (last) {
                        index = (size_t)(last - DVI.path);
                    } else {
                        index = 0;
                    }

                    char* npath = malloc(index+2);
                    strncpy(npath, DVI.path, index);
                    npath[index] = '/';
                    npath[index+1] = '\0';
                    free(DVI.path);
                    DVI.path = npath;
                } else {
                    char* npath = malloc(strlen(DVI.path)+strlen(fname)+1);
                    strcpy(npath, DVI.path);
                    strcat(npath, fname);
                    free(DVI.path);
                    DVI.path = npath;
                }
                tl.free(s->data);
                s->data = list_dir(DVI.path);
                tl.sort(s->data, tlSort.alphaCIAsc);
                SC.mvSelect(s, 0);
            }
            break;
        }
    }
}


void onKeyPress(screenInfo* screen, screenCol* s, char key) {
    switch (s->use) {
        case DIRECTORY_VIEW:
            onDirectoryKeyPress(screen, s, key);
            break;
    }
}

void onArrowPress(screenInfo* screen, screenCol* s, char arrow) {
    switch (s->use) {
        case DIRECTORY_VIEW:
            switch (arrow) {
                case 'u':  // Up arrow
                    SC.mvSelect(s, -1);
                    return;
                case 'd':  // Down arrow
                    SC.mvSelect(s, 1);
                    return;
            }
    }
}

