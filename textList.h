#ifndef TEXTL_H
#define TEXTL_H

typedef struct textItem textItem;
struct textItem{
    char* text;
    textItem* next;
};

typedef struct {
    textItem* startIt;
    textItem* endIt;
    int length;
} textList;

struct tlDefStruct {
    textList* (*init)(void);
    void (*add)(textList* d, char* txt);
    char* (*get)(textList* d, int idx);
    void (*free)(textList* d);
};
extern const struct tlDefStruct tl;  // Functions for working with text lists; use tl.init(...), tl.add(list, ...), etc.

#endif
