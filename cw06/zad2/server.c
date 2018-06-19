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
#include <mqueue.h>
#include "ipcutil.h"
#include <errno.h>

int stringToInt(char *s) {
    int result = 0;
    for (int i = 0; i < strlen(s); i++) {
        result *= 10;
        result += (s[i] - '0');
    }
    return result;
}


void catchSIGINT(int signo) {
    printf("\nOdebrano sygnal SIGINT\n");
    SERVER_STOP = 1;
}


int main(int argc, char *argv[]) {

    SERVER_STOP = 0;

    struct sigaction act;
    act.sa_handler = catchSIGINT;
    sigaction(SIGINT, &act, NULL);

//creating public queue
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct myMsgBuf);
    char qPath[10];
    strcpy(qPath, QUEUE_PATH);
    mqd_t msqID = mq_open(qPath, O_RDONLY | O_CREAT | O_NONBLOCK, S_IRUSR | S_IWUSR, &attr);
//printf("rcqid: %d\n", msqID);
//mqd_t msqID = mq_open(QUEUE_PATH, O_RDONLY | O_CREAT , S_IRUSR |S_IWUSR, &attr);
//printf("%d", msqID);
//printf("%s", strerror(errno));

    int i = 0;
    struct myMsgBuf recMsg;
    int k = 0;
    int SERVER_END = 0;
//pid_t clientPIDs[MAX_NO_OF_CLIENTS];
    struct clientInfo clientMsgQs[MAX_NO_OF_CLIENTS];

    struct clientInfo noClient;
    noClient.pid = 0;
    for (i = 0; i < MAX_NO_OF_CLIENTS; i++) {
        clientMsgQs[i] = noClient;
    }

    int client_no = 0;

    char *buffer = malloc(MAX_MSG_SIZE);
    while (!SERVER_END) {
        if ((k = mq_receive(msqID, (char *) &recMsg, sizeof(struct myMsgBuf), NULL)) > 0) {

            //get client info

            for (i = 0; i < MAX_NO_OF_CLIENTS; i++) {
                if (clientMsgQs[i].pid == recMsg.pid) {
                    break;
                }
            }
            switch (recMsg.mType) {

                case START: {
                    printf("Child %d starts. QID: %s\n", recMsg.pid, recMsg.msg);
                    int flag = 0;
                    while (clientMsgQs[client_no].pid != 0) {
                        if (flag < 2) {

                            client_no++;
                            if (client_no == MAX_NO_OF_CLIENTS) {
                                client_no = 0;
                                flag++;
                            }
                        } else {
                            break;
                        }
                    }
                    if (flag == 2) {
                        printf("No room for client, try again later.\n");
                        break;
                    }
                    clientMsgQs[client_no].pid = recMsg.pid;
                    char qPath[100];
                    strcpy(qPath, "/");
                    //strcat(qPath+1, recMsg.msg);
                    sprintf(qPath + 1, "%d", recMsg.pid);
                    //printf("opening: %s", qPath);
                    mqd_t qID = mq_open(qPath, O_WRONLY);
                    //printf("opened Q: %d, client: %d\n", qID, client_no);
                    if (qID < 0) { printf("%s", strerror(errno)); }

                    clientMsgQs[client_no].qID = qID;
                    //sending client ID to client
                    sprintf(recMsg.msg, "%d", client_no);
                    mq_send(clientMsgQs[client_no].qID, (char *) &recMsg, sizeof(recMsg), 1);

                    break;
                }


                case MIRROR: {
                    printf("Mirror, %d\n", k);
                    int j;
                    printf("Received: %s\n", recMsg.msg);
                    for (j = 0; j < strlen(recMsg.msg) / 2; j++) {
                        char tmp = recMsg.msg[j];
                        recMsg.msg[j] = recMsg.msg[strlen(recMsg.msg) - j - 1];
                        recMsg.msg[strlen(recMsg.msg) - j - 1] = tmp;
                    }
                    printf("Sending: %s, size:%ld to %d\n", recMsg.msg, strlen(recMsg.msg), clientMsgQs[i].qID);
                    //msgsnd(clientMsgQs[i].qID, &recMsg, sizeof(getpid())+strlen(recMsg.msg)+1,0);
                    mq_send(clientMsgQs[i].qID, (char *) &recMsg, sizeof(recMsg), 1);

                    break;
                }
                case ADD: {
                    printf("add\n");
                    int a = stringToInt(strtok(recMsg.msg, " \n"));
                    int b = stringToInt(strtok(NULL, " \n"));
                    sprintf(recMsg.msg, "%d", a + b);
                    printf("Sending: %s, size:%ld to %d\n", recMsg.msg, strlen(recMsg.msg), clientMsgQs[i].qID);
                    //msgsnd(clientMsgQs[i].qID, &recMsg, sizeof(getpid())+strlen(recMsg.msg)+1,0);
                    mq_send(clientMsgQs[i].qID, (char *) &recMsg, sizeof(recMsg), 0);

                    break;
                }
                case MUL: {
                    printf("mul\n");
                    int a = stringToInt(strtok(recMsg.msg, " \n"));
                    int b = stringToInt(strtok(NULL, " \n"));
                    sprintf(recMsg.msg, "%d", a * b);
                    printf("Sending: %s, size:%ld\n", recMsg.msg, strlen(recMsg.msg));
                    //msgsnd(clientMsgQs[i].qID, &recMsg, sizeof(getpid())+strlen(recMsg.msg)+1,0);
                    mq_send(clientMsgQs[i].qID, (char *) &recMsg, sizeof(recMsg), 0);

                    break;
                }
                case SUB: {
                    printf("sub\n");
                    int a = stringToInt(strtok(recMsg.msg, " \n"));
                    int b = stringToInt(strtok(NULL, " \n"));
                    sprintf(recMsg.msg, "%d", a - b);
                    printf("Sending: %s, size:%ld\n", recMsg.msg, strlen(recMsg.msg));
                    //msgsnd(clientMsgQs[i].qID, &recMsg, sizeof(getpid())+strlen(recMsg.msg)+1,0);
                    mq_send(clientMsgQs[i].qID, (char *) &recMsg, sizeof(recMsg), 0);

                    break;
                }
                case DIV: {
                    printf("div\n");
                    float a = (float) stringToInt(strtok(recMsg.msg, " \n"));
                    float b = (float) stringToInt(strtok(NULL, " \n"));
                    if (b != 0) {
                        sprintf(recMsg.msg, "%f", a / b);
                    } else {
                        strcpy(recMsg.msg, "Division by zero!");
                    }
                    printf("Sending: %s, size:%ld\n", recMsg.msg, strlen(recMsg.msg));
                    // msgsnd(clientMsgQs[i].qID, &recMsg, sizeof(getpid())+strlen(recMsg.msg)+1,0);
                    mq_send(clientMsgQs[i].qID, (char *) &recMsg, sizeof(recMsg), 0);

                    break;
                }
                case TIME: {
                    time_t current_time = time(NULL);
                    strcpy(recMsg.msg, ctime(&current_time));
                    //msgsnd(clientMsgQs[i].qID, &recMsg, sizeof(getpid())+strlen(recMsg.msg)+1,0);
                    mq_send(clientMsgQs[i].qID, (char *) &recMsg, sizeof(recMsg), 0);

                    break;
                }
                case END: {
                    printf("Received END.\n");
                    SERVER_END = 1;
                    break;
                }
                case STOP: {
                    printf("Deliting child %d.\n", i);
                    clientMsgQs[i] = noClient;
                    break;
                }
                default:
                    printf("Unrecognised type of message\n");
                    //send err?
                    printf("%ld\n", recMsg.mType);
            }
        }
        /*else{printf("cant receive\n");
            printf("%s", strerror(errno));
        }*/

        if (SERVER_STOP) break;

    }

    printf("Server stopped.\n");
//msgctl(msqID, IPC_RMID, NULL);

    mq_close(msqID);
    mq_unlink("/queue");

    return 0;
}
