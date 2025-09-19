#ifndef LISTDIR_H
#define LISTDIR_H
#include "dirList.h"

textList* list_dir(char *path);
char* expand_tilde(const char *path);

#endif
