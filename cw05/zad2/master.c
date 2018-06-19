#define _XOPEN_SOURCE 500
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/resource.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <limits.h>

int main(int argc, char *argv[]) {

    if (argc == 2) {
        if (mkfifo(argv[1], S_IRUSR | S_IWUSR) != 0) {
            printf("Can't open file!");
            return 1;
        }
        //printf("Created FIFO %s\n", argv[1]);

        char buf[PIPE_BUF];
        //printf("%d\n", PIPE_BUF);
        int pipe_fd = open(argv[1], O_RDONLY);
        if (pipe_fd == -1) {
            printf("cant open");
            return 1;
        }
        while (read(pipe_fd, buf, PIPE_BUF)) {
            printf("%s", buf);

        }

        close(pipe_fd);
        remove(argv[1]);
    }
    return 0;
}
