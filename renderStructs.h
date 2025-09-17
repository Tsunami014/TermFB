#ifndef RENDER_STRUCTS_H
#define RENDER_STRUCTS_H

#include "textList.h"

#define MAX_SCREEN_COLS 3

typedef enum {
    WORDLIST
} screenColTypes;

typedef struct {
    screenColTypes typ;
    void* data;
    int lastOffset;
    void* renderData;  // This is only updated, and therefore used, when rendering
} screenCol;

struct SCDefStruct {
    void (*init)(screenCol* col);
    char* (*step)(screenCol* col);
    int (*len)(screenCol* col);
    void (*offset)(screenCol* col, int cursorRow, int maxRows);
    void (*free)(screenCol* col);
};
extern const struct SCDefStruct SC;

typedef struct {
    int cursorCol;
    int cursorRow;
    int length;
    screenCol* cols;
} screenInfo;

struct scrDefStruct {
    screenInfo* (*init)(void);
    void (*add)(screenInfo* s, void* col, screenColTypes typ);
    void (*setCur)(screenInfo* s, int newCol, int newRow);
    void (*updCur)(screenInfo* s, int columnDiff, int rowDiff);
    void (*shuffle)(screenInfo* s, int* newOrder);
    void (*free)(screenInfo* s);
};
extern const struct scrDefStruct scr;

#endif

