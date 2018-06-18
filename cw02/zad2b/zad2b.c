#define _XOPEN_SOURCE 500
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
#include <ftw.h>

struct tm date;

int cmpTM(struct tm first, struct tm second){
    if(first.tm_year<second.tm_year){
        return -1;
    }else if(first.tm_year>second.tm_year){
            return 1;
            }
    //same year
    if(first.tm_mon<second.tm_mon){
        return -1;
    }else if(first.tm_mon>second.tm_mon){
            return 1;
        }   
    //same month
    if(first.tm_mday<second.tm_mday){
        return -1;
    }else if(first.tm_mday>second.tm_mday){
        return 1;
    }
    //same date
    return 0;
}

int searchOlder(const char* path, const struct stat* fileInfo, int fileflags, struct FTW* pftw){
    if(fileflags == FTW_SL){return 0;}
    time_t modTime = fileInfo->st_mtime;
    struct tm * modTimeTM;
    modTimeTM = localtime(&modTime);
    if(fileflags == FTW_F && cmpTM(*modTimeTM, date)==-1){
        printf("%s\n", path);
        printf(" %ldB  %d/%d/%d  ", fileInfo->st_size, modTimeTM->tm_mday, modTimeTM->tm_mon+1, modTimeTM->tm_year+1900); 
                    printf((fileInfo->st_mode & S_IRUSR) ? "r" : "-");
                    printf((fileInfo->st_mode & S_IWUSR) ? "w" : "-");
                    printf((fileInfo->st_mode & S_IXUSR) ? "x" : "-");
                    printf((fileInfo->st_mode & S_IRGRP) ? "r" : "-");
                    printf((fileInfo->st_mode & S_IWGRP) ? "w" : "-");
                    printf((fileInfo->st_mode & S_IXGRP) ? "x" : "-");
                    printf((fileInfo->st_mode & S_IROTH) ? "r" : "-");
                    printf((fileInfo->st_mode & S_IWOTH) ? "w" : "-");
                    printf((fileInfo->st_mode & S_IXOTH) ? "x" : "-");
                    printf("\n");
         return 0;
    }
    else return 0;
}

int searchNewer(const char* path, const struct stat* fileInfo, int fileflags, struct FTW* pftw){
    if(fileflags == FTW_SL){return 0;}
    time_t modTime = fileInfo->st_mtime;
    struct tm * modTimeTM;
    modTimeTM = localtime(&modTime);
    if(fileflags == FTW_F && cmpTM(*modTimeTM, date)==1){
        printf("%s\n", path);
        printf(" %ldB  %d/%d/%d  ", fileInfo->st_size, modTimeTM->tm_mday, modTimeTM->tm_mon+1, modTimeTM->tm_year+1900); 
                    printf((fileInfo->st_mode & S_IRUSR) ? "r" : "-");
                    printf((fileInfo->st_mode & S_IWUSR) ? "w" : "-");
                    printf((fileInfo->st_mode & S_IXUSR) ? "x" : "-");
                    printf((fileInfo->st_mode & S_IRGRP) ? "r" : "-");
                    printf((fileInfo->st_mode & S_IWGRP) ? "w" : "-");
                    printf((fileInfo->st_mode & S_IXGRP) ? "x" : "-");
                    printf((fileInfo->st_mode & S_IROTH) ? "r" : "-");
                    printf((fileInfo->st_mode & S_IWOTH) ? "w" : "-");
                    printf((fileInfo->st_mode & S_IXOTH) ? "x" : "-");
                    printf("\n");
         return 0;
    }
    else return 0;
}

int searchDate(const char* path, const struct stat* fileInfo, int fileflags, struct FTW* pftw){
    if(fileflags == FTW_SL){return 0;}
    time_t modTime = fileInfo->st_mtime;
    struct tm * modTimeTM;
    modTimeTM = localtime(&modTime);
    if(fileflags == FTW_F && cmpTM(*modTimeTM, date)==0){
        printf("%s\n", path);
        printf(" %ldB  %d/%d/%d  ", fileInfo->st_size, modTimeTM->tm_mday, modTimeTM->tm_mon+1, modTimeTM->tm_year+1900); 
                    printf((fileInfo->st_mode & S_IRUSR) ? "r" : "-");
                    printf((fileInfo->st_mode & S_IWUSR) ? "w" : "-");
                    printf((fileInfo->st_mode & S_IXUSR) ? "x" : "-");
                    printf((fileInfo->st_mode & S_IRGRP) ? "r" : "-");
                    printf((fileInfo->st_mode & S_IWGRP) ? "w" : "-");
                    printf((fileInfo->st_mode & S_IXGRP) ? "x" : "-");
                    printf((fileInfo->st_mode & S_IROTH) ? "r" : "-");
                    printf((fileInfo->st_mode & S_IWOTH) ? "w" : "-");
                    printf((fileInfo->st_mode & S_IXOTH) ? "x" : "-");
                    printf("\n"); 
        return 0;
    }
    else return 0;
}

int main(int argc, char* argv[]){

    if(argc==4){
        char* path = malloc(300*sizeof(char));
        if(argv[1][0] == '/'){strcpy(path, argv[1]);}
        else{
            realpath(argv[1], path);
        }
        sscanf(argv[3], "%d/%d/%d", &date.tm_mday, &date.tm_mon, &date.tm_year);
        date.tm_year-=1900;
        date.tm_mon-=1;
        date.tm_sec=0;
        date.tm_min=0;
        date.tm_hour=0;
        int flag = FTW_DEPTH | FTW_PHYS;    //go into folders before using fn on them | do not follow SL
        if(argv[2][0] == '<'){
            nftw(path, searchOlder, 10, flag);
        }
        else{
            if(argv[2][0] == '>'){
                nftw(path, searchNewer, 10, flag);
            }
            else{
                if(argv[2][0] == '='){
                  nftw(path, searchDate, 10, flag);
                }
            }
        }
        free(path);
    }
    return 0;    
}
