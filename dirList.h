#ifndef DIRLIST_H
#define DIRLIST_H

#include "textList.h"

typedef struct {
    char* path;
    textList* curDir;
} dirViewInfo;

struct dlDefStruct {
    void (*setup)(textList* d, char* path);
    textList* (*filter)(textList* d, char* filter);
    textList* (*copy)(textList* l);
    void (*free)(textList* l);
};
extern const struct dlDefStruct dl;

#endif
