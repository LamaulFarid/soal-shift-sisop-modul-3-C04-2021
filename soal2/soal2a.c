#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

long long matrix_1[4][3];
long long matrix_2[3][6];

int main(){
    key_t key = 4121;
    int shmid = shmget(key, 512, IPC_CREAT | 0666);
	void* memory = shmat(shmid, NULL, 0);
    long long (*result)[6] = memory;
	
    printf("[+] Masukkan matriks 4x3:\n");
	for(int i = 0; i<4; i++){
		for(int j = 0; j<3; ++j){
			scanf("%lld", &matrix_1[i][j]);
		}
	}

	printf("\n");

    printf("[+] Masukkan matriks 3x6:\n");
	for(int i=0; i<3; i++){
		for(int j=0; j<6; ++j){
			scanf("%lld", &matrix_2[i][j]);
		}
	}

	printf("\n");

    // memset 
	for(int i=0; i<4; i++){
		for(int j=0; j<6; j++){
			result[i][j] = 0;
		}
	}

    // hitung perkaliannya
    for(int i=0; i<4; i++){
        for(int j=0; j<6; j++){
            for(int k=0; k<3; k++){
                result[i][j] += matrix_1[i][k] * matrix_2[k][j];
            }
        }
    }

	printf("[!] Hasil output matriks\n");
    // outputkan matrixnya
	for(int i=0; i<4; i++){
		for(int j=0; j<6; j++){
			printf("%lld ", result[i][j]);
		}
        printf("\n");
	}

	return 0;
}
