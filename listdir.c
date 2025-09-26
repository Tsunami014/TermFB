#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "listdir.h"

#if defined(_WIN32)
    #include <windows.h>
#else
    #include <dirent.h>
    #include <sys/stat.h>
    #include <pwd.h>
    #include <unistd.h>
#endif

textList* list_dir(char* path) {
    textList* dir = tl.init();
#if defined(_WIN32)
    char* fixedPth = strdup(path);
    for (int i = 0; i < strlen(fixedPth); i++) {
        if (fixedPth[i] == '/') fixedPth[i] = '\\';
    }
    WIN32_FIND_DATA fd;
    char pattern[strlen(fixedPth)+3]; /* +2 for \* +1 for \0 */
    if (fixedPth[strlen(fixedPth)-1] != '\\') {
        snprintf(pattern, sizeof pattern, "%s\\*", fixedPth);
    } else {
        snprintf(pattern, sizeof pattern, "%s*", fixedPth);
    }
    free(fixedPth);
    HANDLE h = FindFirstFile(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) {
        tl.add(dir, "../");
        return dir;
    }
    if (strlen(path) <= 3) {  // Bcos C:/ is 3 characters
        tl.add(dir, "./");
    }
    do {
        char* name = fd.cFileName;
        if (strlen(path) <= 3) {  // Bcos C:/ is 3 characters
            if (strcmp(name, "..") == 0) continue;
        }
        if (strcmp(name, ".") == 0) continue;  // Any extra ./s will be removed (already added one at the start where required)

        char namebuf[strlen(name)+2]; /* +1 for '/' +1 for '\0' */
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            sprintf(namebuf, "%s/", name);
        } else {
            sprintf(namebuf, "%s", name);
        }

        tl.add(dir, namebuf);
    } while (FindNextFile(h, &fd));
    FindClose(h);
#else
    DIR *d = opendir(path);
    if (!d) {
        tl.add(dir, "../");
        return dir;
    }
    struct dirent *ent;
    struct stat st;
    int baseSze = strlen(path)+2;
    while ((ent = readdir(d)) != NULL) {
        char* name = ent->d_name;
        if (strcmp(name, ".") == 0 && baseSze > 3) continue;
        char fullpath[baseSze+strlen(name)];
        sprintf(fullpath, "%s%s", path, name);
        if (strcmp(fullpath, "/..") == 0) continue;
 
        if (stat(fullpath, &st) != 0) {  // Error occurred
            continue;
        }

        if (stat(fullpath, &st) == 0 && S_ISDIR(st.st_mode)) {
            char namebuf[strlen(name)+2];
            sprintf(namebuf, "%s/", name);
            tl.add(dir, namebuf);
        } else {
            tl.add(dir, name);
        }
    }

    closedir(d);
#endif

    dl.setup(dir, path);
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

