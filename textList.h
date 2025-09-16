#ifndef TEXTL_H
#define TEXTL_H

typedef struct {
    char* path;
    char** items;
    int length;
    int capacity;
} textList;

struct tlDefStruct {
    textList* (*init)(char* path);
    void (*add)(textList* d, char* newIt);
    void (*free)(textList* d);
};
extern const struct tlDefStruct tl;  // Functions for working with text lists; use tl.init(...), tl.add(list, ...), etc.

#endif
