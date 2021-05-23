# Kelompok C04 Modul 3 Sistem Operasi C
- I Kadek Agus Ariesta Putra 05111940000105
- Muhammad Arif Faizin 05111940000060
- Ahmad Lamaul Farid 05111940000134

# Soal
Link soal [Soal Shift 3](https://docs.google.com/document/d/1ud1JyncoSDAo5hA03wPvjn7QBHPUeUG1eBJ8ETtq2dQ/edit)

## Soal 1
## Soal 2

pada soal nomor 2 ini kita diminta untuk membuat program dalam bahasa C dengan ketentuan sebagai berikut : 

## A

Membuat program perkalian matrix (4x3 dengan 3x6) dan menampilkan hasilnya.
```c
    key_t key = 4121;
    int shmid = shmget(key, 512, IPC_CREAT | 0666);
	void* memory = shmat(shmid, NULL, 0);
    long long (*result)[6] = memory;
```
Kami disini menggunakan shared memory, karena pada `soal2a` masih berhubungan dengan soal2b (untuk melakukan pertukaran data antar program).
```c
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
```
Kami menggunakan input sendiri (dapat berubah), karena menurut kami itu lebih fleksibel, dimana dengan memasukkan nilai pada matriks 1 dan 2 yang kemudian nanti nilainya dapat diubah sesuai kehendak pengguna.
```c
	for(int i=0; i<4; i++){
		for(int j=0; j<6; j++){
			result[i][j] = 0;
		}
	}
```
Setelah itu, hasilnya di-set terlebih dahulu ke 0 (nol).
```c
    for(int i=0; i<4; i++){
        for(int j=0; j<6; j++){
            for(int k=0; k<3; k++){
                result[i][j] += matrix_1[i][k] * matrix_2[k][j];
            }
        }
    }

	printf("[!] Hasil output matriks\n");
    
	for(int i=0; i<4; i++){
		for(int j=0; j<6; j++){
			printf("%lld ", result[i][j]);
		}
        printf("\n");
	}
```
Kemudian dihitung perkalian matriks 1 dan 2 serta hasilnya disimpan ke shared memory, setelah itu di cetak hasilnya. Untuk contohnya sebagai berikut : 

Input matriks 1 dan 2
```
[+] Masukkan matriks 4x3:
4 1 4
2 1 3
4 2 2
1 1 4

[+] Masukkan matriks 3x6:
2 1 3 2 0 3
1 4 4 0 0 2
1 1 0 1 2 1
```

Hasil output
```
[!] Hasil output matriks
13 12 16 12 8 18 
8 9 10 7 6 11 
12 14 20 10 4 18 
7 9 7 6 8 9 
```

![Program `soal2a.c` ketika dijalankan](docs/soal2a.JPG)
## B

Membuat program dengan menggunakan matriks output dari program `soal2a`. Kemudian matriks tersebut akan dilakukan perhitungan dengan matrix baru. Perhitungannya adalah setiap cel yang berasal dari matriks A menjadi angka untuk faktorial, lalu cel dari matriks B menjadi batas maksimal faktorialnya (dari paling besar ke paling kecil) atau dengan syarat : 
```
If a >= b -> a!/(a-b)!
If b > a -> a!
If 0 -> 0
```

```c
typedef struct paramArgs {
    long long A;
    long long B;
    long long *address;
} Params;
```

Kami membuat `struct ParamArgs` yang nantinya akan digunakan untuk menyimpan matriks hasil dari `soal2a` (matriks A), matriks inputan baru (matriks B) dan hasil faktorial dari matriks A dan matriks B.
```c
void* perkaliancell(void* args){
    struct paramArgs *arg = args;
    long long A = arg->A;
    long long B = arg->B;
    long long *address = arg->address;

    if(A >= B){
        long long hasil=1;
        long long start = (A-B)+1;
        if(A-B == 0) start = 1;
        if(B==0) hasil = 0;
        
        for(long long i=start; i<=A; i++){
            hasil *= i;
        }
        *address = hasil;
    }else if(B > A){
        long long hasil=1;
        if(A==0) hasil = 0;
        for(long long i=1; i<=A; i++){
            hasil *= i;
        }
        *address = hasil;
    }
}
```

Kami disini juga membuat fungsi `perkaliancell()` untuk melakukan perhitungan pada setiap cel yang berasal dari matriks A daan matriks B dengan syarat : 
```
If a >= b -> a!/(a-b)!
If b > a -> a!
If 0 -> 0
```

Setelah didapatkan hasil dari perhitungan matriks A dan matriks B, kemudian hasilnya akan disimpan ke variabel `address` pada `struct ParamArgs`.
```c
    key_t key = 4121;
    int shmid = shmget(key, 512, IPC_CREAT | 0666);
	void* memory = shmat(shmid, NULL, 0);
    long long (*A)[6] = memory;

    Params *argument
```

Kami juga menggunakan shared memory untuk melakukan pertukaran data antar program `soal2a`.
```c
	printf("[!] Hasil output matriks\n");
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
```

Hasil dari output `soal2a` disimpan pada matriks A kemudian dicetak hasilnya dan kita inputkan matriks yang baru disimpan pada matriks B.
```c
    for(int i=0; i<4; i++){
		for(int j=0; j<6; j++){
            argument = (Params*) malloc(sizeof(Params));
            argument->A = A[i][j];
            argument->B = B[i][j];
            argument->address = &result[i][j];
            
            pthread_create(&thread_id[i][j], NULL, &perkaliancell, (void *)argument);
		}
	}
```

Variabel `argument->A` digunakan untuk menyimpan matriks A `argument->B` digunakan untuk menyimpan matriks B, dan `argument->address` digunakan untuk menyimpan hasil faktorial dari matriks A dan matriks B. Kemudian dibuat `thread` sebanyak 24 kali (matriks 4x6) untuk memanggil fungsi `perkaliancell()` seperti yang dijelaskan diatas. 
```c
    for(int i=0; i<4; i++){
		for(int j=0; j<6; j++){
            pthread_join(thread_id[i][j], NULL);
		}
	}
```

Setelah itu `thread` akan di-wait sampai `thread` selesai berjalan.
```c
    printf("[+] Hasil setelah fungsi selesai dilakukan\n");
    for(int i=0; i<4; i++){
		for(int j=0; j<6; j++){
            printf("%lld ", result[i][j]);
		}
        printf("\n");
	}
```

Kemudian hasil faktorial dari matriks A dan matriks B akan dicetak. 

![Program `soal2b.c` ketika dijalankan](docs/soal2b.JPG)

## C

Membuat program (soal2c.c) untuk mengecek 5 proses teratas apa saja yang memakan resource komputernya dengan command `ps aux | sort -nrk 3,3 | head -5`
```c
void command1() {
    dup2(pipe_[0][1], 1); 
    
    close(pipe_[0][0]);
    close(pipe_[0][1]);

    execlp("ps", "ps", "-aux", NULL);
    exit(EXIT_SUCCESS);
}

void command2() {
    dup2(pipe_[0][0], 0);   
    dup2(pipe_[1][1], 1);  

    close(pipe_[0][0]);
    close(pipe_[0][1]);
    close(pipe_[1][0]);
    close(pipe_[1][1]);

    execlp("sort", "sort", "-nrk", "3,3", NULL);
    exit(EXIT_SUCCESS);
}

void command3() {
    dup2(pipe_[1][0], 0); 

    close(pipe_[1][0]);
    close(pipe_[1][1]);
    
    execlp("head", "head", "-5", NULL);
    exit(EXIT_SUCCESS);
}
```

Kami membuat fungsi `command1()` untuk menyimpan perintah `ps aux`, fungsi `command2()` untuk menyimpan perintah `sort -nrk 3,3` dan fungsi `command3()` untuk menyimpan perintah `head -5`.
```c
    if(pipe(pipe_[0]) == -1){
        perror("gagal membuat pipe");
        exit(EXIT_FAILURE);
    }

    if(fork() == 0) command1();

    if(pipe(pipe_[1]) == -1){
        perror("gagal membuat pipe");
        exit(EXIT_FAILURE);
    }

    if(fork() == 0) command2();

    close(pipe_[0][0]);
    close(pipe_[0][1]);

    if(fork() == 0) command3();
```

Kemudian pada fungsi `main()`, terlebih dahulu dibuat `pipe_[0]` jika gagal membuat `pipe` maka cetak `gagal membuat pipe`. Setelah itu, dibuat fork untuk memanggil fungsi `command1()` dan kemudian dibuat `pipe_[1]` jika gagal membuat `pipe` maka cetak `gagal membuat pipe`. Setelah itu, dibuat fork untuk memanggil fungsi `command2()`. Lalu, fds yang tidak terpakai ditutup dan dibuat fork untuk memanggil fungsi `command3()`.

## Soal 3
Pada soal no 3, diminta untuk membuat program dengan kriteria berikut :
* Memindahkan file ke folder dengan nama sesuai dengan ekstensinya, sesuai dengan command yang diberikan sebagai berikut :
    * menggunakan parameter `-f` untuk menerima input beberapa file. Contoh `./soal3 -f path/to/file.extension [path/to/file2.extension] ...` Apabila menggunakan parameter ini, semua file yang dicantumkan akan dipindahkan sesuai dengan jenis ekstensinya pada *working directory*.
    * menggunakan parameter `-d` untuk menerima input berupa path dari satu folder. Contoh `./soal3 -d path/to/directory`. Apabila menggunakan parameter ini, semua file yang ada pada directory akan otomatis dipindahkan dan dikategorikan secara rekursif sesuai dengan ekstensi masing-masing file.
    * menggunakan parameter tambahan `\*` untuk memindahkan dan mengkategorikan seluruh file yang ada pada *working directory*. Contoh `./soal3 \*`.
* Apabila terdapat dua ekstensi atau lebih pada satu file, maka file akan dipindahkan ke ekstensi paling depan. Contoh apabila file memiliki ekstensi `.tar.gz` maka akan dipindahkan ke folder dengan ekstensi `tar.gz`.
* Untuk file yang hidden (tidak memiliki nama file atau nama file didahului dengan titik '.') akan dipindahkan ke folder "hidden".
* Untuk file yang tidak memiliki ekstensi akan dipindahkan ke folder "unknown".
* Semua proses pindah memindah dilakukan dalam thread tersendiri agar proses berjalan cepat

Untuk menyelesaikan kriteria tersebut, maka program utama pada soal 3 ini sebagai berikut 
```c
int main(int argc, char const *argv[])
{
    getcwd(curPath, sizeof(curPath));
    if(strcmp(argv[1], "-f") == 0) {
        ...
    } else if(strcmp(argv[1], "-d") == 0) {
        ...
    } else if(strcmp(argv[1], "\*") == 0) {
        ...
    }
    return 0;
}
```
Pada program utama soal, program akan membaca argumen dari program yang dijalankan, kemudian membaginya sesuai dengan kriteria di atas.

Kemudian program akan terbagi menjadi beberapa bagian :

### A
Untuk menjawab bagian ini, perlu sebuah fungsi untuk mengecek apakah path yang di cek merupakan file atau folder. Untuk mengeceknya maka akan menggunakan fungsi `checkFile(const char *path)` sebagai berikut 
```c
int checkFile(const char *path) {
    struct stat stat_path;
    stat(path, &stat_path);
    return S_ISREG(stat_path.st_mode);
}
```
Fungsi tersebut akan mengetahui apakah file tersebut valid dengan menggunakan fungsi `S_ISREG()` apabila file tersebut tidak valid akan mengembalikan nilai `0` atau `false`.

Kemudian untuk mengkategorikan sebuah file, program ini juga akan menggunakan fungsi `filterFile()` sebagai berikut.
```c
void *filterFile(void *filePath) {
    char *ptr_ext;
    char ext[100];
    char currPath[1000];
    char oldFile[500];

    ptr_ext = strchr(filePath, '.');

    ext[0] = '\0';
    if(ptr_ext) {
        ptr_ext++;
        for(int i=0; i<strlen(ptr_ext); i++) {
            ext[i] = tolower(ptr_ext[i]);
        }
        ext[strlen(ptr_ext)] = '\0';
    }

    strcpy(oldFile, filePath);
    strcpy(currPath, curPath);
    char *fileName = strrchr(filePath, '/');

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
```
Fungsi ini akan menerima parameter berupa pointer dari nama path yang diinputkan. Kemudian untuk mendapatkan ekstensi dari file dari yang paling depan, maka program menggunakan fungsi `strchr()` untuk mengembalikan pointer dimana posisi character '.' pertama kali ditemukan.
Kemudian untuk mengubahnya ke dalam lowercase, dapat menggunakan fungsi `tolower` dari library `string.h`.

Selain menentukan ekstensi, di sini kita juga harus menentukan apakah file tersebut termasuk file hidden atau tidak, dengan menggunakan fungsi `strrchr()` untuk mengembalikan pointer dimana posisi character '/' terakhir ditemukan dari path. Apabila hasil dari pointer tersebut diawali dengan character '.', maka otomatis file tersebut merupakan file hidden.

Kemudian, program akan membuat direktori baru dengan menggunakan fungsi `mkdir()` untuk membuat folder.

File yang memiliki awalan "." akan dipindahkan ke folder "hidden", sedangkan file tanpa ekstensi akan dipindahkan ke folder "unknown", dan sisanya akan dipindahkan sesuai dengan ekstensi masing-masing.

Kemudian pada program utama akan melakukan looping dari argumen ke-3 hingga argumen terakhir untuk mendapatkan path file yang akan dipindahkan. 
```c
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
        char filePath[argc-2][2000];
        for(int i=2; i<argc; i++) {
            strcpy(filePath[count], argv[i]);

            if(checkFile(filePath[count])) {
                pthread_create(&tid[count], NULL, filterFile, (void *)filePath[count]);
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
    ...
    } else if(strcmp(argv[1], "\*") == 0) {
    ...
    }
    return 0;
}
```

Lalu pada masing-amsing path file akan di cek satu per satu menggunakan fungsi `checkFile()`. Apabila tidak valid, program akan mencetak `File [n]: Sad, gagal :(`. Apabila valid, maka kemudian program akan membuat thread baru untuk menjalankan fungsi `filterFIle()` untuk mengkategorikan file.

Apabila telah berhasil membuat thread, maka program akan mencetak `File [n]: Berhasil Dikategorikan`.

Kemudian program akan melakukan looping pada seluruh thread untuk dilakukan join menggunakan fungsi `pthread_join()` agar tidak terjadi error pada thread selanjutnya.

Berikut dokumentasi dari bagian A

![Program no 3a ketika dijalankan](docs/soal3a.png)

### B
Untuk menjawab bagian ini, perlu fungsi untuk melakukan rekursi dari path sebuah folder, yaitu menggunakan fungsi `filterDir()` sebagai berikut 
```c
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
``` 
Fungsi tersebut akan menerima 2 paramter, yakni path folder yang akan direkursi dan banyak thread yang akan dibuat. Kemudian fungsi ini akan mengecek terlebih dahulu apakah folder atau file tersebut valid atau tidak. Apabila valid, kemudian fungsi akan mengecek apakah path tersebut berbentuk folder atau file menggunakan atribut `d_type` dari path tersebut. 

Apabila berupa `DT_REG` berarti merupakan sebuah file, maka fungsi akan membuat thread baru untuk mengkategorikan file tersebut dengan memanggil fungsi `filterFile()`. Sekaligus sebelumnya, dilakukan pengecekan apakah file yang dibaca merupakan file kode program c nya sendiri atau bukan, agar file kode program tidak ikut terpindahkan.

Kemudian, apabila tipe program berupa `DT_DIR`, berarti path tersebut merupakan sebuah direktori. Kemudian dicek lagi, apakah path tersebut bukan merupakan direktori '.' (direktori itu sendiri) dan juga bukan direktori '..' (direktori *parent*). Apabila telah lolos, maka baru dilakukan looping kembali pada direktori tersebut untuk menentukan ukuran thread pada child folder. Lalu panggil fungsi `filterDir()` lagi untuk melakukan rekursi pada folder childnya. 

Terakhir, lakukan join pada semua thread yang telah dibuat agar tidak terjadi error lagi.

Kemudian, pada fungsi utama program akan mengecek terlebih dahulu jumlah argumen yang diinputkan, apabila tidak sesuai akan mencetak "Argumen tidak sesuai". Apabila lolos, path yang diinput akan dicek terlebih dahulu apakah merupakan folder atau file yang valid atau bukan. 

Apabila bukan merupakan folder yang valid, maka akan mencetak "Yah, gagal disimpan :(". Namun apabila merupakan folder yang valid, maka program akan menghitung jumlah thread yang akan dibuat berdasarkan dengan jumlah child dari folder tersebut. Kemudian akan memanggil fungsi `filterDir()` untuk mengkategorikan direktori.
```c
int main(int argc, char const *argv[])
{
    getcwd(curPath, sizeof(curPath));
    if(strcmp(argv[1], "-f") == 0) {
        ...
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
        ...
    }
    return 0;
}
```

Berikut dokumentasi dari bagian B

![Program no 3b ketika dijalankan](docs/soal3b.png)

### C
Untuk menyelesaikan bagian ini, hampir sama dengan bagian B. Hanya saja, parameter yang diterima yaitu character `\*` saja. 
```c
int main(int argc, char const *argv[])
{
    getcwd(curPath, sizeof(curPath));
    if(strcmp(argv[1], "-f") == 0) {
        ...
    } else if(strcmp(argv[1], "-d") == 0) {
        ...
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
```
Perbedaannya dengan bagian B, pada fungsi `filterDir()`, parameter yang digunakan adalah working directory yang telah didapatkan di awal program utama.

Berikut dokumentasi dari bagian C

![Program no 3c ketika dijalankan](docs/soal3c.png)

## Kesulitan
Kesulitan dalam pengerjaan praktikum ini adalah untuk mengecek apakah outputan dari program telah sesuai atau tidak harus mengecek satu per satu, dan hal itu memakan waktu yang cukup banyak
