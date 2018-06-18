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
    int line_count =0;
    while(getline(&line, &len, file) != -1){
        line_count++;
        printf("%d line:\n", line_count);
        char *tmp;
        tmp = strtok(line, " \n");
        char * arguments[10][10];
        int i,j;
        for(i=0; i<10; i++){ 
            for(j=0; j<10; j++){
                arguments[i][j]=NULL;
            }
        }
        i=0;
        j=0;
        while(tmp!=NULL){
            j=0;
            while(tmp!= NULL && strcmp(tmp,"|")!=0){
                arguments[i][j] = tmp;
                tmp = strtok(NULL, " \n");
                j++;
            }
            i++;
            if(tmp){
                tmp = strtok(NULL, " \n");
            }else{ break;}
            
        }
        if(i==0){printf("Line %d is empty.\n", line_count); continue;}
        //only one program to run - no pipes needed
        if(i==1){
            pid_t childPID = vfork();
            if(childPID==0){
                if(execvp(arguments[0][0], arguments[0])!=0){
                    exit(EXIT_FAILURE); 
                }
            }else{
                int status=0;
                waitpid(childPID, &status, 0);
                //printf("\n%s status: %d\n",arguments[0][0], status);
                if(status!=0){
                    printf("Program %s caused interpreter to stop.\n", arguments[0][0]);
                    return 1;
                }
                continue;
            }
        }
        int pipes[2][2];
        pid_t childPID;
        for(j=0; j<i; j++){
            if(j>0){
                close(pipes[j%2][0]);
                close(pipes[j%2][1]);
            }
            
            if(pipe(pipes[j%2])==-1){
                printf("Can't create pipe\n");
                fflush(stdout);
                exit(EXIT_FAILURE);
            }//else{printf("IN: %d, OUT: %d\n", pipes[j%2][0],pipes[j%2][1]);} 
            
            childPID = fork();
            if(childPID==0){
                //printf("ME: j:%d,i: %d, LC: %d,  %s, IN: %d, OUT: %d\n",j,i,line_count, arguments[j][0], pipes[(j+1)%2][0], pipes[j%2][1]);
                //fflush(stdout);
                if(j!=0){
                    close(pipes[(j+1)%2][1]);
                    if(dup2(pipes[(j+1)%2][0], STDIN_FILENO)<0){
                        exit(EXIT_FAILURE);
                    }
                }
                if(j!=i-1){
                    close(pipes[j%2][0]);
                    if(dup2(pipes[j%2][1], STDOUT_FILENO)<0){
                        exit(EXIT_FAILURE);
                    }
                }
                //printf("arg: %s, %s\n",arguments[j][0], arguments[j][1]); 
                //fflush(stdout);
                if(execvp(arguments[j][0], arguments[j])!=0){
                    exit(EXIT_FAILURE); 
                }
            }

        }
        close(pipes[j%2][0]);
        close(pipes[j%2][1]);
        close(pipes[(j+1)%2][0]);
        close(pipes[(j+1)%2][1]);
        
        int status;
        waitpid(childPID, &status, 0);
        //printf("linia: %d, status: %d, PID: %d\n",line_count, status, childPID); 
        if(status!=0){printf("Error in line %d\n", line_count); return 1;}

    }
    fclose(file);
}   
    return 0;
}
