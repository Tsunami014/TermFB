#ifndef RENDER_STRUCTS_H
#define RENDER_STRUCTS_H

#include "textList.h"
#include "actions.h"

#define MAX_SCREEN_COLS 4

typedef struct {
    textItem* nxt;
    textItem* tmpnxt;
    int rendOffs;  // Offset while rendering for lines that have been broken up
} tmpRendDat;

typedef enum {
    WORDLIST,
    TEMPORARY,
} screenColTypes;

struct screenCol {
    // Behaviours
    screenColTypes typ;
    screenColUses use;
    // The actual column render object
    void* data;
    // Header data
    char* header;
    // Cursor stuff
    int cursorY;
    int cursorX;
    int lastCursorX;
    int selectingRow;
    char* selectedTxt;
    // Render stuff
    int lastOffset;
    void* renderData;  // Implementation varies based on typ
};

struct SCDefStruct {
    void (*init)(screenCol* col);  // The start of the render loop
    char* (*step)(screenCol* col, int scrWid);  // Each step along the render loop, returning the row
    int (*len)(screenCol* col);
    void (*offset)(screenCol* col, int maxRows);
    void (*mvSelect)(screenCol* col, int chngRows);
    void (*mvHCurs)(screenCol* col, int dx);
    void (*free)(screenCol* col);
};
extern const struct SCDefStruct SC;

struct screenInfo{
    int cursorCol;
    int length;
    screenCol* cols;
};

struct scrDefStruct {
    screenInfo* (*init)(void);
    void (*add)(screenInfo* s, void* col, screenColTypes typ, screenColUses use);
    void (*setCur)(screenInfo* s, int newCol);
    void (*updCur)(screenInfo* s, int columnDiff);
    void (*shuffle)(screenInfo* s, int* newOrder);
    void (*free)(screenInfo* s);
};
extern const struct scrDefStruct scr;

void makeTempCol(screenInfo* screen, char* txt);

#endif

