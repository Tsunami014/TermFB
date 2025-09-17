#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    .free = tl_free
};

