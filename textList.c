#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "textList.h"

textList* dir_init(char* path) {
    textList* d = malloc(sizeof(textList));
    if (!d) { perror("malloc"); exit(EXIT_FAILURE); }
    d->path = path;
    d->length = 0;
    d->capacity = 4;
    d->items = malloc(d->capacity * sizeof(char*));
    if (!d->items) { perror("malloc"); exit(EXIT_FAILURE); }
    return d;
}

void add_to_dir(textList* d, char* newIt) {
    if (d->capacity == d->length) {
        d->capacity *= 2;
        char** tmp = realloc(d->items, d->capacity * sizeof(char*));
        if (!tmp) { perror("realloc"); exit(EXIT_FAILURE); }
        d->items = tmp;
    }
    d->items[d->length++] = strdup(newIt);
}

void free_dir(textList* d) {
    if (!d) return;
    for (int i = 0; i < d->length; i++) free(d->items[i]);
    free(d->items);
    free(d);
}

const struct tlDefStruct tl = {
    .init = dir_init,
    .add  = add_to_dir,
    .free = free_dir
};

