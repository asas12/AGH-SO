#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>

struct rusage usage;
struct timeval startU, endU, startS, endS;

int stringToInt(char *s) {
    int result = 0;
    for (int i = 0; i < strlen(s); i++) {
        result *= 10;
        result += (s[i] - '0');
    }
    return result;
}

void generateLine(char *s, int n) {
    for (int i = 0; i < n - 1; i++) {
        //s[i] = 32+(rand()%95);
        s[i] = 97 + (rand() % 26);
    }
    s[n - 1] = ' ';
}

void startTime() {
    getrusage(RUSAGE_SELF, &usage);
    startU = usage.ru_utime;
    startS = usage.ru_stime;
}

void stopTime() {
    getrusage(RUSAGE_SELF, &usage);
    endU = usage.ru_utime;
    endS = usage.ru_stime;
    timersub(&endU, &startU, &endU);
    timersub(&endS, &startS, &endS);
}

int main(int argc, char *argv[]) {

    if (argc == 5 && !strcmp(argv[1], "generate")) {
        printf("You have chosen to generate data\n");
        int filedesc = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC,
                            S_IRUSR | S_IWUSR); //if doesnt exist-create, if does-del
        if (filedesc < 0) {
            printf("Can't create file!\n");
            return 1;
        }
        int NoR = stringToInt(argv[3]);
        int SoR = stringToInt(argv[4]);
        srand(time(0));
        char *buffer = malloc(SoR * sizeof(char));
        for (int i = 0; i < NoR; i++) {
            generateLine(buffer, SoR);
            write(filedesc, buffer, SoR);
        }
        if (close(filedesc) == 0) { return 0; }
        else {
            printf("Can't close the file\n");
            return 1;
        }
    } else {
        if (argc == 6 && !strcmp(argv[1], "sort")) {
            printf("Sorting ");
            if (!strcmp(argv[5], "sys")) {
                printf("with system functions");
                startTime();
                int filedesc = open(argv[2], O_RDWR);
                if (filedesc < 0) {
                    printf("Can't create file!\n");
                    return 1;
                }
                int NoR = stringToInt(argv[3]);
                int SoR = stringToInt(argv[4]);
                printf(" %d records with size %d\n", NoR, SoR);
                for (int i = SoR; i < NoR * SoR; i += SoR) {
                    char a;
                    int j = i;
                    if (lseek(filedesc, j, SEEK_SET) < 0) printf("wrong\n");
                    if (read(filedesc, &a, 1) <= 0) printf("Wrong\n");
                    char b;
                    if (lseek(filedesc, j - SoR, SEEK_SET) < 0) printf("wrong2\n");
                    if (read(filedesc, &b, 1) <= 0) printf("Wrong2\n");
                    lseek(filedesc, -1, SEEK_CUR);
                    while (a < b && j >= 2 * SoR) {
                        char *A = malloc(SoR * sizeof(char));
                        char *B = malloc(SoR * sizeof(char));
                        read(filedesc, B, SoR);
                        read(filedesc, A, SoR);
                        lseek(filedesc, j - SoR, SEEK_SET);
                        write(filedesc, A, SoR);
                        write(filedesc, B, SoR);
                        j -= SoR;
                        //if(j==0){break;} either this or if after this while
                        if (lseek(filedesc, j - SoR, SEEK_SET) < 0) printf("wrong3\n");
                        if (read(filedesc, &b, 1) <= 0) printf("Wrong3\n");
                        lseek(filedesc, -1, SEEK_CUR);
                    }
                    if (a < b) {
                        char *A = malloc(SoR * sizeof(char));
                        char *B = malloc(SoR * sizeof(char));
                        read(filedesc, B, SoR);
                        read(filedesc, A, SoR);
                        lseek(filedesc, j - SoR, SEEK_SET);
                        write(filedesc, A, SoR);
                        write(filedesc, B, SoR);
                    }
                }
                stopTime();
                printf("  User: %ld.%lds\n  System: %ld.%lds\n", endU.tv_sec, endU.tv_usec, endS.tv_sec, endS.tv_usec);
                if (close(filedesc) == 0) { return 0; }
                else {
                    printf("Can't close the file\n");
                    return 1;
                }
            } else {
                if (!strcmp(argv[5], "lib")) {
                    printf("with library functions");
                    startTime();
                    FILE *f = fopen(argv[2], "r+");
                    int NoR = stringToInt(argv[3]);
                    int SoR = stringToInt(argv[4]);
                    printf(" %d records with size %d\n", NoR, SoR);
                    for (int i = SoR; i < NoR * SoR; i += SoR) {
                        char a;
                        int j = i;
                        fseek(f, j, 0);
                        fread(&a, 1, 1, f);
                        char b;
                        fseek(f, j - SoR, 0);
                        fread(&b, 1, 1, f);
                        fseek(f, -1, 1);
                        while (a < b && j >= 2 * SoR) {
                            char *A = malloc(SoR * sizeof(char));
                            char *B = malloc(SoR * sizeof(char));
                            fread(B, 1, SoR, f);
                            fread(A, 1, SoR, f);
                            fseek(f, j - SoR, 0);
                            fwrite(A, 1, SoR, f);
                            fwrite(B, 1, SoR, f);
                            j -= SoR;
                            //if(j==0){break;} either this or if after this while
                            if (fseek(f, j - SoR, 0) != 0) printf("wrong3\n");
                            if (fread(&b, 1, 1, f) != 1) printf("Wrong3\n");
                            fseek(f, -1, 1);
                        }
                        if (a < b) {
                            char *A = malloc(SoR * sizeof(char));
                            char *B = malloc(SoR * sizeof(char));
                            fread(B, 1, SoR, f);
                            fread(A, 1, SoR, f);
                            fseek(f, j - SoR, 0);
                            fwrite(A, 1, SoR, f);
                            fwrite(B, 1, SoR, f);
                        }
                    }
                    stopTime();
                    printf("  User: %ld.%lds\n  System: %ld.%lds\n", endU.tv_sec, endU.tv_usec, endS.tv_sec,
                           endS.tv_usec);
                    if (fclose(f) == 0) { return 0; }
                    else {
                        printf("Can't close the file\n");
                        return 1;
                    }
                } else {
                    printf("Incorrect arguments\n");
                    return 1;
                }
            }
        } else {
            if (argc == 7 && !strcmp(argv[1], "copy")) {
                printf("Copying ");
                if (!strcmp(argv[6], "sys")) {
                    printf("with system functions");
                    startTime();
                    int fileFrom = open(argv[2], O_RDONLY);
                    if (fileFrom < 0) {
                        printf("Can't create file!\n");
                        return 1;
                    }
                    int fileTo = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                    int NoR = stringToInt(argv[4]);
                    int SoR = stringToInt(argv[5]);
                    printf(" %d records with size %d\n", NoR, SoR);
                    for (int i = 0; i < NoR; i++) {
                        char *A = malloc(SoR * sizeof(char));
                        read(fileFrom, A, SoR);
                        write(fileTo, A, SoR);
                    }
                    close(fileFrom);
                    close(fileTo);
                    stopTime();
                    printf("  User: %ld.%lds\n  System: %ld.%lds\n", endU.tv_sec, endU.tv_usec, endS.tv_sec,
                           endS.tv_usec);
                } else {
                    if (argc == 7 && !strcmp(argv[6], "lib")) {
                        printf("with library functions");
                        startTime();
                        FILE *fileFrom = fopen(argv[2], "r");
                        FILE *fileTo = fopen(argv[3], "w+");
                        int NoR = stringToInt(argv[4]);
                        int SoR = stringToInt(argv[5]);
                        printf(" %d records with size %d\n", NoR, SoR);
                        for (int i = 0; i < NoR; i++) {
                            char *A = malloc(SoR * sizeof(char));
                            fread(A, 1, SoR, fileFrom);
                            fwrite(A, 1, SoR, fileTo);
                        }
                        fclose(fileFrom);
                        fclose(fileTo);
                        stopTime();
                        printf("  User: %ld.%lds\n  System: %ld.%lds\n", endU.tv_sec, endU.tv_usec, endS.tv_sec,
                               endS.tv_usec);
                    } else {
                        printf("Incorrect arguments\n");
                        return 1;
                    }
                }
            } else {
                printf("Incorrect arguments\n");
                return 1;
            }
        }
    }

    return 0;
}
