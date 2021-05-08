#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

long long B[4][6];
long long result[4][6];
pthread_t thread_id[4][6];

typedef struct paramArgs {
    long long A;
    long long B;
    long long *address;
} Params;

void* perkaliancell(void* args){
    struct paramArgs *arg = args;
    long long A = arg->A;
    long long B = arg->B;
    long long *address = arg->address;

    if(A >= B){
        long long hasil=1;
        long long start = (A-B)+1;
        if(A-B == 0) start = 1;
        
        for(long long i=start; i<=A; i++){
            hasil *= i;
        }
        *address = hasil;
    }else if(B > A){
        long long hasil=1;
        for(long long i=1; i<=A; i++){
            hasil *= i;
        }
        *address = hasil;
    }else{
        *address = 0;
    }
}

int main(){

    key_t key = 4121;
    int shmid = shmget(key, 512, IPC_CREAT | 0666);
	void* memory = shmat(shmid, NULL, 0);
    long long (*A)[6] = memory;

    Params *argument;

	printf("[!] Hasil output matriks\n");
    // outputkan matrixnya
	for(int i=0; i<4; i++){
		for(int j=0; j<6; j++){
			printf("%lld ", A[i][j]);
		}
        printf("\n");
	}

    printf("\n");

    printf("[+] Masukkan matriks 4x6 yang akan di pakai:\n");
	for(int i = 0; i<4; i++){
		for(int j = 0; j<6; ++j){
			scanf("%lld", &B[i][j]);
		}
	}
    printf("\n");

    // Perkalian cell barunya
    for(int i=0; i<4; i++){
		for(int j=0; j<6; j++){
            argument = (Params*) malloc(sizeof(Params));
            argument->A = A[i][j];
            argument->B = B[i][j];
            argument->address = &result[i][j];
            
            pthread_create(&thread_id[i][j], NULL, &perkaliancell, (void *)argument);
		}
	}

    // tungguiin
    for(int i=0; i<4; i++){
		for(int j=0; j<6; j++){
            pthread_join(thread_id[i][j], NULL);
		}
	}

    printf("[+] Hasil setelah fungsi selesai dilakukan\n");
    for(int i=0; i<4; i++){
		for(int j=0; j<6; j++){
            printf("%lld ", result[i][j]);
		}
        printf("\n");
	}
	return 0;
}
