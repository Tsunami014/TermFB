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

typedef int (*tlSortFunc)(textItem** a, textItem** b);

struct tlDefStruct {
    textList* (*init)(void);
    void (*add)(textList* l, char* txt);
    char* (*get)(textList* l, int idx);
    void (*sort)(textList* l, tlSortFunc sortFunc);
    void (*free)(textList* l);
};
extern const struct tlDefStruct tl;  // Functions for working with text lists; use tl.init(...), tl.add(list, ...), etc.
struct tlSortDefStruct {
    tlSortFunc alphaAsc;  // Sort alphabetically in ascending order
    tlSortFunc alphaCIAsc;  // Sort alphabetically Case Ignorant in ascending order
};
extern const struct tlSortDefStruct tlSort;

#endif
