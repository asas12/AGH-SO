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

int stringToInt(char *s) {
    int result = 0;
    for (int i = 0; i < strlen(s); i++) {
        result *= 10;
        result += (s[i] - '0');
    }
    return result;
}

int cmpTM(struct tm first, struct tm second) {
    if (first.tm_year < second.tm_year) {
        return -1;
    } else if (first.tm_year > second.tm_year) {
        return 1;
    }
    //same year
    if (first.tm_mon < second.tm_mon) {
        return -1;
    } else if (first.tm_mon > second.tm_mon) {
        return 1;
    }
    //same month
    if (first.tm_mday < second.tm_mday) {
        return -1;
    } else if (first.tm_mday > second.tm_mday) {
        return 1;
    }
    //same date
    return 0;
}

void searchOlder(char *path, struct tm time) {
    DIR *root = opendir(path);
    if (root != NULL) {
        struct dirent *file = readdir(root);
        while (file != NULL) {
            struct stat fileInfo;
            char *filePath = malloc(200 * sizeof(char));
            strcpy(filePath, path);
            strcat(filePath, "/");
            strcat(filePath, file->d_name);
            lstat(filePath, &fileInfo);
            if ((fileInfo.st_mode & S_IFMT) == S_IFREG) {
                time_t modTime = fileInfo.st_mtime;
                struct tm *modTimeTM;
                modTimeTM = localtime(&modTime);
//                sscanf(argv[3], "%d/%d/%d", &time.tm_mday, &time.tm_mon, &time.tm_year);
                if (cmpTM(*modTimeTM, time) == -1) {
                    printf("%s\n %ldB  %d/%d/%d  ", filePath, fileInfo.st_size, modTimeTM->tm_mday,
                           modTimeTM->tm_mon + 1, modTimeTM->tm_year + 1900);
                    printf((fileInfo.st_mode & S_IRUSR) ? "r" : "-");
                    printf((fileInfo.st_mode & S_IWUSR) ? "w" : "-");
                    printf((fileInfo.st_mode & S_IXUSR) ? "x" : "-");
                    printf((fileInfo.st_mode & S_IRGRP) ? "r" : "-");
                    printf((fileInfo.st_mode & S_IWGRP) ? "w" : "-");
                    printf((fileInfo.st_mode & S_IXGRP) ? "x" : "-");
                    printf((fileInfo.st_mode & S_IROTH) ? "r" : "-");
                    printf((fileInfo.st_mode & S_IWOTH) ? "w" : "-");
                    printf((fileInfo.st_mode & S_IXOTH) ? "x" : "-");
                    printf("\n");
                }
            } else {
                if (((fileInfo.st_mode & S_IFMT) == S_IFDIR) && strcmp(file->d_name, "..") &&
                    strcmp(file->d_name, ".")) {
                    searchOlder(filePath, time);
                }
            }
            file = readdir(root);
            free(filePath);
        }
    }
    closedir(root);
}

void searchNewer(char *path, struct tm time) {
    DIR *root = opendir(path);
    if (root != NULL) {
        struct dirent *file = readdir(root);
        while (file != NULL) {
            struct stat fileInfo;
            char *filePath = malloc(200 * sizeof(char));
            strcpy(filePath, path);
            strcat(filePath, "/");
            strcat(filePath, file->d_name);
            lstat(filePath, &fileInfo);
            if ((fileInfo.st_mode & S_IFMT) == S_IFREG) {
                time_t modTime = fileInfo.st_mtime;
                struct tm *modTimeTM;
                modTimeTM = localtime(&modTime);
                if (cmpTM(*modTimeTM, time) == 1) {
                    printf("%s\n %ldB  %d/%d/%d  ", filePath, fileInfo.st_size, modTimeTM->tm_mday,
                           modTimeTM->tm_mon + 1, modTimeTM->tm_year + 1900);
                    printf((fileInfo.st_mode & S_IRUSR) ? "r" : "-");
                    printf((fileInfo.st_mode & S_IWUSR) ? "w" : "-");
                    printf((fileInfo.st_mode & S_IXUSR) ? "x" : "-");
                    printf((fileInfo.st_mode & S_IRGRP) ? "r" : "-");
                    printf((fileInfo.st_mode & S_IWGRP) ? "w" : "-");
                    printf((fileInfo.st_mode & S_IXGRP) ? "x" : "-");
                    printf((fileInfo.st_mode & S_IROTH) ? "r" : "-");
                    printf((fileInfo.st_mode & S_IWOTH) ? "w" : "-");
                    printf((fileInfo.st_mode & S_IXOTH) ? "x" : "-");
                    printf("\n");
                }
            } else {
                if (((fileInfo.st_mode & S_IFMT) == S_IFDIR) && strcmp(file->d_name, "..") &&
                    strcmp(file->d_name, ".")) {
                    searchNewer(filePath, time);
                }
            }
            file = readdir(root);
            free(filePath);
        }
    }
    closedir(root);
}

void searchDate(char *path, struct tm time) {
    DIR *root = opendir(path);
    if (root != NULL) {
        struct dirent *file = readdir(root);
        while (file != NULL) {
            struct stat fileInfo;
            char *filePath = malloc(200 * sizeof(char));
            strcpy(filePath, path);
            strcat(filePath, "/");
            strcat(filePath, file->d_name);
            lstat(filePath, &fileInfo);
            if ((fileInfo.st_mode & S_IFMT) == S_IFREG) {
                time_t modTime = fileInfo.st_mtime;
                struct tm *modTimeTM;
                modTimeTM = localtime(&modTime);
                if (cmpTM(*modTimeTM, time) == 0) {
                    printf("%s\n %ldB  %d/%d/%d  ", filePath, fileInfo.st_size, modTimeTM->tm_mday,
                           modTimeTM->tm_mon + 1, modTimeTM->tm_year + 1900);
                    printf((fileInfo.st_mode & S_IRUSR) ? "r" : "-");
                    printf((fileInfo.st_mode & S_IWUSR) ? "w" : "-");
                    printf((fileInfo.st_mode & S_IXUSR) ? "x" : "-");
                    printf((fileInfo.st_mode & S_IRGRP) ? "r" : "-");
                    printf((fileInfo.st_mode & S_IWGRP) ? "w" : "-");
                    printf((fileInfo.st_mode & S_IXGRP) ? "x" : "-");
                    printf((fileInfo.st_mode & S_IROTH) ? "r" : "-");
                    printf((fileInfo.st_mode & S_IWOTH) ? "w" : "-");
                    printf((fileInfo.st_mode & S_IXOTH) ? "x" : "-");
                    printf("\n");
                }
            } else {
                if (((fileInfo.st_mode & S_IFMT) == S_IFDIR) && strcmp(file->d_name, "..") &&
                    strcmp(file->d_name, ".")) {
                    //printf("dir: %s\n", file->d_name);
                    //printf("Into %s\n", filePath);
                    searchDate(filePath, time);
                }
            }
            file = readdir(root);
            free(filePath);
        }
    }
    closedir(root);
}

int main(int argc, char *argv[]) {

    if (argc == 4) {
        char *path = malloc(300 * sizeof(char));
        if (argv[1][0] == '/') { strcpy(path, argv[1]); }
        else {
            realpath(argv[1], path);
        }
        struct tm time;
        sscanf(argv[3], "%d/%d/%d", &time.tm_mday, &time.tm_mon, &time.tm_year);
        time.tm_year -= 1900;
        time.tm_mon -= 1;
        time.tm_sec = 0;
        time.tm_min = 0;
        time.tm_hour = 0;
        if (argv[2][0] == '<') {
            searchOlder(path, time);
        } else {
            if (argv[2][0] == '>') {
                searchNewer(path, time);
            } else {
                if (argv[2][0] == '=') {
                    searchDate(path, time);
                }
            }
        }
        free(path);
    }
    return 0;
}
