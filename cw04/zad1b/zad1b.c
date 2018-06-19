#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

pid_t childPID = -1;

void catchSIGTSTP(int signo);


void catchSIGINT(int signo) {
    printf("\nOdebrano sygnal SIGINT\n");
//    kill(childPID, 9);
    exit(0);
}

void catch2ndSIGTSTP(int signo) {
    printf("\n");
    childPID = fork();
    if (childPID == 0) {
        if (execl("./script.sh", "") != 0) {
            printf("ERR!");
            exit(EXIT_FAILURE);
        }
    } else {
        struct sigaction act;
        act.sa_handler = catchSIGINT;
        sigaction(SIGINT, &act, NULL);
        signal(SIGTSTP, catchSIGTSTP);
        pause();
    }

}

void catchSIGTSTP(int signo) {
    kill(childPID, 9);
    printf("\nOczekuje na CTRL+Z - kontynuacja albo CTRL+C - zakonczenie programu\n");
    struct sigaction act;
    act.sa_handler = catchSIGINT;
    sigaction(SIGINT, &act, NULL);
    signal(SIGTSTP, catch2ndSIGTSTP);
    pause();
}

int main(void) {

    childPID = fork();
    if (childPID == 0) {
        if (execl("./script.sh", "") != 0) { exit(EXIT_FAILURE); }
    } else {
        struct sigaction act;
        act.sa_handler = catchSIGINT;
        sigaction(SIGINT, &act, NULL);
        signal(SIGTSTP, catchSIGTSTP);
        pause();
    }

    return 0;
}
