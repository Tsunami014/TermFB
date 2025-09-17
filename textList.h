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
    void (*add)(textList* l, char* txt);
    char* (*get)(textList* l, int idx);
    void (*sort)(textList* l, int (*sortFunc)(textItem** a, textItem** b));
    void (*free)(textList* l);
};
extern const struct tlDefStruct tl;  // Functions for working with text lists; use tl.init(...), tl.add(list, ...), etc.
struct tlSortDefStruct {
    int (*alphaAsc)(textItem** a, textItem** b);  // Sort alphabetically in ascending order
    int (*alphaCIAsc)(textItem** a, textItem** b);  // Sort alphabetically Case Ignorant in ascending order
};
extern const struct tlSortDefStruct tlSort;

#endif
