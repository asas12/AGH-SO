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

int stringToInt(char *s) {
    int result = 0;
    for (int i = 0; i < strlen(s); i++) {
        result *= 10;
        result += (s[i] - '0');
    }
    return result;
}

int main(int argc, char *argv[]) {

    if (argc == 3) {
        srand(time(0));
        int pipe_fd = open(argv[1], O_WRONLY);
        if (pipe_fd == -1) {
            printf("cant open");
            return 1;
        }
        printf("My pid from STDIO: %d\n", getpid());

        char buf1[PIPE_BUF];
        char buf2[PIPE_BUF];

        for (int i = 0; i < stringToInt(argv[2]); i++) {
            FILE *date = popen("date", "r");
            fgets(buf1, PIPE_BUF, date);
            sprintf(buf2, "My pid from pipe: %d, date: %s\n", getpid(), buf1);
            write(pipe_fd, buf2, strlen(buf2));
            pclose(date);
            sleep((rand() % 4) + 1);
        }

        close(pipe_fd);
        remove(argv[1]);
    } else { printf("Wrong argc\n"); }
    return 0;
}
