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
    void* renderData;  // This is initialised, used and freed at the renderer's request.
} screenCol;

void initialiseScreenCol(screenCol* col);
char* stepScreenCol(screenCol* col);

typedef struct {
    int length;
    screenCol* cols;
} screenInfo;

struct scrDefStruct {
    screenInfo* (*init)(void);
    void (*add)(screenInfo* s, void* col, screenColTypes typ);
    void (*shuffle)(screenInfo* s, int* newOrder);
    void (*free)(screenInfo* s);
};
extern const struct scrDefStruct scr;

#endif

