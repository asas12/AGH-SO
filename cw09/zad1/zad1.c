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
#include "ipcutil.h"

#define MAX_LINE_LENGTH 256

int P, K, N, L, search_mode, info_mode, NK;
char file_name[20];
char **global_buffer;

FILE *file_to_read;

int end_time;

int line_nr;
int writing_pos, reading_pos;

pthread_mutex_t write_lock, read_lock;
pthread_mutex_t *buffer_lock;

pthread_cond_t someone_is_reading;
pthread_cond_t someone_is_writing;

int line_counter;

int stringToInt(char *s) {
    int result = 0;
    for (int i = 0; i < strlen(s); i++) {
        result *= 10;
        result += (s[i] - '0');
    }
    return result;
}

void setOptions(char *path) {
    FILE *options = fopen(path, "r");
    if (!options) {
        printf("Error opening file\n");
        return;
    }

    fscanf(options, "%d %d %d %s %d %d %d %d", &P, &K, &N, file_name, &L, &search_mode, &info_mode, &NK);
    fclose(options);
}

void *P_start_routine(void *p) {

    while (1) {

        pthread_mutex_lock(&write_lock);

        pthread_mutex_lock(&buffer_lock[writing_pos]);

        while (global_buffer[writing_pos] != NULL) {
            if (info_mode)
                printf("Buffer is full");
            pthread_cond_wait(&someone_is_reading, &buffer_lock[writing_pos]);
        }

        char *line_buf = malloc(MAX_LINE_LENGTH * sizeof(*line_buf));

        size_t read_len = MAX_LINE_LENGTH;
        ssize_t rc = getline(&line_buf, &read_len, file_to_read);
        if (rc != -1) {
            if (info_mode)
                printf("I am %ld and I read line %d into buf cell %d: %s", pthread_self(), line_nr, writing_pos,
                       line_buf);

            global_buffer[writing_pos] = line_buf;

            line_nr++;

            pthread_cond_broadcast(&someone_is_writing);

            pthread_mutex_unlock(&buffer_lock[writing_pos]);

            ++writing_pos;
            writing_pos %= N;

            pthread_mutex_unlock(&write_lock);
        } else {

            pthread_cond_broadcast(&someone_is_writing);

            pthread_mutex_unlock(&buffer_lock[writing_pos]);

            pthread_mutex_unlock(&write_lock);

            if (!NK)
                end_time = 1;

            return (&P);
        }
    }

}

void *C_start_routine(void *p) {
    if (info_mode)
        printf("I am consumer no: %ld.\n", pthread_self());


    while (1) {

        pthread_mutex_lock(&read_lock);
        //printf("got read lock\n");
        pthread_mutex_lock(&buffer_lock[reading_pos]);
        //printf("got global_buffer lock\n");
        while (global_buffer[reading_pos] == NULL) {
            if (info_mode)
                printf("Buffer is empty\n");
            if (end_time) {
                if (info_mode)
                    printf("Time to end, %ld.\n", pthread_self());
                pthread_mutex_unlock(&buffer_lock[reading_pos]);
                pthread_mutex_unlock(&read_lock);
                return &P;
            }
            pthread_cond_wait(&someone_is_writing, &buffer_lock[reading_pos]);
        }
        if (info_mode)
            printf("I am %ld and I read line from buf cell %d: %s", pthread_self(), reading_pos,
                   global_buffer[reading_pos]);

        if (strlen(global_buffer[reading_pos]) < L && search_mode == 0) {
            printf("Found line: %s", global_buffer[reading_pos]);
            line_counter++;
        }
        if (strlen(global_buffer[reading_pos]) == L && search_mode == 1) {
            printf("Found line: %s", global_buffer[reading_pos]);
            line_counter++;
        }
        if (strlen(global_buffer[reading_pos]) > L && search_mode == 2) {
            printf("Found line: %s", global_buffer[reading_pos]);
            line_counter++;
        }

        global_buffer[reading_pos] = NULL;

        pthread_cond_broadcast(&someone_is_reading);

        pthread_mutex_unlock(&buffer_lock[reading_pos]);

        ++reading_pos;
        reading_pos %= N;

        pthread_mutex_unlock(&read_lock);
    }
}


int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Wrong number of arguments!\n");
        return 1;
    }

    //process file with options
    setOptions(argv[1]);
    if (info_mode)
        printf("P: %d, K: %d, N: %d, file_name: %s L: %d, search_mode: %d, info_mode: %d, NK: %d\n", P, K, N, file_name,
               L, search_mode, info_mode, NK);

    //buffer for producers data
    global_buffer = (char **) malloc(N * sizeof(*global_buffer));

    //open file with data
    file_to_read = fopen(file_name, "r");
    if (!file_to_read) {
        printf("Cant open %s!", file_name);
        return 1;
    }


    //init mutexes
    int rc = 0;
    rc = pthread_mutex_init(&read_lock, NULL);
    if (rc != 0) {
        printf("error initialising mutex!");
        return 1;
    }
    rc = pthread_mutex_init(&write_lock, NULL);
    if (rc != 0) {
        printf("error initialising mutex!");
        return 1;
    }

    buffer_lock = (pthread_mutex_t *) malloc(N * sizeof(*buffer_lock));
    for (int i = 0; i < N; i++) {

        rc = pthread_mutex_init(&buffer_lock[i], NULL);
        if (rc != 0) {
            printf("error initialising mutex!");
            return 1;
        }

    }

    //init condition variables
    //someone_is_reading = PTHREAD_COND_INITIALIZER;
    //empty_buffer = PTHREAD_COND_INITIALIZER;
    pthread_cond_init(&someone_is_reading, NULL);
    pthread_cond_init(&someone_is_writing, NULL);

    end_time = 0;
    line_counter = 0;

    //nothing is written
    writing_pos = 0;
    reading_pos = 0;
    for (int i = 0; i < N; i++) {
        global_buffer[i] = NULL;
    }

    //start producers
    pthread_t *P_ar = malloc(sizeof(*P_ar) * P);
    for (int i = 0; i < P; i++) {

        pthread_create(&P_ar[i], NULL, P_start_routine, NULL);
    }

    //start consumers
    pthread_t *C_ar = malloc(sizeof(*C_ar) * K);
    for (int i = 0; i < K; i++) {

        pthread_create(&C_ar[i], NULL, C_start_routine, NULL);
    }

    if (NK) {
        sleep(NK);
        printf("%d seconds have passed, time to end\n", NK);
        return 0;
    }

    //wait for producers to finish
    for (int i = 0; i < P; i++) {
        pthread_join(P_ar[i], NULL);
    }

    //wait for consumers to finish
    for (int i = 0; i < K; i++) {
        pthread_join(C_ar[i], NULL);
    }


    free(global_buffer);



    //if(info_mode)
    printf("Done. Found %d lines.\n", line_counter);
    return 0;
}
