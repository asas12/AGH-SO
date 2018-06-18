#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <dlfcn.h>

#define NoBlocks 1000000
#define SoBlocks 100

int rep = 1000.0;

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
        void* handle = dlopen("./libzad3_shared.so", RTLD_LAZY);
        if(!handle){printf("Cant link library\n"); return 0;}
        //int* NoB = (int*) dlsym(handle, "NoBlocks");
        //int NoBlocks = *NoB;
        //int* SoB = (int*) dlsym(handle, "SoBlocks");
        //int SoBlocks = *SoB;
//        char StaticArray[NoBlocks][SoBlocks];
        char * StaticArray = dlsym(handle, "StaticArray");
        void (*CAS) (void) = dlsym(handle, "createArrayStatic");
        startTime();
        CAS();
/*        for(int i=0; i<NoBlocks; i++){
                for(int j=0; j<SoBlocks-1; j++){
                   *((char*)StaticArray+i*NoBlocks+j)='A';
                }
                   *((char*)StaticArray+i*NoBlocks+SoBlocks-1)='A';
//StaticArray[i][SoBlocks-1]='\0';
            }*/
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
                    void (*IBS) (int, char*) = dlsym(handle, "insertBlockStatic");
                    void (*DBS) (int) = dlsym(handle, "deleteBlockStatic");
                    startTime();
                    for(int k=0; k<rep; k++){
                        for(int j=0; j<n; j++){
                            IBS(i%NoBlocks, "My string");
                            DBS(i%NoBlocks);
                         }
                    }
                    stopTime();
                    divTime();
                    printf("  Adding and removing %d times:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n, endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                    fprintf(f, "  Adding and removing %d times:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n ,endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);                  
                }
                else{
                    printf("You have chosen to add %d blocks.\n", n);
                    void (*IBS) (int, char*) = dlsym(handle, "insertBlockStatic");
                    startTime();
                    for(int k=0; k<rep; k++){
                        for(int j=0; j<n; j++){
                            IBS(i%NoBlocks, "My string");
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
                    void (*DBS) (int) = dlsym(handle, "deleteBlockStatic");                 
                    startTime();
                    for(int k=0; k<rep; k++){
                        for(int j=0; j<n; j++){
                            DBS(i%NoBlocks);
                        }                      
                    }
                    stopTime();
                    divTime();
                    printf("  Removing %d times:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n, endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                    fprintf(f, "  Removing %d times:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n ,endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
            break;
            case('s'):
                printf("You have chosen to search for nearest to %d. block.\n", n);
                int (*NSS) (int) = dlsym(handle, "nearestStaticSum");
//check n!                insertBlockStatic(n, "Ha");
//                insertBlockStatic(n/2, "Ha");
                startTime();
//                for(int k=0; k<rep; k++){
                    NSS(n);
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
        void (*DAS) (void) = dlsym(handle, "deleteArrayStatic");
        DAS();
        fclose(f);   
        dlclose(handle); 
    }
    else{
        if(!strcmp(argv[1], "cd")){
            printf("Dynamic array\n");
            if(argc<8){printf("Not enough operations.\n"); return 0;}
            int al = stringToInt(argv[2]);
            int bl = stringToInt(argv[3]);
            if(al<1 || bl<1){printf("Too small dimensions.\n"); return 0;}
            FILE *f = fopen("raport2.txt", "w");
            if(f==NULL){ printf("Couldn't open the file\n"); return 0;}        
            void* handle = dlopen("./libzad3_shared.so", RTLD_LAZY);
            if(!handle){printf("Cant link library\n"); return 0;}
            char** (*CAD) (int, int) = dlsym(handle, "createArrayDynamic");
            startTime();
            char** MyArray = CAD(al, bl);
            //filling up the array with random stuff
//            for(int k=0; k<rep; k++){
                for(int i=0; i<al; i++){
                    for(int j=0; j<bl-1; j++){
                        MyArray[i][j]='A';
                    }
                    MyArray[i][bl-1]='\0';
//                }
            }
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
                    char** (*ABD) (int, int, int, char*, char**) = dlsym(handle, "addBlockDynamic");
                    char** (*DBD) (int, int, char**) = dlsym(handle, "deleteBlockDynamic");
                    startTime();
//                    for(int k=0; k<rep; k++){
                        for(int j=0; j<n; j++){
                            MyArray = ABD(j%al, al, bl, "AA", MyArray);
                            al+=1;
                            MyArray = DBD(j%al, al, MyArray);
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
                    char** (*ABD) (int, int, int, char*, char**) = dlsym(handle, "addBlockDynamic");
                    startTime();
//                    for(int k=0; k<rep; k++){
                        for(int j=0; j<n; j++){
                            MyArray = ABD(j%al, al, bl, "AA", MyArray);
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
                    char** (*DBD)(int, int, char**) = dlsym(handle, "deleteBlockDynamic");
                    startTime(); 
//                    for(int k=0; k<rep; k++){
                        for(int j=0; j<n; j++){ 
                            MyArray = DBD(j%al, al, MyArray);
                            al-=1;
//                        }
                    }
                    stopTime();
//                    divTime();
                    printf("  Removing %d times:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n, endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                   fprintf(f, "  Removing%d times:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n ,endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                break;
                case('s'):
                    printf("You have chosen to search for nearest to %d. block.\n", n);
//                    insertBlockDynamic(n/2, al, bl, "BB", MyArray);
//                    insertBlockDynamic(n, al, bl, "BB", MyArray);
                    int (*NDS)(int, int, char**) = dlsym(handle, "nearestDynamicSum");
                    startTime();
//                    for(int k=0; k<rep; k++){
                        NDS(n, al, MyArray);
//                    }
//                    stopTime();
                    divTime();
                    printf("  Searching block at %d index:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n, endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                   fprintf(f, "  Searching block at %d index:\n    User: %ld.%lds\n    System: %ld.%lds\n    Real %ld.%.lds\n",n ,endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_sec, endR.tv_sec, endR.tv_usec);
                break;
                default:
                    printf("Incorrect syntax!\n");
                break;
                }
            }   
            void (*DAD) (char**, int) = dlsym(handle, "deleteArrayDynamic");         
            DAD(MyArray, al);
            dlclose(handle);
        }
        else{
            printf("You should firstly create your array!");
        }
    }

return 0;
}
