#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dirList.h"


void dl_setup(textList* d, char* path) {
    dirViewInfo* dvi = malloc(sizeof(dirViewInfo));
    if (!dvi) { perror("malloc"); exit(EXIT_FAILURE); }
    dvi->path = strdup(path);
    dvi->curDir = tl.copy(d);
    d->info = dvi;
}

textList* dl_filter(textList* d, char* filter) {
    textList* nd = tl.filter(d, filter);
    dl_setup(nd, ((dirViewInfo*)d->info)->path);
    return nd;
}

textList* dl_copy(textList* d) {
    textList* nd = tl.copy(d);
    dirViewInfo* dvi = malloc(sizeof(dirViewInfo));
    if (!dvi) { perror("malloc"); exit(EXIT_FAILURE); }
    dirViewInfo* inf = d->info;
    dvi->path = strdup(inf->path);
    dvi->curDir = tl.copy(inf->curDir);
    nd->info = dvi;
    return nd;
}

void dl_free(textList* d) {
    dirViewInfo* inf = d->info;
    free(inf->path);
    tl.free(inf->curDir);
    tl.free(d);
}

const struct dlDefStruct dl = {
    .setup = dl_setup,
    .filter = dl_filter,
    .copy = dl_copy,
    .free = dl_free
};
