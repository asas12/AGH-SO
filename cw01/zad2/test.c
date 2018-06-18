#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "library.h"

int rep = 1000;

struct rusage usage;
struct timeval startU, endU, startS, endS, startR, endR;

int stringToInt(char* s){
    int result=0;
    for(int i = 0; i<strlen(s); i++){
        result*=10;
        result+=(s[i]-'0');
    }
    return result;
}

void startTime(){
    gettimeofday(&startR, NULL);
    getrusage(RUSAGE_SELF, &usage);
    startU = usage.ru_utime;
    startS = usage.ru_stime;
}
void stopTime(){
    getrusage(RUSAGE_SELF, &usage);
    gettimeofday(&endR, NULL);
    endU = usage.ru_utime;
    endS = usage.ru_stime; 
    timersub(&endU, &startU, &endU);
    timersub(&endS, &startS, &endS);
    timersub(&endR, &startR, &endR);
}

void divTime(){
    endU.tv_sec/=rep;
    endU.tv_usec/=rep; 
    endS.tv_sec/=rep;
    endS.tv_usec/=rep;
    endR.tv_sec/=rep;
    endR.tv_usec/=rep;
}

int main(int argc, char* argv[]){
    if(argc==1){printf("You have to write what the program should do in command line arguments.\n"); return 0;}
    if(!strcmp(argv[1], "cs")){
        printf("Static array it is\n");
        if(argc<6){printf("Not enough operations.\n"); return 0;}
        FILE *f = fopen("raport2.txt", "w");
        if(f==NULL){ printf("Couldn't open the file\n"); return 0;}        
        startTime();
        createArrayStatic();
        for(int i=0; i<NoBlocks; i++){
                for(int j=0; j<SoBlocks-1; j++){
                    StaticArray[i][j]='A';
                }
                StaticArray[i][SoBlocks-1]='\0';
            }
        stopTime();
        printf("  Creating static array:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n", endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
        fprintf(f, "  Creating static array:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n", endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
       //process second and third operation
        for(int i=2; i<6; i+=2){
            int n = stringToInt(argv[i+1]);
            switch(argv[i][0]){
            case('a'):
                if(strlen(argv[i])>1 && argv[i][1] == 'r')
                {
                    printf("You have chosen add and remove %d times.\n", n);
                    startTime();
                    for(int k=0; k<rep; k++){
                        for(int j=0; j<n; j++){
                            insertBlockStatic(i%NoBlocks, "My string");
                            deleteBlockStatic(i%NoBlocks);
                         }
                    }
                    stopTime();
                    divTime();
                    printf("  Adding and removing %d times:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n, endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                    fprintf(f, "  Adding and removing %d times:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n ,endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);                  
                }
                else{
                    printf("You have chosen to add %d blocks.\n", n);
                    startTime();
                    for(int k=0; k<rep; k++){
                        for(int j=0; j<n; j++){
                            insertBlockStatic(i%NoBlocks, "My string");
                        }
                    }  
                    stopTime();
                    divTime();
                    printf("  Adding %d times:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n, endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                    fprintf(f, "  Adding %d times:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n ,endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                }
            break;
            case('r'):
                    printf("You have chosen to remove %d blocks.\n", n);
                    startTime();
                    for(int k=0; k<rep; k++){
                        for(int j=0; j<n; j++){
                            deleteBlockStatic(i%NoBlocks);
                        }                      
                    }
                    stopTime();
                    divTime();
                    printf("  Removing %d times:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n, endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                    fprintf(f, "  Removing %d times:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n ,endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
            break;
            case('s'):
                printf("You have chosen to search for nearest to %d. block.\n", n);
//check n!                insertBlockStatic(n, "Ha");
//                insertBlockStatic(n/2, "Ha");
                startTime();
//                for(int k=0; k<rep; k++){
                    nearestStaticSum(n);
//                }
                stopTime();
//                divTime();
                printf("  Searching block at %d index:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n, endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                fprintf(f, "  Searching block at %d index:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n ,endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
            break;
            default:
                printf("Incorrect syntax!\n");
            break;
            }
        }            
        deleteArrayStatic();
        fclose(f);    
    }
    else{
        if(!strcmp(argv[1], "cd")){
            printf("Dynamic array\n");
            if(argc<8){printf("Not enough operations.\n"); return 0;}
            int al = stringToInt(argv[2]);
            int bl = stringToInt(argv[3]);
            printf("%d, %d\n", al, bl);
            if(al<1 || bl<1){printf("Too small dimensions.\n"); return 0;}
            FILE *f = fopen("raport2.txt", "w");
            if(f==NULL){ printf("Couldn't open the file\n"); return 0;}        
            startTime();
            char** MyArray = createArrayDynamic(al, bl);
            //filling up the array with random stuff
//            for(int k=0; k<rep; k++){
                for(int i=0; i<al; i++){
                    for(int j=0; j<bl-1; j++){
                        MyArray[i][j]='A';
                    }
                    MyArray[i][bl-1]='\0';
                }
//            }
            stopTime();
            divTime();
            printf("  Creating dynamic array:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n", endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
            fprintf(f, "  Creating dynamic array:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n", endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);

            //process second and third operation
            for(int i=4; i<7; i+=2){ 
                int n = stringToInt(argv[i+1]);
                switch(argv[i][0]){
                case('a'):
                    if(strlen(argv[i])>1 && argv[i][1] == 'r'){
                    printf("You have chosen add and remove %d times.\n", n);
                    startTime();
//                    for(int k=0; k<rep; k++){
                        for(int j=0; j<n; j++){
                            MyArray = addBlockDynamic(j%al, al, bl, "AA", MyArray);
                            al+=1;
                            MyArray = deleteBlockDynamic(j%al, al, MyArray);
                            al-=1;
                        }
//                    }
                    stopTime();
//                    divTime();
                    printf("  Adding and removing %d times:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n, endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                   fprintf(f, "  Adding and removing %d times:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n ,endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                    }
                    else{
                    printf("You have chosen to add %d blocks.\n", n);
                    startTime();
//                    for(int k=0; k<rep; k++){
                        for(int j=0; j<n; j++){
                            MyArray = addBlockDynamic(j%al, al, bl, "AA", MyArray);
                            al+=1;
                        }
//                    }
                    stopTime();
//                    divTime();
                    printf("  Adding %d times:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n, endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                    fprintf(f, "  Adding %d times:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n ,endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                    }
                break;
                case('r'):
                    printf("You have chosen to remove %d blocks.\n", n);
                    startTime(); 
                    for(int k=0; k<rep; k++){
                        for(int j=0; j<n; j++){ 
                            MyArray = deleteBlockDynamic(j%al, al, MyArray);
                            al-=1;
                        }
                    }
                    stopTime();
                    divTime();
                    printf("  Removing %d times:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n, endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                   fprintf(f, "  Removing%d times:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n ,endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                break;
                case('s'):
                    printf("You have chosen to search for nearest to %d. block.\n", n);
//                    insertBlockDynamic(n/2, al, bl, "BB", MyArray);
//                    insertBlockDynamic(n, al, bl, "BB", MyArray);
                    startTime();
//                    for(int k=0; k<rep; k++){
                        nearestDynamicSum(n, al, MyArray);
//                    }
                    stopTime();
//                    divTime();
                    printf("  Searching block at %d index:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n, endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                   fprintf(f, "  Searching block at %d index:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n ,endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                break;
                default:
                    printf("Incorrect syntax!\n");
                break;
                }
            }            
            deleteArrayDynamic(MyArray, al);
        }
        else{
            printf("You should firstly create your array!");
        }
    }

return 0;
}
