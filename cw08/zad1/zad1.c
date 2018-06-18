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

int stringToInt(char* s){
    int result = 0;
    for(int i=0; i<strlen(s); i++){
        result*=10;
        result+=(s[i]-'0');
    }
    return result;
}

void close_files(FILE* a, FILE* b, FILE* c){
    fclose(a);
    fclose(b);
    fclose(c);
}


int** img_in_array;
int** img_out_array;
double** filter_array;
int height;
int width;
int c;
int* start_end_ar;

void* start_routine(void* p){
    int* ar = (int*) p;
    int start = ar[0];
    int end = ar[1];
    for(int x=start; x<end; x++){
        for(int y=0; y<width; y++){
            double s = 0;
            for(int i=0; i<c; i++){
                for(int j=0; j<c; j++){
                    int index_i;
                    index_i = (x-ceil(c/2.0)+i)>0? ((x-(int)ceil(c/2.0)+i)<height-1? (x-(int)ceil(c/2.0)+i) : height-1) : 0;
                    int index_j;
                    index_j = (y-ceil(c/2.0)+j)>0? ((y-(int)ceil(c/2.0)+j)<width-1? (y-(int)ceil(c/2.0)+j) : width-1) : 0;
                    s+=img_in_array[index_i][index_j]*filter_array[i][j];
                }
            }
            img_out_array[x][y] = (int) round(s);
        }
    }
    free(p);
    return ar;
}


int main(int argc, char* argv[]){

    if (argc!=5){
        printf("Wrong number of arguments!\n");
        return 1;
    }

    int thread_no = stringToInt(argv[1]);
    FILE* img_in = fopen(argv[2], "r");
    FILE* filter = fopen(argv[3], "r");
    FILE* img_out = fopen(argv[4], "w");
    if(!img_in || !filter || !img_out){
        printf("Error opening files\n");
        return 1;
    }

    //read img_in to array
    int how_many_chars = 0;
    char buffer[70];
    char* endptr = NULL;
    //check for P2
    fscanf(img_in, "%s", buffer);
    if(strcmp("P2", buffer)!=0) {
        printf("Wrong in file format");
    }
    fwrite(buffer, sizeof(*buffer), strlen(buffer), img_out);
    fwrite("\n", sizeof(*buffer), 1, img_out);
    //check for width
    fscanf(img_in, "%s", buffer);
    width = stringToInt(buffer);
    fwrite(buffer, sizeof(*buffer), strlen(buffer), img_out);
    fwrite(" ", sizeof(*buffer), 1, img_out);
    //check for height
    fscanf(img_in, "%s", buffer);
    height = stringToInt(buffer);
    fwrite(buffer, sizeof(*buffer), strlen(buffer), img_out);
    fwrite("\n", sizeof(*buffer), 1, img_out);
    //check for max grey value
    fscanf(img_in, "%s", buffer);
    int max_grey_value = stringToInt(buffer);
    fwrite(buffer, sizeof(*buffer), strlen(buffer), img_out);
    fwrite("\n", sizeof(*buffer), 1, img_out);

    img_in_array = malloc(height*sizeof(*img_in_array));
    img_out_array = malloc(height*sizeof(*img_in_array));
    for(int i=0; i< height; i++){
        img_in_array[i] = (int*) malloc(width*sizeof(*img_in_array[i]));
        img_out_array[i] = (int*) malloc(width*sizeof(*img_in_array[i]));
    }

    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            fscanf(img_in, "%s", buffer);
            int n = (int) strtol(buffer, &endptr, 10);
            img_in_array[i][j]=n;
        }
    }


    //read filter size
    fscanf(filter, "%s", buffer);
    c = stringToInt(buffer);

    //allocate filter
    filter_array = malloc(c*sizeof(*filter_array));
    for(int i=0; i<c; i++){
        filter_array[i] = (double*) malloc(c*sizeof(*filter_array));
    }

    //read filter to array
    for(int i=0; i<c; i++){
        for(int j=0; j<c; j++){
            fscanf(filter, "%s", buffer);
            double n = strtod(buffer, &endptr);
            filter_array[i][j]=n;
            //printf("%f\n", filter_array[i][j]);
        }
    }
    printf("No of threads: %d. Filter size: %dx%d.\n", thread_no, c, c);

    struct timeval start_time;
    gettimeofday(&start_time, NULL);


    //create threads
    pthread_t* thread_ar = malloc(sizeof(*thread_ar)*thread_no);
    for(int i=0; i<thread_no*2; i+=2){
        //freed in thread routine
        int * ar = malloc(2*sizeof(*ar));
        ar[0] = (height/thread_no)*i/2;
        ar[1] = (height/thread_no)*(i/2+1);
        if(i==thread_no*2-2){
            ar[1] = height;
        }
        pthread_create(&thread_ar[i], NULL, start_routine, (void*)ar);
    }
    for(int i=0; i<thread_no; i++){
         pthread_join(thread_ar[i], NULL);
    }

    struct timeval end_time;
    gettimeofday(&end_time, NULL);

    timersub(&end_time, &start_time, &end_time);
    printf("Took %ld.%06ld seconds.", end_time.tv_sec, end_time.tv_usec);

    //filtering itself
    /*for(int x=0; x<height; x++){
        for(int y=0; y<width; y++){
            double s = 0;
            for(int i=0; i<c; i++){
                for(int j=0; j<c; j++){
                    int index_i;
                    index_i = (x-ceil(c/2.0)+i)>0? ((x-(int)ceil(c/2.0)+i)<height-1? (x-(int)ceil(c/2.0)+i) : height-1) : 0;
                    //printf("ind i:%d", index_i);
                    int index_j;
                    index_j = (y-ceil(c/2.0)+j)>0? ((y-(int)ceil(c/2.0)+j)<width-1? (y-(int)ceil(c/2.0)+j) : width-1) : 0;
                    s+=img_in_array[index_i][index_j]*filter_array[i][j];
                }
            }
            img_out_array[x][y] = (int) round(s);
        }
    }*/

    //result into img_out file
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            sprintf(buffer, "%d\n", img_out_array[i][j]);
            fwrite(buffer, sizeof(*buffer), strlen(buffer), img_out);

        }
    }


    for(int i=0; i< height; i++){
        free(img_in_array[i]);
        free(img_out_array[i]);
    }
    free(img_in_array);
    free(img_out_array);


    close_files(img_in, img_out, filter);


    printf("Done.\n");
    return 0;
}
