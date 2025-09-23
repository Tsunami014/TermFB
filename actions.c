#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "actions.h"
#include "getch.h"
#include "renderStructs.h"
#include "listdir.h"


void tryEdit(screenCol* s, char* tmpl, int cursorPos) {
    if (!s->selectingRow) {
        char* text = tl.get(s->data, s->cursorY)->text;
        if (strcmp(text, "../") == 0) {
            return;
        }
        s->selectingRow = 1;
        s->lastCursorX = s->cursorX;

        // Find the new length
        int len = snprintf(NULL, 0, tmpl, text);
        if (len < 0) { perror("format error"); exit(EXIT_FAILURE); }
        // Now write knowing the length
        s->selectedTxt = malloc(len + 1);
        if (!s->selectedTxt) { perror("malloc"); exit(EXIT_FAILURE); }
        snprintf(s->selectedTxt, len + 1, tmpl, text);
        if (cursorPos >= 0) {
            s->cursorX = cursorPos;
        } else {
            s->cursorX = len - cursorPos - 1;
        }
    }
}

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
    char** editing;
    if (s->selectingRow) {
        editing = &s->selectedTxt;
    } else {
        editing = &s->header;
    }
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
        size_t len = strlen(*editing);
        memmove(*editing + s->cursorX - 1, *editing + s->cursorX, len - s->cursorX + 1);
        *editing = realloc(*editing, len);
        if (!*editing) { perror("realloc"); exit(EXIT_FAILURE); }
        s->cursorX--;
        if (!s->selectingRow) {
            filter_dirSC(s);
        }
        return;
    }
    if (key == '\x1E') {  // My delete character
        size_t len = strlen(*editing);
        if (s->cursorX >= len) return;
        memmove(*editing + s->cursorX, *editing + s->cursorX + 1, len - s->cursorX);
        *editing = realloc(*editing, len);
        if (!*editing) { perror("realloc"); exit(EXIT_FAILURE); }
        if (!s->selectingRow) {
            filter_dirSC(s);
        }
        return;
    }
    if (key >= 32 && key <= 126) {  // Printable character
        size_t nlen = strlen(*editing)+1;
        *editing = realloc(*editing, nlen+1);
        if (!*editing) { perror("realloc"); exit(EXIT_FAILURE); }
        memmove(*editing + s->cursorX + 1, *editing + s->cursorX, nlen - s->cursorX + 1);
        (*editing)[s->cursorX++] = key;
        if (!s->selectingRow) {
            filter_dirSC(s);
        }
    }
}


void blankHeader(screenCol* s) {
    s->cursorY = 0;
    s->cursorX = 0;
    s->header = realloc(s->header, 1);  // Clear header
    if (!s->header) { perror("realloc"); exit(EXIT_FAILURE); }
    s->header[0] = '\0';
}

void onKeyPress(screenInfo* screen, screenCol* s, char key) {
    if (s->use == DIRECTORY_VIEW || s->use == DIRECTORY_SELECT) {
        if (s->use == DIRECTORY_VIEW) {
            if (key == toCtrl('r')) {
                tryEdit(s, "mv %1$s %1$s", -1);
                return;
            }
            if (key == toCtrl('m')) {
                tryEdit(s, "mv %s ", -1);
                return;
            }
            if (key == toCtrl('d')) {
                tryEdit(s, "rm %s", 3);
                return;
            }
            if (key == '\n' || key == '\r') {
                if (s->selectingRow) {
                    char* pth = ((dirViewInfo*)((textList*)s->data)->info)->path;
                    char fullCmd[3+strlen(pth)+1+strlen(s->selectedTxt)];
                    strcpy(fullCmd, "cd ");
                    strcat(fullCmd, pth);
                    strcat(fullCmd, ";");
                    strcat(fullCmd, s->selectedTxt);
                    system(fullCmd);
                    textList* newL = list_dir(pth);  // Do this before freeing so the path doesn't turn to mush
                    tl.sort(newL, tlSort.dirs);
                    dl.free(s->data);
                    s->data = newL;

                    s->selectingRow = 0;
                    s->cursorX = s->lastCursorX;
                    free(s->selectedTxt);
                    s->selectedTxt = NULL;
                    return;
                }
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
                    tl.sort(s->data, tlSort.dirs);
                    dl.setup(s->data, npath);
                    blankHeader(s);
                }
                return;
            }
        } else {
            if (key == toCtrl('r') || key == toCtrl('m')) {
                tryEdit(s, "%s", -1);
                return;
            }
            if (key == toCtrl('d')) {
                textList* l = s->data;
                if (s->cursorY == 0) {
                    if (l->startIt == NULL) return;
                    if (l->length-- == 0) {
                        free(l->startIt);
                        l->startIt = NULL;
                        l->endIt = NULL;
                    } else {
                        l->startIt = l->startIt->next;
                    }
                } else {
                    textItem* befIt = tl.get(l, s->cursorY-1);
                    if (s->cursorY == --l->length) {
                        befIt->next = NULL;
                        l->endIt = befIt;
                        s->cursorY--;
                    } else {
                        befIt->next = befIt->next->next;
                    }
                }
                return;
            }
            if (key == '\n' || key == '\r') {
                if (s->selectingRow) {
                    s->selectingRow = 0;
                    // Swap the pointers; the text now becomes the selected text and selected text becomes NULL
                    char** txt = &tl.get((textList*)s->data, s->cursorY)->text;
                    free(*txt);  // Remove old text
                    *txt = s->selectedTxt;
                    s->selectedTxt = NULL;
                    s->cursorX = s->lastCursorX;
                    return;
                }
                for (int i = 0; i < screen->length; i++) {  // This means there can only be one directory view
                    if (screen->cols[i].use == DIRECTORY_VIEW) {
                        screenCol* col = &screen->cols[i];
                        blankHeader(col);  // Clear next column's header
                        char* path = expand_tilde(tl.get(col->data, col->cursorY)->text);
                        textList* dir = list_dir(path);
                        tl.sort(dir, tlSort.dirs);
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
        }
        onDirectoryKeyPress(screen, s, key);
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

