#include "actions.h"
#include "renderStructs.h"  // TRUST GUYS THIS ONE IS USED
#include "listdir.h"
#include <stdio.h>
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
    if (key == '\n') {
        textItem* it = ((textList*)s->data)->startIt;
        for (int i = 0; i < s->cursorY; i++) {
            it = it->next;
        }
        char* fname = it->text;
        if (fname[strlen(fname)-1] == '/') {
            if (strcmp(fname, "./") == 0) {
                return;
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
            s->cursorY = 0;
        }
        return;
    }
    if (key == '\x7F') {
        if (s->cursorX == 0) return;
        size_t len = strlen(s->header);
        memmove(s->header + s->cursorX - 1, s->header + s->cursorX, len - s->cursorX + 1);
        s->header = realloc(s->header, len);
        if (!s->header) { perror("realloc"); exit(EXIT_FAILURE); }
        s->cursorX--;
        return;
    }
    if (key == '\x2E') {
        size_t len = strlen(s->header);
        if (s->cursorX >= len) return;
        memmove(s->header + s->cursorX, s->header + s->cursorX + 1, len - s->cursorX);
        s->header = realloc(s->header, len);
        if (!s->header) { perror("realloc"); exit(EXIT_FAILURE); }
        return;
    }
    if (key != '/') {
        size_t nlen = strlen(s->header)+1;
        s->header = realloc(s->header, nlen+1);
        if (!s->header) { perror("realloc"); exit(EXIT_FAILURE); }
        memmove(s->header + s->cursorX + 1, s->header + s->cursorX, nlen - s->cursorX + 1);
        s->header[s->cursorX++] = key;
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
                case 'l':
                    SC.mvHCurs(s, -1);
                    return;
                case 'r':
                    SC.mvHCurs(s, 1);
                    return;
            }
    }
}

