#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

textItem* tl_get(textList* l, int idx) {
    textItem* it = l->startIt;
    for (int i = 0; i < idx; i++) {
        it = it->next;
    }
    return it;
}

void tl_sort(textList* l, tlSortFunc sortFunc) {
    if (l->length < 2) return;
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
    l->endIt = pointers[l->length - 1];
}

void toLowercase(char* inp) {
    for (size_t i = 0; inp[i] != '\0'; i++) {
        inp[i] = (char)tolower(inp[i]);
    }
}
int match(char* a, char* b) {
    char* aCopy = strdup(a);
    char* bCopy = strdup(b);
    toLowercase(aCopy);
    toLowercase(bCopy);
    int res = strstr(aCopy, bCopy) != NULL;
    free(aCopy);
    free(bCopy);
    return res;
}

textList* tl_filter(textList* l, char* filter) {
    textList* ntl = tl_init();
    textItem* it = l->startIt;
    while (it != NULL) {
        if (match(it->text, filter)) {
            tl_add(ntl, it->text);
        }
        it = it->next;
    }
    return ntl;
}

textList* tl_copy(textList* l) {
    textList* ntl = tl_init();
    textItem* it = l->startIt;
    while (it != NULL) {
        tl_add(ntl, it->text);
        it = it->next;
    }
    return ntl;
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
    .filter = tl_filter,
    .copy = tl_copy,
    .free = tl_free
};


int sortDirs(textItem** a, textItem** b) {
    const char *ta = (*a)->text;
    const char *tb = (*b)->text;

    if (strcmp(ta, "../") == 0) return -1;  // a forced in front
    if (strcmp(tb, "../") == 0) return 1;    // b forced in front
#ifdef _WIN32
    return _stricmp(ta, tb);
#else
    return strcasecmp(ta, tb);
#endif
}

const struct tlSortDefStruct tlSort = {
    .dirs = sortDirs
};

