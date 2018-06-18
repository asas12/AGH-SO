#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>


void catchSIGINT(int signo){
    printf("\nOdebrano sygnal SIGINT\n");
    exit(0);
}

void catch2ndSIGTSTP(int signo){
    printf("\n");
}

void catchSIGTSTP(int signo){
    printf("\nOczekuje na CTRL+Z - kontynuacja albo CTRL+C - zakonczenie programu\n");
    struct sigaction act;
    act.sa_handler = catchSIGINT;
    sigaction(SIGINT, &act, NULL);   
    signal(SIGTSTP, catch2ndSIGTSTP);
    pause();   //sigactionSIGINT
}


int main(void){

struct tm* currentTime;
time_t now;
 
signal(SIGTSTP, catchSIGTSTP);
struct sigaction act;
act.sa_handler = catchSIGINT;
sigaction(SIGINT, &act, NULL);

while(1){
    now = time(NULL);
    currentTime = localtime(&now);
    printf("%02d:%02d:%02d\n", currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec);
    
    signal(SIGTSTP, catchSIGTSTP);

    sleep(1);
}
return 0;    
}
