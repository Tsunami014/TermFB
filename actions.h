#ifndef ACTIONS_H
#define ACTIONS_H

typedef enum {
    DIRECTORY_VIEW
} screenColUses;

void init_actions(char* startingPath);

typedef struct screenCol screenCol;  // Forward declaration
void onKeyPress(screenCol* s, int cursorRow, char key);

#endif

