#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char StaticArray[NoBlocks][SoBlocks];

void createArrayStatic(void){
    for(int i=0;i<NoBlocks; i++){
        for(int j=0; j<SoBlocks-1; j++){
            StaticArray[i][j]='A';
        }
        StaticArray[i][SoBlocks-1]='\0';
    }
}

//al - array length, bl - block length, including '\0' sign, so watch out! ("hello" fits in bl=6 not bl=5)
char** createArrayDynamic(int al, int bl){
    char** ar = malloc(al*sizeof(char*));
    for(int i = 0; i<al; i++){
        ar[i] = malloc(bl*sizeof(char));
    }
//    printf("Allocated %d blocks of %d length\n", al, bl);
    return ar;
}

void deleteArrayDynamic(char** ar, int al){
    for(int i = 0; i<al; i++){
        free(ar[i]);
    }
    free(ar);
//    printf("Deleting...\n");
}

void deleteArrayStatic(){
     memset(StaticArray, '\0', sizeof(StaticArray[0][0]) * NoBlocks * SoBlocks);    
}

void insertBlockStatic(int n, char* s){
    if(n<0 || n>=NoBlocks){
        printf("No such index!\n");
        return;}
    if(strlen(s)>SoBlocks){
        printf("Too long string!\n");
        return;}
    strcpy(StaticArray[n], s);
    int i=0;
    for(; i<strlen(s); i++){
        StaticArray[n][i]=s[i];
    }
    for(; i<SoBlocks; i++){
        StaticArray[n][i]='\0';
    }
    //printf("Inserted \"%s\" at index %d\n", s, n);    
}

char** addBlockDynamic(int n, int al, int bl, char* s, char** ar){
    if(n<0 || n>=al){
        printf("No such index!\n");
        return ar;}
    //check if s is not too long (with \0 sign included)
    if(strlen(s)>bl-1){
        printf("You are trying to add a string that is to long!\n");
        return ar;}
    ar = (char**) realloc(ar, (al+1)*sizeof(char*));
//    printf("New dynamic array length is %d\n", al+1);
    //char* tmp = ar[al];
    char* fullS = (char*) malloc(bl*sizeof(char));
    strcpy(fullS, s);
    for(int i=strlen(s); i<bl; i++){
        fullS[i]='\0';
    }
    ar[al]=fullS;
    int pos=al;
    while(pos!=n){
        char* tmp = ar[pos-1];
        ar[pos-1] = ar[pos];
        ar[pos] = tmp;
        --pos;
    }
//    printf("Adding memory block at %d index...\n", n);
    return ar;
}

void insertBlockDynamic(int n, int al, int bl, char* s, char** ar){
    if(n<0 || n>=al){
        printf("No such index!\n");
    }
    //check if s is not too long (with \0 sign included)
    if(strlen(s)>bl-1){
        printf("You are trying to add a string that is to long!\n");
    }
     
    //s = (char*) realloc(s, bl*sizeof(char));
    for(int i=strlen(s); i<bl; i++){
        ar[n][i]='\0';
    }
    strcpy(ar[n], s);   
    //printf("Inserted \"%s\" at index %d\n", s, n);  
}

void deleteBlockStatic(int n){
    if(n<0 || n>=NoBlocks){
        printf("No such index!\n");
        return;}
    memset(StaticArray[n], '\0', sizeof(StaticArray[0][0]) * SoBlocks);
//    printf("Deleting  memory block at %d index...\n", n);
}

char** deleteBlockDynamic(int n, int al, char** ar){
    if(n<0 || n>=al){
        printf("No such index!\n");
        return ar;}
    
    int pos=n;
    while(pos!=al-1){
        char* tmp = ar[pos+1];
        ar[pos+1] = ar[pos];
        ar[pos] = tmp;
        ++pos;
    }
    free(ar[al-1]);
    ar = (char**) realloc(ar, (al-1)*sizeof(char*));
//    printf("New dynamic array length is %d\n", al-1);
//    printf("Deleting  memory block at %d index...\n", n);
    return ar;
}

int nearestDynamicSum(int n, int al, char** ar){
    if(n<0 || n>=al){
        printf("No such index!\n");
        return -1;
    }
    //int len = strlen(ar[n]);
    int sum = 0;
    for(int i=0; i<strlen(ar[n]); i++){
        sum+=ar[n][i];
    }
    int pos=0;
    int sumdiff=sum;
    for(int i=0; i<al; i++){
        int tmpsum=0;
        for(int j=0; j<strlen(ar[i]); j++){
            tmpsum+=ar[i][j];
        }
//        printf("Sum of %d word is %d\n", i, tmpsum);
        if(abs(tmpsum-sum)<sumdiff && i!=n){
            pos=i;
            sumdiff=abs(tmpsum-sum);
        }
    }
//    printf("Nearest block at %d index.\n", pos);
    return pos;
}

int nearestStaticSum(int n){
    if(n<0 || n>=NoBlocks){
        printf("No such index!");
        return -1;}
    int sum = 0;
    for(int i=0; i<strlen(StaticArray[n]); i++){
        sum+=StaticArray[n][i];
    }
//    printf("Sum of block at %d place is %d\n", n, sum);
    int pos=0;
    int sumdiff=sum;
    for(int i=0; i<NoBlocks; i++){
        int tmpsum=0;
        for(int j=0; j<SoBlocks; j++){
            tmpsum+=StaticArray[i][j];
        }
        //printf("Sum of %d word is %d\n", i, tmpsum);
        if(abs(tmpsum-sum)<sumdiff && i!=n){
//            printf("Found a candidate at %d place with %d sum", i, tmpsum);
            pos=i;
            sumdiff=(tmpsum-sum);
        }
    }
//    printf("Nearest block at %d index with sumdiff of %d.\n", pos, sumdiff);
    return pos;
}
