#ifndef ACTIONS_H
#define ACTIONS_H

#include "textList.h"

typedef enum {
    DIRECTORY_VIEW
} screenColUses;

textList* init_actions(char* startingPath);

typedef struct screenCol screenCol;  // Forward declaration
typedef struct screenInfo screenInfo;  // Forward declaration
void onKeyPress(screenInfo* screen, screenCol* s, char key);
void onArrowPress(screenInfo* screen, screenCol* s, char arrow);

#endif

