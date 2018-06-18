#define _XOPEN_SOURCE 500
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
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
#include <sys/time.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <netinet/in.h>
#include "ipcutil.h"

char clients[MAX_NO_OF_CLIENTS][MAX_NAME_SIZE];

sem_t clients_list_lock;

struct sockaddr_in socket_net;


int stringToInt(char* s){
    int result = 0;
    for(int i=0; i<strlen(s); i++){
        result*=10;
        result+=(s[i]-'0');
    }
    return result;
}



void close_sems(void){
    sem_destroy(&clients_list_lock);
}

void* IO_start_routine(void* p){

    if(pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)!=0)printf("err\n");
    if(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL)!=0){printf("err2\n");};

    while(1) {

        sem_wait(&write_lock);

        sem_wait(&buffer_lock[writing_pos]);

        while(global_buffer[writing_pos]!=NULL){
            //if(info_mode)
            //printf("Buffer is full");
            sem_post(&buffer_lock[writing_pos]);
            sem_wait(&someone_is_reading);
            sem_wait(&buffer_lock[writing_pos]);
        }

        char *line_buf = malloc(MAX_LINE_LENGTH * sizeof(*line_buf));

        size_t read_len = MAX_LINE_LENGTH;
        ssize_t rc = getline(&line_buf, &read_len, file_to_read);
        if (rc != -1) {
            if(info_mode)
                printf("I am %ld and I read line %d into buf cell %d: %s", pthread_self(),line_nr, writing_pos, line_buf);

            global_buffer[writing_pos] = line_buf;

            line_nr++;

            //printf("Writing - up.\n");
            sem_post(&someone_is_writing);

            sem_post(&buffer_lock[writing_pos]);

            ++writing_pos;
            writing_pos %= N;

            sem_post(&write_lock);
        } else {

            sem_post(&someone_is_writing);

            sem_post(&buffer_lock[writing_pos]);

            sem_post(&write_lock);

            if(!NK)
                end_time = 1;

            return (&P);
        }
    }

}

void* com_start_routine(void* p) {

}

void* ping_start_routine(void* p){

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    if(info_mode)
        printf("I am consumer no: %ld.\n", pthread_self());


    while(1) {

        sem_wait(&read_lock);
        //printf("got read lock\n");
        sem_wait(&buffer_lock[reading_pos]);
        //printf("got global_buffer lock\n");
        while (global_buffer[reading_pos] == NULL) {
            //if(info_mode)
            //printf("Buffer is empty\n");
            if (end_time) {
                if(info_mode)
                    printf("Time to end, %ld.\n", pthread_self());
                sem_post(&buffer_lock[reading_pos]);
                sem_post(&read_lock);
                return &P;
            }
            sem_post(&buffer_lock[reading_pos]);
            sem_wait(&someone_is_writing);
            sem_wait(&buffer_lock[reading_pos]);
        }
        if(info_mode)
            printf("I am %ld and I read line from buf cell %d: %s", pthread_self(), reading_pos, global_buffer[reading_pos]);

        if(strlen(global_buffer[reading_pos])<L && search_mode == 0){
            printf("Found line: %s", global_buffer[reading_pos]);
            line_counter++;
        }
        if(strlen(global_buffer[reading_pos])==L && search_mode == 1){
            printf("Found line: %s", global_buffer[reading_pos]);
            line_counter++;
        }
        if(strlen(global_buffer[reading_pos])>L && search_mode == 2){
            printf("Found line: %s", global_buffer[reading_pos]);
            line_counter++;
        }

        global_buffer[reading_pos] = NULL;

        sem_post(&someone_is_reading);

        sem_post(&buffer_lock[reading_pos]);

        ++reading_pos;
        reading_pos %= N;

        sem_post(&read_lock);
    }
}



int main(int argc, char* argv[]){

    if (argc!=3){
        printf("Wrong number of arguments!\n");
        return 1;
    }

    //first arg - port no
    //set for net socket
    struct in_addr socket_net_addr;
    //big endian???
    socket_net_addr.s_addr = (uint32_t) stringToInt(argv[1]);
    socket_net.sin_addr

    //buffer for producers data
    global_buffer = (char**) malloc(N*sizeof(*global_buffer));

    //open file with data
    file_to_read = fopen(file_name, "r");
    if(!file_to_read){
        printf("Cant open %s!", file_name);
        return 1;
    }


    //init semaphores
    sem_init(&read_lock,0,1);
    sem_init(&write_lock,0,1);

    buffer_lock = (sem_t*) malloc(N*sizeof(*buffer_lock));
    for(int i=0; i<N; i++){
        char buf[10];
        sprintf(buf, "/%d_sem", i);
        sem_init(&buffer_lock[i],0,1);

    }

    //init condition variables
    sem_init(&someone_is_reading,0,0);
    sem_init(&someone_is_reading,0,0);

    end_time = 0;
    line_counter = 0;

    //nothing is written
    writing_pos = 0;
    reading_pos = 0;
    for(int i=0; i<N; i++){
        global_buffer[i] = NULL;
    }

    //start producers
    pthread_t* P_ar = malloc(sizeof(*P_ar)*P);
    for(int i=0; i<P; i++){

        pthread_create(&P_ar[i], NULL, P_start_routine, NULL);
    }

    //start consumers
    pthread_t* C_ar = malloc(sizeof(*C_ar)*K);
    for(int i=0; i<K; i++){

        pthread_create(&C_ar[i], NULL, C_start_routine, NULL);
    }

    if(NK){
        sleep(NK);
        for(int i=0; i<P; i++){
            pthread_cancel(P_ar[i]);
        }
        for(int i=0; i<K; i++){
            pthread_cancel(C_ar[i]);
        }
        //wait for producers to finish
        for(int i=0; i<P; i++){
            pthread_join(P_ar[i], NULL);
        }

        //wait for consumers to finish
        for(int i=0; i<K; i++){
            pthread_join(C_ar[i], NULL);
        }
        close_sems();
        printf("%d seconds have passed, time to end\n", NK);
        return 0;
    }

    //wait for producers to finish
    for(int i=0; i<P; i++){
        pthread_join(P_ar[i], NULL);
    }

    //wait for consumers to finish
    for(int i=0; i<K; i++){
        pthread_join(C_ar[i], NULL);
    }



    free(global_buffer);
    close_sems();

    //if(info_mode)
    printf("Done. Found %d lines.\n", line_counter);
    return 0;
}