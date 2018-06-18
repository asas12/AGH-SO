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

int stringToInt(char* s){
    int result = 0;
    for(int i=0; i<strlen(s); i++){
        result*=10;
        result+=(s[i]-'0');
    }
    return result;
}



int main(int argc, char* argv[]){

    if (argc!=3){
        printf("Wrong number of arguments!\n");
        return 1;
    }

    int c = stringToInt(argv[1]);

    FILE* filter = fopen(argv[2], "w");

    char buffer[10];
    sprintf(buffer, "%d\n", c);
    fwrite(buffer, sizeof(*buffer), strlen(buffer), filter);

    c*=c;
    int* array = malloc(c*sizeof(*array));

    for(int i=0; i<c; i++){
        array[i]=i;
    }

    //shuffle
    srand(time(NULL));
    for(int i=0; i<c; i++){
        int random = rand()%(c);
        int tmp = array[random];
        array[random] = array[i];
        array[i] = tmp;
    }


    //fill array with doubles that sum to 1
    double* d_array = malloc(c*sizeof(*array));
    for(int i=0; i<c; i++){
        d_array[i] = array[i]/((c*(c-1)/2.0));
        sprintf(buffer, "%f\n", d_array[i]);
        fwrite(buffer, sizeof(*buffer), strlen(buffer), filter);
        printf("%f\n", d_array[i]);
    }

    fclose(filter);
    printf("I finished.\n");
    return 0;
}
