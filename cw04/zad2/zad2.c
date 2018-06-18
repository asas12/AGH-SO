#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>

int n,k,N,K;

sem_t* flag=NULL;
pid_t* childAr;
pid_t* childBuf;

int stringToInt(char* s){
    int result = 0;
    for(int i=0; i<strlen(s); i++){
        result*=10;
        result+=(s[i]-'0');
    }
    return result;
}

void catchSIGINT(int signo){
    exit(0);
}

void catchUSR1(int signo, siginfo_t* siginfo, void* context){

    pid_t cpid = siginfo->si_pid; 
    if(k<K){childBuf[k]=cpid;}
    k++;
    printf("Caught %d work request from child %d\n", k, cpid);
    if(k>K){
        printf("Allowing process %d\n", cpid);
        kill(siginfo->si_pid, SIGUSR2);
    }
    else{
        if(k==K){
            for(int l=0; l<k; l++){
                printf("Allowing process %d in bulk\n", childBuf[l]);
                kill(childBuf[l], SIGUSR2);
            }           
        }
    }
    sem_post(flag);
}

void catchUSR2(int signo, siginfo_t* siginfo, void* context){
    sem_wait(flag);
    union sigval value;
    srand(getpid());
    int newSig = rand()%(SIGRTMAX-SIGRTMIN);
    newSig+= SIGRTMIN;
    sigqueue(siginfo->si_pid, newSig, value);
    sem_post(flag);
}


void catchSIGRT(int signo, siginfo_t* siginfo, void* context){
    n++;
    printf("Caught %d real time signal nr %d from child %d\n", n, signo, siginfo->si_pid);
}

void catchSIGCHLD(int signo, siginfo_t* siginfo, void* context){
    int status;
    int cpid = wait(&status);
    if(WIFEXITED(status) && cpid==siginfo->si_pid){
    printf("Process %d exited with status %d.\n", siginfo->si_pid, WEXITSTATUS(status));}
    else{printf("Process exited\n");}
    sem_post(flag);
}

int main(int argc, char* argv[]){
if(argc!=3){printf("Wrong argc.\n"); return 1;}

N = stringToInt(argv[1]);
K = stringToInt(argv[2]);
int i = 0;
int childPID=-1;
n=k=0;

childAr = malloc(sizeof(*childAr*N));
childBuf = (pid_t*) calloc(N, sizeof(*childBuf));

//create flags for proccesing USR1
shm_unlink("mem");  //clean from previous tests

int fd = shm_open("mem",O_CREAT | O_RDWR, 0777);
if(fd!=-1){
    if(ftruncate(fd, sizeof(*flag))==0){
        flag = mmap(NULL, sizeof(*flag), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        sem_init(flag, 1, 1);        
    }else{
        printf("err2");
        return 1;
    }
}else{printf("err1");return 1;}

//set responses to signals
//parent's reaction to child's requests
struct sigaction actUSR1;
actUSR1.sa_flags = SA_SIGINFO;
actUSR1.sa_sigaction = catchUSR1;
sigaction(SIGUSR1, &actUSR1, NULL);

//reaction to INT
struct sigaction actINT;
actINT.sa_handler = catchSIGINT;
sigaction(SIGINT, &actINT, NULL);

//child's reaction to parent allowing to work
struct sigaction actUSR2;
actUSR2.sa_flags = SA_SIGINFO;
actUSR2.sa_sigaction = catchUSR2;
sigaction(SIGUSR2, &actUSR2, NULL);

//parent's reaction to child's work
struct sigaction actSIGRT;
actSIGRT.sa_flags = SA_SIGINFO;
actSIGRT.sa_sigaction = catchSIGRT;
for(i=SIGRTMIN; i<=SIGRTMAX; i++){
    sigaction(i, &actSIGRT, NULL);
}

//parent's reaction to child's termination
struct sigaction actSIGCHLD;
actSIGCHLD.sa_flags = SA_SIGINFO;
actSIGCHLD.sa_sigaction = catchSIGCHLD;
sigaction(SIGCHLD, &actSIGCHLD, NULL);

sem_wait(flag);
for(i=0; i<N; i++){     //create n child processes
    childPID = fork();
    if(childPID==0){
        srand(getpid());
        int sleepTime = rand()%11;
        sleep(sleepTime);
        sem_wait(flag);
        int sv;
        sem_getvalue(flag, &sv);
        kill(getppid(), SIGUSR1);
        //sem_post(flag); moved to handler in parent
        pause(); //wait for work permission, work in handler
        sem_wait(flag);
//        printf("exiting: %d\n", getpid());
        exit(sleepTime);
    }
    else{
        printf("Created %d child: %d\n", i+1, childPID);
        childAr[i]=childPID;
    }
}
sem_post(flag);

while(n!=N){
    pause();
}

sem_destroy(flag);
kill(0, 2);
return 0;
}
