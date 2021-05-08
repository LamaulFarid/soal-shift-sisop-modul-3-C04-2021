#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<pthread.h> 
#include <sys/ipc.h>
#include <sys/shm.h>
#include<time.h>

int matriksA[4][3];
int matriksB[3][6];
int row[2], col[2];

void *kaliMatriks( void *ptr );

int main(void){
    row[0]=4;
    row[1]=3;
    col[0]=3;
    col[1]=6;
    
    // membuat matrix A 
    for(int i=0; i<row[0]; i++)
        for(int j=0; j<col[0]; j++)
            matriksA[i][j] = rand()%20+1;
    
    // membuat matrix B
    for(int i=0; i<row[1]; i++)
        for(int j=0; j<col[1]; j++)
            matriksB[i][j] = rand()%20+1;
    
    // print matrix A
    printf("Matriks A : \n");
    for(int i=0; i<row[0]; i++){
        for(int j=0; j<col[0]; j++)
            printf("%d ", matriksA[i][j]);
        printf("\n");
    }
    printf("\n");

    // print matrix B
    printf("Matriks B : \n");
    for(int i=0; i<row[1]; i++){
        for(int j=0; j<col[1]; j++)
            printf("%d ", matriksB[i][j]);
        printf("\n");
    }
    printf("\n");

    // shared memory
    key_t key = 1234;
    int *value;

    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    value = shmat(shmid, NULL, 0);

    int maximum = row[0]*col[1];
    value[0] = row[0];
    value[1] = col[1];
    
    // membuat thread
    pthread_t *thread;
    thread = (pthread_t *)malloc(maximum*sizeof(pthread_t));
    int n=0, *data;

    // Lakukan perkalian pada thread untuk setiap row dan col 
    for(int i=0; i<row[0]; i++){
        for(int j=0; j<col[1]; j++){
            data = (int *)malloc((24)*sizeof(int));
            data[0] = col[0];

            for(int k=0; k<col[0]; k++)
                data[k+1] = matriksA[i][k];
            for(int l=0; l<row[1]; l++)
                data[l+col[0]+1] = matriksB[l][j];
            
            pthread_create(&(thread[n++]), NULL, kaliMatriks, (void *) data);
        }
    }
    // print result
    printf("Hasil perkalian Matriks A dan B : \n");
    for(int i=0; i<maximum; i++){
        void *temp;
        pthread_join(thread[i], &temp);

        int *res = (int *)temp;
        value[i+col[0]] = *res;
        printf("%d ", *res);

        // pada setiap baris, jumlah kolom=6 print baris baru
        if((i+1)%col[1] == 0)
            printf("\n");
    }
    printf("\n");
    return 0;
}

void *kaliMatriks( void *ptr ){
    int *data, It, sum=0;
    data = (int *) ptr;
    It = data[0];
    for(int i=0; i<=It; i++){
        sum += data[i+1]*data[i+It+1];
    }
    int *res = (int *)malloc(sizeof(int));
    *res = sum;

    pthread_exit(res);
}
