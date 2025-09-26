#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "config.h"

const char* appName = "termFB";

const char defaultConf[] = 
    "~/\n"
    "~/Downloads/\n" "~/Documents/\n" "~/Desktop/\n" "~/Pictures/\n"
    ; 

void create_dir_if_not_exists(const char *path) {
#ifdef _WIN32
    _mkdir(path);
#else
    mkdir(path, 0700); // rwx for user only
#endif
}

char* confPath = NULL;

void get_config_path() {
    const int pathSze = 512;
    char path[512];  // In people not having stupidly long home directories we trust!
#ifdef _WIN32
    const char *base = getenv("APPDATA");
    if (!base) base = "C:\\";
    snprintf(path, pathSze, "%s\\%s", base, appName);
    create_dir_if_not_exists(path);
    strncat(path, "\\config", pathSze - strlen(path) - 1);
#else
    const char *base = getenv("XDG_CONFIG_HOME");
    if (!base) {
        const char *home = getenv("HOME");
        if (!home) home = "/tmp"; // fail-safe
        snprintf(path, pathSze, "%s/.config/%s", home, appName);
    } else {
        snprintf(path, pathSze, "%s/%s", base, appName);
    }
    create_dir_if_not_exists(path);
    strncat(path, "/config", pathSze - strlen(path) - 1);
#endif
    confPath = malloc(strlen(path)+1);
    if (!confPath) { perror("malloc"); exit(EXIT_FAILURE); }
    strcpy(confPath, path);

    FILE *f = fopen(path, "r");
    if (f) {
        fclose(f);
        return;  // File already exists
    }
    // Does not exist
    f = fopen(path, "w+");
    if (!f) {
        perror("fopen");
        return;
    }
    fprintf(f, "%s", defaultConf);
    fclose(f);
}

textList* init_config() {
    get_config_path();

    textList* dirs = tl.init();

    FILE* file = fopen(confPath, "r");
    if (!file) {
        perror("fopen");
        return dirs;
    }
    char line[256];  // Again, we trust in out users so heavily that their folders are not stupidly long. But only the ones stored in the config...

    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) <= 1) continue;
        line[strlen(line)-1] = '\0';  // Ensure the last newline is deleted
        tl.add(dirs, line);
    }
    fclose(file);

    dl.setup(dirs, confPath);
    return dirs;
}

