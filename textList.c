#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
    #include <strings.h>
#endif

#include "textList.h"

textList* tl_init() {
    textList* l = malloc(sizeof(textList));
    if (!l) { perror("malloc"); exit(EXIT_FAILURE); }
    l->length = 0;
    l->startIt = NULL;
    l->endIt = NULL;
    return l;
}

void tl_add(textList* l, char* txt) {
    textItem* it = malloc(sizeof(textItem));
    if (!it) { perror("malloc"); exit(EXIT_FAILURE); }
    it->text = strdup(txt);
    it->next = NULL;
    if (l->length++ == 0) {
        l->startIt = it;
    } else {
        l->endIt->next = it;
    }
    l->endIt = it;
}

char* tl_get(textList* l, int idx) {
    textItem* it = l->startIt;
    for (int i = 0; i < idx; i++) {
        it = it->next;
    }
    return it->text;
}

void tl_sort(textList* l, int (*sortFunc)(textItem** a, textItem** b)) {
    // We build a list of all the pointers
    textItem* pointers[l->length];
    textItem* curPtr = l->startIt;
    int i = 0;
    while (curPtr != NULL) {
        pointers[i++] = curPtr;
        curPtr = curPtr->next;
    }
    // We sort the pointers
    qsort(pointers, l->length, sizeof(textItem*), (int (*)(const void*, const void*))sortFunc);
    // We rebuild the pointer maze
    l->startIt = pointers[0];
    for (int i = 1; i < l->length; i++) {
        pointers[i-1]->next = pointers[i];
    }
    pointers[l->length - 1]->next = NULL;
}

void tl_free(textList* l) {
    if (!l) return;
    textItem* it = l->startIt;
    while (it != NULL) {
        textItem* next = it->next;
        free(it);
        it = next;
    }
    free(l);
}

const struct tlDefStruct tl = {
    .init = tl_init,
    .add  = tl_add,
    .get  = tl_get,
    .sort = tl_sort,
    .free = tl_free
};


int sortAlphAsc(textItem** a, textItem** b) {
    return strcmp((*a)->text, (*b)->text);
}
int sortAlphCIAsc(textItem** a, textItem** b) {    
#ifdef _WIN32
    return _stricmp((*a)->text, (*b)->text);
#else
    #include <strings.h>
    return strcasecmp((*a)->text, (*b)->text);
#endif
}

const struct tlSortDefStruct tlSort = {
    .alphaAsc = sortAlphAsc,
    .alphaCIAsc = sortAlphCIAsc
};

