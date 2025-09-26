#ifndef RENDER_H
#define RENDER_H

#include "renderStructs.h"

void printScrn(screenInfo* screen);

extern char* helpTxt;
void init_help();
void onExit(screenInfo* screen);

#endif
