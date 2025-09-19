#include "actions.h"
#include "renderStructs.h"
#include "listdir.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// NOTE: This is the file where I make assumptions that there is only one of each screenColUses on the screen at a time

typedef struct {
    char* path;
    textList* curDir;
} dirViewInfo;

static dirViewInfo DVI;

textList* init_actions(char* startingPath) {
    DVI.path = strdup(startingPath);
    textList* dir = list_dir(startingPath);
    tl.sort(dir, tlSort.alphaCIAsc);
    DVI.curDir = tl.filter(dir, "");  // Copy
    return dir;
}

void filter_dirSC(screenCol* s) {
    tl.free(s->data);
    s->data = tl.filter(DVI.curDir, s->header);
    SC.mvSelect(s, 0);
}

void onDirectoryKeyPress(screenInfo* screen, screenCol* s, char key) {
    if (key == '\t') {
        onArrowPress(screen, s, 'd');
        return;
    }
    if (key == '\x1F') {  // My custom 'shift-tab' key
        onArrowPress(screen, s, 'u');
        return;
    }
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
            tl.free(DVI.curDir);
            s->data = list_dir(DVI.path);
            tl.sort(s->data, tlSort.alphaCIAsc);
            DVI.curDir = tl.filter(s->data, "");  // Copy
            s->cursorY = 0;
            s->cursorX = 0;
            s->header = realloc(s->header, 1);  // Clear header
            if (!s->header) { perror("realloc"); exit(EXIT_FAILURE); }
            s->header[0] = '\0';
        }
        return;
    }
    if (key == '\x7F') {  // Backspace
        if (s->cursorX == 0) return;
        size_t len = strlen(s->header);
        memmove(s->header + s->cursorX - 1, s->header + s->cursorX, len - s->cursorX + 1);
        s->header = realloc(s->header, len);
        if (!s->header) { perror("realloc"); exit(EXIT_FAILURE); }
        s->cursorX--;
        filter_dirSC(s);
        return;
    }
    if (key == '\x1E') {  // My delete character
        size_t len = strlen(s->header);
        if (s->cursorX >= len) return;
        memmove(s->header + s->cursorX, s->header + s->cursorX + 1, len - s->cursorX);
        s->header = realloc(s->header, len);
        if (!s->header) { perror("realloc"); exit(EXIT_FAILURE); }
        filter_dirSC(s);
        return;
    }
    size_t nlen = strlen(s->header)+1;
    s->header = realloc(s->header, nlen+1);
    if (!s->header) { perror("realloc"); exit(EXIT_FAILURE); }
    memmove(s->header + s->cursorX + 1, s->header + s->cursorX, nlen - s->cursorX + 1);
    s->header[s->cursorX++] = key;
    filter_dirSC(s);
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

