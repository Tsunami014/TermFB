#ifndef LISTDIR_H
#define LISTDIR_H

typedef struct directory {
    char* path;
    char** items;
    int length;
    int capacity;
} directory;

directory* list_dir(char *path);
char* expand_tilde(const char *path);

#endif
