#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "actions.h"
#include "renderStructs.h"
#include "listdir.h"


void filter_dirSC(screenCol* s) {
    textList* dat = s->data;
    textList* newL = tl.filter(((dirViewInfo*)dat->info)->curDir, s->header);  // Get a filtered copy of the original
    // Delete the old filtered version
    textItem* it = dat->startIt;
    while (it != NULL) {
        textItem* next = it->next;
        free(it);
        it = next;
    }
    // Insert the new filtered copy in place
    dat->startIt = newL->startIt;
    dat->endIt = newL->endIt;
    dat->length = newL->length;
    free(newL);  // Remove the space taken up by the temporary copy; but the items are still here, now passed on to the original dat
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
    if (key == '\x7F' || key == '\b') {  // Backspace
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


void blankHeader(screenCol* s) {
    s->cursorY = 0;
    s->cursorX = 0;
    s->header = realloc(s->header, 1);  // Clear header
    if (!s->header) { perror("realloc"); exit(EXIT_FAILURE); }
    s->header[0] = '\0';
}

void onKeyPress(screenInfo* screen, screenCol* s, char key) {
    switch (s->use) {
        case DIRECTORY_VIEW:
            if (key == '\n' || key == '\r') {
                textList* txtL = s->data;
                dirViewInfo* dvi = txtL->info;
                textItem* it = txtL->startIt;
                for (int i = 0; i < s->cursorY; i++) {
                    it = it->next;
                }
                if (it == NULL) {  // Not selecting anything
                    return;
                }
                char* fname = it->text;
                if (fname[strlen(fname)-1] == '/') {
                    if (strcmp(fname, "./") == 0) {
                        return;
                    } else if (strcmp(fname, "../") == 0) {
                        dvi->path[strlen(dvi->path)-1] = '\0';  // Destroy the last /
                        const char *last = strrchr(dvi->path, '/');
                        size_t index;
                        if (last) {
                            index = (size_t)(last - dvi->path);
                        } else {
                            index = 0;
                        }

                        char* npath = malloc(index+2);
                        strncpy(npath, dvi->path, index);
                        npath[index] = '/';
                        npath[index+1] = '\0';
                        free(dvi->path);
                        dvi->path = npath;
                    } else {
                        char* npath = malloc(strlen(dvi->path)+strlen(fname)+1);
                        strcpy(npath, dvi->path);
                        strcat(npath, fname);
                        free(dvi->path);
                        dvi->path = npath;
                    }
                    char* npath = strdup(dvi->path);
                    dl.free(s->data);
                    s->data = list_dir(npath);
                    tl.sort(s->data, tlSort.alphaCIAsc);
                    dl.setup(s->data, npath);
                    blankHeader(s);
                }
                return;
            }
            onDirectoryKeyPress(screen, s, key);
            break;
        case DIRECTORY_SELECT:
            if (key == '\n' || key == '\r') {
                for (int i = 0; i < screen->length; i++) {  // This means there can only be one directory view
                    if (screen->cols[i].use == DIRECTORY_VIEW) {
                        screenCol* col = &screen->cols[i];
                        blankHeader(col);  // Clear next column's header
                        char* path = expand_tilde(tl.get(col->data, col->cursorY));
                        textList* dir = list_dir(path);
                        tl.sort(dir, tlSort.alphaCIAsc);
                        dl.setup(dir, path);
                        dl.free(col->data);
                        col->data = dir;
                        screen->cursorCol = i;  // Select the dir view
                        blankHeader(s);  // Clear this column's filter
                        filter_dirSC(s);  // Unfilter
                        return;
                    }
                }
                return;
            }
            onDirectoryKeyPress(screen, s, key);
            break;
    }
}

void onArrowPress(screenInfo* screen, screenCol* s, char arrow) {
    switch (s->typ) {
        case WORDLIST:
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

