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
    void* info;  // For other stuff to do with as they please, not kept track of (OR FREED) by this at all.
} textList;

typedef int (*tlSortFunc)(textItem** a, textItem** b);

struct tlDefStruct {
    textList* (*init)(void);
    void (*add)(textList* l, char* txt);
    textItem* (*get)(textList* l, int idx);
    void (*sort)(textList* l, tlSortFunc sortFunc);
    textList* (*filter)(textList* l, char* filter);
    textList* (*copy)(textList* l);
    void (*free)(textList* l);
};
extern const struct tlDefStruct tl;  // Functions for working with text lists; use tl.init(...), tl.add(list, ...), etc.
struct tlSortDefStruct {
    tlSortFunc dirs;  // Sort alphabetically in ascending order, case ignorant, ensuring '../' goes to the top
};
extern const struct tlSortDefStruct tlSort;

#endif
