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

int main(int argc, char* argv[]){

if(argc==2){
    FILE* file = fopen(argv[1], "r");
    if(file == NULL){ printf("Can't open file!"); return 0;}
    char* line = NULL;
    size_t len = 0;
    while(getline(&line, &len, file) != -1){
        //printf("%s", line);
        char *tmp;
        tmp = strtok(line, " \n");
        char * arguments[10];
        int i;
        for(i=0; i<10; i++){ arguments[i]=NULL;}
        i=0;
        while(tmp!=NULL){
            arguments[i] = tmp;
            tmp = strtok(NULL, " \n");
            //printf("%s %ld", arguments[i], strlen(arguments[i]));
            i++;
        }
        pid_t childPID = vfork();
        if(childPID==0){
            if(execvp(arguments[0], arguments)!=0){
                exit(EXIT_FAILURE); 
            }
        }else{
            int status=0;
            waitpid(childPID, &status, 0);
            //printf("\n%s status: %d",arguments[0], status);
            if(status!=0){
                printf("Program %s caused interpreter to stop.\n", arguments[0]);
                return 1;
            }
        }
        //printf("\n");
        free(line); 
    }
    
    
}
    return 0;
}
