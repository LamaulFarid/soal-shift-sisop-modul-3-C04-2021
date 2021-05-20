#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pthread_t thread_id[1000];
int thread_id_index;
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

    printf("Thread Id [%d]\n", thread_id_index);
    ptr_ext = strrchr(filePath, '.');

    ext[0] = '\0';
    if(ptr_ext) {
        ptr_ext++;
        for(int i=0; i<strlen(ptr_ext); i++) {
            ext[i] = tolower(ptr_ext[i]);
        }
        ext[strlen(ptr_ext)] = '\0';
    }
    printf("Ekstensi : %s\n", ext);

    strcpy(oldFile, filePath);
    strcpy(currPath, curPath);
    char *fileName = strrchr(filePath, '/');
    memmove(&fileName[0], &fileName[1], strlen(fileName) - 0);

    printf("File name : %s\n", fileName);

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

    pthread_exit(0);
}

void filterDir(char *basePath) {
    char path[500], thisCode[500];
    struct dirent *dirent_ptr;
    DIR *directory = opendir(basePath);

    if(!directory) {
        return;
    }

    while((dirent_ptr = readdir(directory)) != NULL) {
        if(strcmp(dirent_ptr->d_name, ".") != 0 && strcmp(dirent_ptr->d_name, "..") != 0) {
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dirent_ptr->d_name);
            if(dirent_ptr->d_type == DT_REG) {
                strcpy(thisCode, basePath);
                strcat(thisCode, "/soal3.c");
                if(strcmp(path, thisCode) != 0) {
                    pthread_create(&thread_id[thread_id_index], NULL, filterFile, (void *)path);
                    sleep(1);
                    thread_id_index++;
                }
            } else if(dirent_ptr->d_name[0] != '.') {
                filterDir(path);
            }
        }
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
        for(int i=2; i<argc; i++) {
            char filePath[2000];
            strcpy(filePath, argv[i]);

            if(checkFile(filePath)) {
                printf("Current dir : %s\nFile Path : %s\n", curPath, filePath);
                pthread_create(&thread_id[thread_id_index], NULL, filterFile, (void *)filePath);

                printf("File %d: Berhasil Dikategorikan\n", i-1);
                sleep(1);
                ++thread_id_index;
            } else {
                printf("File %d: Sad, gagal :(\n", i-1);
            }
        }
        for(int i=0; i<thread_id_index; i++) {
            pthread_join(thread_id[i], NULL);
        }
    } else if(strcmp(argv[1], "-d") == 0) {
        if(argc <= 2) {
            printf("Argument tidak sesuai\n");
            exit(1);
        }
        char basePath[500];
        int oldThreadId, newThreadId;

        strcpy(basePath, argv[2]);
        oldThreadId = thread_id_index;
        filterDir(basePath);
        newThreadId = thread_id_index;
        int sukses = 1;

        for(int i=oldThreadId; i<newThreadId; i++) {
            if(pthread_join(thread_id[i], NULL)) {
                sukses = 0;
                printf("Thread gagal : %d\n", i);
            }
        }
        if(sukses) {
            printf("Direktori sukses disimpan!\n");
        } else {
            printf("Yah, gagal disimpan :(\n");
        }
    } else if(strcmp(argv[1], "\*") == 0) {
        int oldThreadId, newThreadId;
        
        oldThreadId = thread_id_index;
        filterDir(curPath);
        newThreadId = thread_id_index;
        int sukses = 1;

        for(int i=oldThreadId; i<newThreadId; i++) {
            if(pthread_join(thread_id[i], NULL)) {
                sukses = 0;
                printf("Thread gagal : %d\n", i);
            }
        }
        if(sukses) {
            printf("Direktori sukses disimpan!\n");
        } else {
            printf("Yah, gagal disimpan :(\n");
        }
    }
    return 0;
}

