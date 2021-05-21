#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char curPath[2000];

int checkFile(const char *path) {
    struct stat stat_path;
    stat(path, &stat_path);
    return S_ISREG(stat_path.st_mode);
}

void *filterFile(void *filePath) {
    char *ptr_ext;
    char ext[100];
    char currPath[1000];
    char oldFile[500];

    ptr_ext = strchr(filePath, '.');
    // printf("Pointer of Ext : %s\n", ptr_ext);

    ext[0] = '\0';
    if(ptr_ext) {
        ptr_ext++;
        for(int i=0; i<strlen(ptr_ext); i++) {
            ext[i] = tolower(ptr_ext[i]);
        }
        ext[strlen(ptr_ext)] = '\0';
    }
    // printf("Ekstensi : %s\n", ext);

    strcpy(oldFile, filePath);
    strcpy(currPath, curPath);
    char *fileName = strrchr(filePath, '/');
    // printf("File name : %s\n", (char *)filePath);
    memmove(&fileName[0], &fileName[1], strlen(fileName) - 0);


    if(fileName[0] == '.') {
        mkdir("hidden", 0777);
        strcat(currPath, "/hidden/");
        strcat(currPath, fileName);
        rename(oldFile, currPath);
    } else {
        if(strlen(ext) > 0 ) {
            mkdir(ext, 0777);
            strcat(currPath, "/");
            strcat(currPath, ext);
            strcat(currPath, "/");
            strcat(currPath, fileName);
            rename(oldFile,currPath);
        } else {
            mkdir("unknown", 0777);
            strcat(currPath, "/unknown/");
            strcat(currPath, fileName);
            rename(oldFile,currPath);
        }
    }
}

void filterDir(char *basePath, int threadSize) {
    char path[500][500], thisCode[500];
    DIR *directory = opendir(basePath);
    struct dirent *dirent_ptr;
    pthread_t tid[threadSize];
    int count=0;

    if(!directory) {
        return;
    }

    while((dirent_ptr = readdir(directory)) != NULL) {
        strcpy(path[count], basePath);
        strcat(path[count], "/");
        strcat(path[count], dirent_ptr->d_name);
        if(dirent_ptr->d_type == DT_REG) {
            strcpy(thisCode, basePath);
            strcat(thisCode, "/soal3.c");
            if(strcmp(path[count], thisCode) != 0) {
                pthread_create(&tid[count], NULL, filterFile, (void *)path[count]);
                
                count++;
            }
        } else if((dirent_ptr->d_type == DT_DIR) && (strcmp(dirent_ptr->d_name, ".") != 0)
                    && (strcmp(dirent_ptr->d_name, "..") != 0)) {
            
            DIR *directory2 = opendir(path[count]);
            struct dirent *dirent_ptr2;
            int threadSize2=0;
            while((dirent_ptr2 = readdir(directory2)) != NULL) {
                if(dirent_ptr2->d_type == DT_REG) {
                    threadSize2++;
                }
            }
            filterDir(path[count], threadSize2);
            closedir(directory2);
        }
    }

    for(int i=0; i<threadSize; i++) {
        pthread_join(tid[i], NULL);
    }

    closedir(directory);
}

int main(int argc, char const *argv[])
{
    getcwd(curPath, sizeof(curPath));
    if(strcmp(argv[1], "-f") == 0) {
        if(argc <= 2) {
            printf("Argument tidak sesuai\n");
            exit(1);
        }
        pthread_t tid[argc-2];
        int count=0;
        for(int i=2; i<argc; i++) {
            char filePath[2000];
            strcpy(filePath, argv[i]);

            if(checkFile(filePath)) {
                // printf("Current dir : %s\nFile Path : %s\n", curPath, filePath);
                pthread_create(&tid[count], NULL, filterFile, (void *)filePath);
                count++;
                printf("File %d: Berhasil Dikategorikan\n", i-1);
                
            } else {
                printf("File %d: Sad, gagal :(\n", i-1);
            }
        }
        for(int i=0; i<count; i++) {
            pthread_join(tid[i], NULL);
        }
    } else if(strcmp(argv[1], "-d") == 0) {
        char folderPath[400];
        strcpy(folderPath, argv[2]);

        if(argc <= 2) {
            printf("Argument tidak sesuai\n");
            exit(1);
        }
        DIR *fd = opendir(argv[2]);

        if(fd) {
            struct dirent *dp;
            int threadSize=0;

            while((dp = readdir(fd)) != NULL) {
                if(dp->d_type == DT_REG) {
                    threadSize++;
                }
            }
            filterDir(folderPath, threadSize);
            closedir(fd);
            printf("Direktori sukses disimpan!\n");
        } else if(ENOENT == errno) {
            printf("Yah, gagal disimpan :(\n");
        }
    } else if(strcmp(argv[1], "\*") == 0) {
        DIR *fd = opendir(curPath);
        struct dirent *dp;
        int threadSize=0;
        if(fd) {
            struct dirent *dp;
            int threadSize=0;

            while((dp = readdir(fd)) != NULL) {
                if(dp->d_type == DT_REG) {
                    threadSize++;
                }
            }
            filterDir(curPath, threadSize);
            closedir(fd);
            printf("Direktori sukses disimpan!\n");
        } else if(ENOENT == errno) {
            printf("Yah, gagal disimpan :(\n");
        }
    }
    return 0;
}

