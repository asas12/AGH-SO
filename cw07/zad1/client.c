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
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include "ipcutil.h"

int stringToInt(char *s) {
    int result = 0;
    for (int i = 0; i < strlen(s); i++) {
        result *= 10;
        result += (s[i] - '0');
    }
    return result;
}

//resources
char *buffer;
int sendMsgqID;
struct timespec tp;
int shaved = 0;
struct sembuf recMsgSem;
struct sembuf sendMsgSem;
int semID;

void catchSIGINT(int signo) {
    printf("\nOdebrano sygnal SIGINT\n");


    free(buffer);
    exit(0);
}

void catchSIGUSR1(int signo, siginfo_t *info, void *ucontext) {


    //recMsgSem.sem_op =-1;
    //semop(semID, &recMsgSem, 1);

    //sat in seat
    clock_gettime(CLOCK_MONOTONIC, &tp);
    printf("%d\t%d.%ld\tSat in barber's seat.\n", getpid(), (int) tp.tv_sec, tp.tv_nsec);
    //kill(info->si_pid, SIGUSR1);
    sendMsgSem.sem_op = 1;
    semop(semID, &sendMsgSem, 1);

    //waiting for shave to complete
    //sigwait(&sigmask, &sig);
    recMsgSem.sem_op = -1;
    semop(semID, &recMsgSem, 1);


    //leave
    clock_gettime(CLOCK_MONOTONIC, &tp);
    printf("%d\t%d.%ld\tI'm leaving.\n", getpid(), (int) tp.tv_sec, tp.tv_nsec);
    //kill(info->si_pid, SIGUSR1);
    sendMsgSem.sem_op = 1;
    semop(semID, &sendMsgSem, 1);

    shaved = 1;
}

void catchSIGUSR2(int signo) {
    printf("%d\tOdebrano sygnal SIGUSR2\n", getpid());
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Wrong number of arguments!\n");
        return 1;
    }

    int noOfClients = stringToInt(argv[1]);
    int noOfShaves = stringToInt(argv[2]);

    struct sigaction act;
    act.sa_handler = catchSIGINT;
    sigaction(SIGINT, &act, NULL);

    act.sa_sigaction = catchSIGUSR1;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);

    act.sa_handler = catchSIGUSR2;
    sigaction(SIGUSR2, &act, NULL);


//opening public queue
    const char *homePath;
    if ((homePath = getenv("HOME")) == NULL) {
        printf("Can't get gome variable.\n");
    }
    key_t parentKey = ftok(homePath, PROJECT_NO);
    sendMsgqID = msgget(parentKey, 0);

//opening semaphore
    semID = semget(parentKey, 0, 0);
    struct sembuf sem;
    sem.sem_num = 0;
    sem.sem_flg = 0;

    recMsgSem.sem_num = 2;
    recMsgSem.sem_flg = 0;

    sendMsgSem.sem_num = 1;
    sendMsgSem.sem_flg = 0;

//opening shared memory
    int shmID = shmget(parentKey, 3 * sizeof(semID) + sizeof(getpid()), 0);//uwaga czy 2 arg nie ma byc 0!!!
    int *shmAddr = (int *) shmat(shmID, NULL, 0);

    size_t bufSize = MAX_MSG_SIZE;
    buffer = (char *) malloc(bufSize * sizeof(char));

    pid_t childPID;
    int i = 0;
    for (i; i < noOfClients; i++) {

        childPID = fork();
        if (childPID == 0) {
            while (noOfShaves > 0) {
                //lock FIFO and shm
                sem.sem_op = -1;
                semop(semID, &sem, 1);

                //check if barber is awake
                if (shmAddr[0]) {
                    //awake
                    if (shmAddr[2] < shmAddr[1]) {
                        //free seats
                        clock_gettime(CLOCK_MONOTONIC, &tp);
                        printf("%d\t%d.%ld\tWaiting in queue.\n", getpid(), (int) tp.tv_sec, tp.tv_nsec);

                        //more people in queue
                        shmAddr[2]++;

                        //sitting in the queue
                        struct myMsgBuf mMsg;
                        mMsg.pid = getpid();
                        mMsg.mType = 1;
                        strcpy(mMsg.msg, " ");
                        mMsg.msg[strlen(mMsg.msg) - 1] = '\0';
                        if ((i = msgsnd(sendMsgqID, (void *) &mMsg, sizeof(getpid()) + strlen(mMsg.msg) + 1, 0)) < 0) {
                            //char* buf = malloc(256);
                            //buf = strerror_r(errno, buf, 256);
                            clock_gettime(CLOCK_MONOTONIC, &tp);
                            printf("size: %ld\n", sizeof(getpid()) + strlen(mMsg.msg) + 1);
                            perror("msgsnd err:");
                            printf("%d\tCant send to %d! msgsnd:%d , %d.%ld\n", getpid(), sendMsgqID, i,
                                   (int) tp.tv_sec, tp.tv_nsec);
                            sem.sem_op = 1;
                            semop(semID, &sem, 1);
                            return 1;
                        } else {
                            //printf("Sent: %d, msg: %s to: %d\n", sendMsgqID, mMsg.msg, sendMsgqID);
                        }


                        //unlocking FIFO and shm
                        sem.sem_op = 1;
                        semop(semID, &sem, 1);

                        //receive confirmation - I am invited SIGUSR1
                        //sigwait(&sigmask, &sig);
                        while (!shaved) { ;
                        }
                        noOfShaves--;
                        shaved = 0;
                        printf("%d\tI have to enter %d more times.\n", getpid(), noOfShaves);

                    } else {
                        //no free seats
                        clock_gettime(CLOCK_MONOTONIC, &tp);
                        printf("%d\t%d.%ld\tNo free seats.\n", getpid(), (int) tp.tv_sec, tp.tv_nsec);
                        //open FIFO nad shm
                        sem.sem_op = 1;
                        semop(semID, &sem, 1);
                    }
                } else {
                    //asleep
                    clock_gettime(CLOCK_MONOTONIC, &tp);
                    printf("%d\t%d.%ld\tWaking the barber up.\n", getpid(), (int) tp.tv_sec, tp.tv_nsec);

                    //setting up receiving info from barber
                    //int sig;
                    //sigset_t sigmask;
                    //sigemptyset(&sigmask);
                    //sigaddset(&sigmask, SIGUSR2);

                    //wake up
                    kill(shmAddr[3], SIGUSR2);

                    //wait for conformation
                    //sigwait(&sigmask, &sig);
                    recMsgSem.sem_op = -1;
                    semop(semID, &recMsgSem, 1);

                    //unlocking FIFO and shm
                    sem.sem_op = 1;
                    semop(semID, &sem, 1);

                    clock_gettime(CLOCK_MONOTONIC, &tp);
                    printf("%d\t%d.%ld\tSat in barber's seat.\n", getpid(), (int) tp.tv_sec, tp.tv_nsec);

                    //letting know that i'm in a chair
                    //kill(shmAddr[3], SIGUSR1);
                    sendMsgSem.sem_op = 1;
                    semop(semID, &sendMsgSem, 1);

                    //waiting for him to finish
                    //sigwait(&sigmask, &sig);
                    recMsgSem.sem_op = -1;
                    semop(semID, &recMsgSem, 1);

                    clock_gettime(CLOCK_MONOTONIC, &tp);
                    printf("%d\t%d.%ld\tI'm leaving.\n", getpid(), (int) tp.tv_sec, tp.tv_nsec);

                    //let the barber know i left
                    //kill(shmAddr[3], SIGUSR1);
                    sendMsgSem.sem_op = 1;
                    semop(semID, &sendMsgSem, 1);
                    noOfShaves--;

                    printf("%d\tI have to enter %d more times\n", getpid(), noOfShaves);

                }


                //noOfShaves--;
            }


            free(buffer);
            //unlocking shm
            shmdt(shmAddr);
            printf("%d\tI'm finished.\n", getpid());
            return 0;
        }


    }

    while (noOfClients) {
        wait(NULL);
        noOfClients--;
    }
    free(buffer);
    //unlocking shm
    shmdt(shmAddr);
    printf("%d\tI spawned all - I'm finished.\n", getpid());
    return 0;
}
