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

void catchSIGINT(int signo){
    printf("\nOdebrano sygnal SIGINT\n");
    CLIENT_STOP=1;
}

int main(int argc, char* argv[]){

CLIENT_STOP=0;

struct sigaction act;
act.sa_handler = catchSIGINT;
sigaction(SIGINT, &act, NULL);

mqd_t sendMsgqID = mq_open("/queue", O_WRONLY);;
//printf("psgq: %d\n", sendMsgqID);
struct mq_attr attr;
attr.mq_maxmsg = 10;
attr.mq_msgsize = sizeof(struct myMsgBuf);
char qPath[10];
strcpy(qPath, "/");
sprintf(qPath+1, "%d", getpid());
//printf("path: %s\n", qPath);
mqd_t recMsgqID = mq_open(qPath, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR, &attr);
//printf("rcqid: %d\n", recMsgqID);
//printf("%s\n", strerror(errno));

//const char* homePath;
//if((homePath = getenv("HOME")) == NULL){
//  printf("Can't get gome variable.\n");
//}

//key_t parentKey = ftok(homePath, PROJECT_NO);

//int sendMsgqID = msgget(parentKey, 0);

//int recMsgqID = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);


char* buffer;
size_t bufSize = MAX_MSG_SIZE;
buffer = (char*) malloc(bufSize*sizeof(char));

//sending private queue ID
struct myMsgBuf myMsg;
myMsg.mType = START;
myMsg.pid = getpid();
sprintf(buffer, "%d", recMsgqID);
strcpy(myMsg.msg, buffer);
//printf("msg set:%s, pid: %d\n", myMsg.msg, myMsg.pid);
//mq_send(sendMsgqID,(char*) &myMsg, sizeof(getpid())+strlen(myMsg.msg)+1 , 0);
mq_send(sendMsgqID,(char*) &myMsg, sizeof(myMsg) , 1);

//printf("msg sent. %s\n", myMsg.msg);
mq_receive(recMsgqID,(char*)  &myMsg, sizeof(struct myMsgBuf), NULL);
printf("My ID from parent: %s\n", myMsg.msg);
int send=0;
int i=0;
while(!CLIENT_STOP){
    if(!feof(stdin) && getline(&buffer, &bufSize, stdin)>0){
        struct myMsgBuf mMsg;
        mMsg.pid = getpid();

        if(CLIENT_STOP){
            mMsg.mType = STOP;
            strcpy(mMsg.msg, " ");
            send=2;
        }
        //printf("%s", buffer);
        char* fWord;
        fWord = strtok(buffer, " \n");
        //printf("%s\n", fWord);
        if(!strcmp(fWord, "MIRROR")){ 
            mMsg.mType = MIRROR;
            //strcpy(mMsg.msg, strtok(NULL, "\n\0"));
            strcpy(mMsg.msg, buffer+7);
            send = 1;
        }else if(!strcmp(fWord, "ADD")){
            mMsg.mType = ADD;
            strcpy(mMsg.msg, buffer+4);
            send=1;             
        }else if(!strcmp(fWord, "MUL")){
            mMsg.mType = MUL;
            strcpy(mMsg.msg, buffer+4);
            send=1;
        }else if(!strcmp(fWord, "SUB")){
            mMsg.mType = SUB;
            strcpy(mMsg.msg, buffer+4);
            send=1;
        }else if(!strcmp(fWord, "DIV")){
            mMsg.mType = DIV;
            strcpy(mMsg.msg, buffer+4);
            send=1;
        }else if(!strcmp(fWord, "TIME")){
            mMsg.mType = TIME;
            strcpy(mMsg.msg, " ");
            send=1;
        }else if(!strcmp(fWord, "END")){
            mMsg.mType = END;
            strcpy(mMsg.msg, " ");
            CLIENT_STOP=1;
            send=2;
        }else{
            printf("Wrong operation\n");
            send=0;
        }
        
        if(send>0){
            mMsg.msg[strlen(mMsg.msg)-1]='\0';
            //if((i = msgsnd(sendMsgqID, &mMsg, sizeof(getpid())+strlen(mMsg.msg)+1, 0))<0){
            if((i=mq_send(sendMsgqID,(char*) &mMsg, sizeof(struct myMsgBuf),1))<0){
                printf("Cant send!\n");
            }else{
                printf("Sent: %d,  msg: %s\n", sendMsgqID,  mMsg.msg);
            }if(!CLIENT_STOP){
                //msgrcv(recMsgqID, &mMsg, MAX_MSG_SIZE, 0,0);
                mq_receive(recMsgqID, (char*) &mMsg, sizeof(struct myMsgBuf),NULL);
                printf("Received: %s\n", mMsg.msg);
            }
            send--;
        }

       
    }
}

if(!send){
    struct myMsgBuf mMsg;
    mMsg.pid = getpid();
    mMsg.mType = STOP;
    strcpy(mMsg.msg, " ");
    mMsg.msg[strlen(mMsg.msg)-1]='\0';
    //if((i = msgsnd(sendMsgqID, &mMsg, sizeof(getpid())+strlen(mMsg.msg)+1, 0))<0){
     if((i=mq_send(sendMsgqID,(char*) &mMsg, sizeof(struct myMsgBuf),1))<0){
        printf("Cant send!\n");
    }else{
        printf("Sent: %d, size of msg: %d, msg: %s\n", sendMsgqID, i, mMsg.msg);
    }if(!CLIENT_STOP){
        //msgrcv(recMsgqID, &mMsg, MAX_MSG_SIZE, 0,0);
         mq_receive(recMsgqID, (char*) &mMsg, sizeof(struct myMsgBuf),NULL);
    
        printf("Received: %s\n", mMsg.msg);
    }
}

free(buffer);
mq_close(sendMsgqID);
mq_close(recMsgqID);
mq_unlink(qPath);
printf("closed\n");   
return 0;
}
