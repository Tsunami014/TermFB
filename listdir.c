#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "listdir.h"

#if defined(_WIN32)
#include <windows.h>
#else
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>
#endif

directory* dir_init(char* path) {
    directory* d = malloc(sizeof(directory));
    if (!d) { perror("malloc"); exit(EXIT_FAILURE); }
    d->path = path;
    d->length = 0;
    d->capacity = 4;
    d->items = malloc(d->capacity * sizeof(char*));
    if (!d->items) { perror("malloc"); exit(EXIT_FAILURE); }
    return d;
}

void add_to_dir(directory* d, char* newIt) {
    if (d->capacity == d->length) {
        d->capacity *= 2;
        char** tmp = realloc(d->items, d->capacity * sizeof(char*));
        if (!tmp) { perror("realloc"); exit(EXIT_FAILURE); }
        d->items = tmp;
    }
    d->items[d->length++] = strdup(newIt);
}

void free_dir(directory* d) {
    if (!d) return;
    for (int i = 0; i < d->length; i++) free(d->items[i]);
    free(d->items);
    free(d);
}

directory* list_dir(char *path) {
    path = expand_tilde(path);
    directory* dir = dir_init(path);
#if defined(_WIN32)
    WIN32_FIND_DATA fd;
    char pattern[MAX_PATH];
    snprintf(pattern, sizeof pattern, "%s\\*", path);
    HANDLE h = FindFirstFile(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) return dir;
    do {
        add_to_dir(dir, fd.cFileName);
    } while (FindNextFile(h, &fd));
    FindClose(h);
#else
    DIR *d = opendir(path);
    if (!d) return dir;
    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        add_to_dir(dir, ent->d_name);
    }
    closedir(d);
#endif
    return dir;
}

char* expand_tilde(const char *path) {
    if (!path || path[0] != '~')
        return strdup(path ? path : "");

    const char *home = NULL;

#ifdef _WIN32
    home = getenv("USERPROFILE");
    if (!home) {
        const char *drive = getenv("HOMEDRIVE");
        const char *p = getenv("HOMEPATH");
        if (drive && p) {
            size_t len = strlen(drive) + strlen(p) + 1;
            char *tmp = malloc(len);
            if (!tmp) return NULL;
            snprintf(tmp, len, "%s%s", drive, p);
            home = tmp;        /* we'll free later */
        }
    }
#else
    home = getenv("HOME");
    if (!home) {
        struct passwd *pw = getpwuid(getuid());
        if (pw) home = pw->pw_dir;
    }
#endif

    if (!home) return strdup(path+1);   /* fallback: leave as-is */

    size_t hlen = strlen(home);
    size_t plen = strlen(path+1);
    char *out = malloc(hlen + plen + 1);
    if (!out) return NULL;
    strcpy(out, home);
    strcat(out, path+1);
    return out;
}

