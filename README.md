# Kelompok C04 Modul 3 Sistem Operasi C
- I Kadek Agus Ariesta Putra 05111940000105
- Muhammad Arif Faizin 05111940000060
- Ahmad Lamaul Farid 05111940000134

# Soal
Link soal [Soal Shift 3](https://docs.google.com/document/d/1ud1JyncoSDAo5hA03wPvjn7QBHPUeUG1eBJ8ETtq2dQ/edit)

## Soal 1
Pada soal nomor 1, kita diminta untuk membuat aplikasi server-client menggunakan socket. Dengan ketentuan sebagai berikut:

### A
Client yang dilayani hanya satu client, apabila lebih dari satu client maka client lainya akan mengantri. Kemudian terdapat opsi login atau register yang databasenya berada pada akun.txt

Pertama-tama sistem menerima dan mengirim pesan dari server-client yang kami gunakan adalah client hanya mampu mengirim pesan apabila sudah ada respon dari client.Untuk kasus normal server hanya mampu mengirim respon satu kali, selain itu terdapat kasus khusus untuk menerima respon lebih dari satu kali. 

Untuk pengaturan socket pada server adalah sebagai berikut:
```c
	int server_fd;
	struct sockaddr_in server;
	char *msg_in;
	
	// buat fd
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("Server Error: Socket not created succesfully");
		exit(EXIT_FAILURE);
	}
	
	// Initialize
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);


	// Bind
	bind(server_fd, (struct sockaddr *)&server, sizeof(server));

	// Set listent
	listen(server_fd, 3);

	int fd_in;

	int addrlen = sizeof(server);
	getpeername(server_fd, (struct sockaddr*)&server, (socklen_t*)&addrlen);  
	printf("Host created: ip %s:%d \n",inet_ntoa(server.sin_addr) , ntohs(server.sin_port));

	// B :: Create "FILES" directory
	mkdir("FILES", 0777);

	while(true){
		fd_in = accept(server_fd, (struct sockaddr*) &server, (socklen_t*)&addrlen);

        ...
    }
```

Untuk pengaturan socket pada client adalah sebagai berikut:
```c
int server_fd;
    bzero(buffer, SIZE);
    
    // set socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed\n");
        exit(EXIT_FAILURE);
    }
    
    int opt = 1;
    // setup socket option
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Initialize
    struct sockaddr_in server;
    memset(&server, '0', sizeof(server)); 

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    int message_in = inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
    if(message_in < 0){
        perror("Client Error: IP not initialized successfully");
        return 0;
    }

    // Connect to given server
    message_in = connect(server_fd, (struct sockaddr*) &server, sizeof(server));
```

Kemudian fungsi register adalah sebagai berikut:
```c
// A :: Register
if(!strcmp(option, "1")){
    // register
    char username[1024];
    char password[1024];

    while(true){
        strcpy(response, "Signup >> username: ");
        send(fd_in, response, strlen(response), 0);

        char *temp = readClient(fd_in);
        strcpy(username, temp);

        if(usernameExist(username)){
            strcpy(response, "Username exist!\r\n");
            send(fd_in, response, strlen(response), 0);
        }else{
            break;
        }
    }

    reply(fd_in, "Signup >> password: ");
    char *temp = readClient(fd_in);
    strcpy(password, temp);
    
    // save password
    storeNewUser(username, password);
    reply(fd_in, "Berhasil Signup!, sekarang login!\r\n");
}

...
void storeNewUser(char *username, char *password){
	FILE* fptr = fopen("akun.txt", "a");
	fprintf(fptr, "%s:%s\n", username, password);
	fclose(fptr);
}
```

Kemudian fungsi login adalah sebagai berikut:
```c
// A :: Login
while(true){
    char *username;
    char *password;

    reply(fd_in, "Login username: ");
    username = readClient(fd_in);

    reply(fd_in, "Login password: ");
    password = readClient(fd_in);
    
    if(auth(username, password)){
        reply(fd_in, "Berhasil login!\r\n[<] ");
        break;
    }else{
        reply(fd_in, "Password salah!\r\n");
    }
}

...
bool auth(char *username, char *password){
	// printf("Input : %s|%s\n", username, password);
	FILE* file = fopen("akun.txt", "r");
	char line[2050];
	char userpass[2050]; sprintf(userpass, "%s:%s\n", username, password);
	char userpass_noline[2050]; sprintf(userpass_noline, "%s:%s", username, password);
	
	// printf("Userpass : %s\n", userpass);

	while (fgets(line, sizeof(line), file)) {
		// printf("%s", line);
		if(!strcmp(userpass,line)){
			strcpy(sess_userpass, userpass_noline);
			return true;
		}
	}

	fclose(file);
	return false;
}
```

### B
Sistem memiliki sebuah database yang bernama files.tsv. Isi dari files.tsv ini adalah path file saat berada di server, publisher, dan tahun publikasi. Setiap penambahan dan penghapusan file pada folder file yang bernama  FILES pada server akan memengaruhi isi dari files.tsv. Folder FILES otomatis dibuat saat server dijalankan.

Jadi setiap server dijalankan server membuat folder FILES
```c
	// B :: Create "FILES" directory
	mkdir("FILES", 0777);
```

### C
Terdapat fungsi untuk menambahkan files dari client ke server

Berikut merupakan potongan kode program untuk menangani fungsi tambah file dari sisi server.
```c
// C :: Fungsi Nambah File
if(!strcmp(command, "add")){
    char *publisher;
    char *tahun_publikasi;
    char *file_path;

    reply(fd_in, "Publisher: ");
    publisher = readClient(fd_in);
    sprintf(log_msg, "[%d] send : %s", fd_in, publisher);
    logs();

    reply(fd_in, "Tahun Publikasi: ");
    tahun_publikasi = readClient(fd_in);
    sprintf(log_msg, "[%d] send : %s", fd_in, tahun_publikasi);
    logs();

    reply(fd_in, "File Path: ");
    file_path = readClient(fd_in);
    sprintf(log_msg, "[%d] send : %s", fd_in, file_path);
    logs();
    
    char *end_str;
    char *filename = strtok_r(strrev(file_path), "/", &end_str);

    char full_path[PATH_MAX];
    sprintf(full_path, "%s/FILES/%s", cwd, strrev(filename));
    write_file(fd_in, full_path);

    storeDBFiles(publisher, tahun_publikasi, full_path);

    // Lalu logs ke running.log
    FILE* fptr_logs = fopen("running.log", "a");
    fprintf(fptr_logs, "Tambah : %s (%s)\r\n", filename, sess_userpass);
    fclose(fptr_logs);
}

...

void write_file(int sockfd, char *file_path){
	readClient(sockfd);
	long int file_size = atoi(buffer);
	bzero(buffer, SIZE);

	int n;
	remove(file_path);
	FILE *fp = fopen(file_path, "a");
	long int bytes_recevied = 0;
	while (1){
		n = recv(sockfd, buffer, SIZE, 0);
		if (n <= 0){ printf("File transfer failed!"); fflush(stdout); break; return; }

		bytes_recevied += strlen(buffer);
		printf("Menerima file %ld/%ld\n", bytes_recevied, file_size); fflush(stdout);
		fprintf(fp, "%s", buffer);
		bzero(buffer, SIZE);

		if(bytes_recevied == file_size){
			printf("File berhasil diterima\n"); fflush(stdout);
			break;
		}
	}

	fclose(fp);
	reply(sockfd, "File berhasil diterima!\r\n[<] ");
	return;
}

void storeDBFiles(char *publisher, char *tahun, char *filepath){
	int cek = fileExist(filepath);
	printf("cek : %d\n", cek);
	char new_row[2050]; sprintf(new_row, "%s\t%s\t%s\n", publisher, tahun, filepath);
	if(cek > 0){
		updateDBAtRow(cek, new_row, false);
	}else{
		FILE* file = fopen("files.tsv", "a");
		fprintf(file, "%s", new_row);
		fclose(file);
	}
}
```

Berikut merupakan dari sisi client
```c
if(!strcmp(buffer, "File Path: ")){
    printf("%s/", cwd);
    fflush(stdout);

    fgets(buffer, 255, stdin);                    
    bzero(t_filepath, SIZE); sprintf(t_filepath, "%s/%s", cwd, buffer);
    t_filepath[strcspn(t_filepath, "\n")] = 0;

    send(server_fd, t_filepath, strlen(t_filepath), 0);
    send_file(server_fd, t_filepath);

    bzero(buffer,256);
    continue;
}

...

void send_file(int sockfd, char* filename){
    // printf("send_file called\n"); fflush(stdout);
    long int file_size = getFileSize(filename);

    char size_msg[SIZE]; sprintf(size_msg, "%ld", file_size);
    send(sockfd, size_msg, strlen(size_msg), 0);

    FILE *fp = fopen(filename, "r");
    char line[SIZE];

    long int bytes_sent = 0;
    while (fgets(line, sizeof(line), fp)) {
        bytes_sent += strlen(line);
        printf("[!] Sending %ld/%ld bytes\n", bytes_sent, file_size);
        send(sockfd, line, strlen(line), 0);
        bzero(line, SIZE);
    }

    // printf("Send file selesai\n");
}

long int getFileSize(char file_name[]){
    FILE* fp = fopen(file_name, "r");  
    fseek(fp, 0L, SEEK_END);
    long int res = ftell(fp);
    fclose(fp);  
    return res;
}
```

Jadi mekanisme pengiriman file yang kami gunakan adalah pertama-tama client mengirim informasi berapa banyak bytes yang akan dikirim ke server. Lalu server akan menerima informasi tersebut untuk dilakukan validasi apakah nantinya file yang terkirim berhasil atau tidak tergantung kesesuaian data yang dikirim. Karena pada soal yang dikirim hanyalah file teks maka kami mengirim isi filenya saja.

### D

Kemudian server juga mampu mengirimkan file ke client.

Berikut merupakan potongan kode dari sisi server:
```c
else if(!strcmp(command, "download")){ // D :: download file
    if(arg1 != NULL){
        char full_path[PATH_MAX];
        sprintf(full_path, "%s/FILES/%s", cwd, arg1);
        if(fileExist(full_path)>0){
            // go proses kirim
            sprintf(response, "Downloading files >%s", arg1);
            reply(fd_in, response);
            send_file(fd_in, full_path);
            sprintf(log_msg, "pengiriman file selesai");
            logs();
            reply(fd_in, "Pengiriman file selesai\r\n[<] ");
        }else{
            // file tidak ada
            reply(fd_in, "File gak ada!\r\n[<] ");
        }
    }else{
        reply(fd_in, "File gak boleh kosong!\r\n[<] ");
    }
}

...
long int getFileSize(char file_name[]){
    FILE* fp = fopen(file_name, "r");  
    fseek(fp, 0L, SEEK_END);
    long int res = ftell(fp);
    fclose(fp);  
    return res;
}

void send_file(int sockfd, char* filename){
    long int file_size = getFileSize(filename);

    char size_msg[SIZE]; sprintf(size_msg, "%ld", file_size);
    send(sockfd, size_msg, strlen(size_msg), 0);

    FILE *fp = fopen(filename, "r");
    char line[SIZE];

    long int bytes_sent = 0;
    while (fgets(line, sizeof(line), fp)) {
        bytes_sent += strlen(line);
        printf("[!] Sending %ld/%ld bytes\n", bytes_sent, file_size);
        send(sockfd, line, strlen(line), 0);
        bzero(line, SIZE);
    }
	// reply(sockfd, "File berhasil dikirim!\r\n[<] ");
}
```

Berikut merupakan dari sisi client:

```c
else if(strstr(buffer, "Downloading files >") != NULL){
    //
    printf("\n"); fflush(stdout);
    char d_filename[SIZE];
    char *end_str;
    char *filename = strtok_r(buffer, ">", &end_str);
    filename = strtok_r(NULL, ">", &end_str);
    bzero(t_filepath, SIZE); sprintf(t_filepath, "%s/%s", cwd, filename);
    t_filepath[strcspn(t_filepath, "\n")] = 0;

    write_file(server_fd, t_filepath);
    continue;
}

...
void write_file(int sockfd, char *file_path){
    printf("writing file to : %s\n", file_path);
	readServer(sockfd);
	long int file_size = atoi(buffer);
	bzero(buffer, SIZE);

	int n;
	remove(file_path);
	FILE *fp = fopen(file_path, "a");
	long int bytes_recevied = 0;
	while (1){
		n = recv(sockfd, buffer, SIZE, 0);
		if (n <= 0){ printf("File transfer failed!"); fflush(stdout); break; return; }

        // printf("buffer : [BEGIN]%s[END]\n", buffer);
		bytes_recevied += strlen(buffer);
		printf("Menerima file %ld/%ld\n", bytes_recevied, file_size); fflush(stdout);
		fprintf(fp, "%s", buffer);
		bzero(buffer, SIZE);

		if(bytes_recevied == file_size){
			printf("File berhasil diterima\r\n");
            fflush(stdout);
			break;
		}
	}

	fclose(fp);
	return;
}
```

Mekanisme pengirimannya sama saja hanya saja berbalik dari sisi server mengirim data ke client.

### E
Client juga mampu menghapus data file dari server dengan perintah `delete namafile.ekstensi`

Berikut merupakan kode program dari sisi server:
```c
else if(!strcmp(command, "delete")){ // E :: delete file
    if(arg1 != NULL){
        char full_path[PATH_MAX];
        sprintf(full_path, "%s/FILES/%s", cwd, arg1);
        int dbrow = fileExist(full_path);
        if(dbrow>0){
            // go proses delete
            deleteFile(dbrow, arg1);
            reply(fd_in, "File berhasil dihapus!\r\n[<] ");

            // Lalu logs ke running.log
            FILE* fptr_logs = fopen("running.log", "a");
            fprintf(fptr_logs, "Hapus : %s (%s)\r\n", arg1, sess_userpass);
            fclose(fptr_logs);
        }else{
            // file tidak ada
            reply(fd_in, "File gak ada!\r\n[<] ");
        }
    }else{
        reply(fd_in, "File gak boleh kosong!\r\n[<] ");
    }
}

...

void deleteFile(int rownum, char* filename){
	char oldfilename[SIZE];
	char newfilename[SIZE];
	
	sprintf(oldfilename, "%s/FILES/%s", cwd, filename);
	sprintf(newfilename, "%s/FILES/old-%s", cwd, filename);
	rename(oldfilename, newfilename);

	// then delete in DB
	updateDBAtRow(rownum, "", true);
}

void updateDBAtRow(int rowupdate, char *newrow, bool isdelete){
	FILE* file = fopen("files.tsv", "r");
	FILE* file_temp = fopen("files.tsv.copy", "a");
	char line[2050];

	int rownum=0;
	while (fgets(line, sizeof(line), file)) {
		if(rownum == rowupdate){
			if(!isdelete){
				fprintf(file_temp, "%s", newrow);
			}
		}else{
			fprintf(file_temp, "%s", line);
		}
		rownum++;
	}

	fclose(file);
	fclose(file_temp);

	remove("files.tsv");
	rename("files.tsv.copy", "files.tsv");
}
```

Mekanisme penghapusan file yang diminta hanyalah mengganti nama filenya dengan menambah `old-` di depan nama file. Kemudian terdapat juga fungsi untuk menghapus data file pada database files.tsv yaitu updateDBAtRow().

### F

Terdapat perintah `see` yang dikirim oleh client untuk melihat isi database files.tsv

Berikut merupakan potongan kode program dari sisi server:

```c
void see(int sockfd){
	FILE* file = fopen("files.tsv", "r");
	char line[2050];

	reply(sockfd, "::BEGIN::");
	int rownum=0;
	while (fgets(line, sizeof(line), file)) {
		if(rownum!=0){
			char out_nama[SIZE],
				out_publisher[SIZE],
				out_tahun[SIZE],
				out_ext[SIZE],
				out_path[SIZE];
			char *end_str;

			char *publisher = strtok_r(line, "\t", &end_str); // get publisher
			strcpy(out_publisher, publisher);
			char *tahun = strtok_r(NULL, "\t", &end_str); // get tahun publish
			strcpy(out_tahun, tahun);
			char *path = strtok_r(NULL, "\t", &end_str); // get path
			path[strcspn(path, "\n")] = 0;
			strcpy(out_path, path);

			char *end_str2;
			char *filename = strrev(strtok_r(strrev(path), "/", &end_str2));
			strcpy(out_nama, filename);

			char *end_str3;
			char *file_ext = strrev(strtok_r(strrev(path), ".", &end_str3));
			strcpy(out_ext, file_ext);

			char fileinfo[2048];
			sprintf(fileinfo, "Nama: %s\r\nPublisher: %s\r\nTahun publishing: %s\r\nEkstensi File : %s\r\nFilepath : %s\r\n\r\n", out_nama, out_publisher, out_tahun, out_ext, out_path);
			reply(sockfd, fileinfo);
		}
		rownum++;
	}
	reply(sockfd, "[<] ");

	fclose(file);
}
```

Secara sederhana hanya melakukan looping dari isi files.tsv lalu mengirimnya ke client.

### G
Aplikasi client juga dapat melakukan pencarian dengan memberikan suatu string. Hasilnya adalah semua nama file yang mengandung string tersebut. Format output seperti format output f

Berikut merupakan potongan kode program dari sisi server:
```c
void find(int sockfd, char *str){
	FILE* file = fopen("files.tsv", "r");
	char line[2050];

	reply(sockfd, "::BEGIN::");
	int rownum=0;
	while (fgets(line, sizeof(line), file)) {
		if(rownum!=0){
			char out_nama[SIZE],
				out_publisher[SIZE],
				out_tahun[SIZE],
				out_ext[SIZE],
				out_path[SIZE];
			char *end_str;

			char *publisher = strtok_r(line, "\t", &end_str); // get publisher
			strcpy(out_publisher, publisher);
			char *tahun = strtok_r(NULL, "\t", &end_str); // get tahun publish
			strcpy(out_tahun, tahun);
			char *path = strtok_r(NULL, "\t", &end_str); // get path
			path[strcspn(path, "\n")] = 0;
			strcpy(out_path, path);

			char *end_str2;
			char *filename = strrev(strtok_r(strrev(path), "/", &end_str2));
			strcpy(out_nama, filename);

			char *end_str3;
			char *file_ext = strrev(strtok_r(strrev(path), ".", &end_str3));
			strcpy(out_ext, file_ext);

			if(strstr(out_nama, str) != NULL){
				char fileinfo[2048];
				sprintf(fileinfo, "Nama: %s\r\nPublisher: %s\r\nTahun publishing: %s\r\nEkstensi File : %s\r\nFilepath : %s\r\n\r\n", out_nama, out_publisher, out_tahun, out_ext, out_path);
				reply(sockfd, fileinfo);
			}
		}
		rownum++;
	}
	reply(sockfd, "[<] ");

	fclose(file);
}
```

Sama saja dengan point F, namun hanya mengirim yang nama filenya mengandung string yang dicari.

### H
Dikarenakan Keverk waspada dengan pertambahan dan penghapusan file di server, maka Keverk membuat suatu log untuk server yang bernama running.log.

Jadi setiap penambahan file atau penghapusan file berisikan berikut:

Untuk tambah file:
```c
FILE* fptr_logs = fopen("running.log", "a");
fprintf(fptr_logs, "Tambah : %s (%s)\r\n", filename, sess_userpass);
fclose(fptr_logs);
```

Untuk hapus file:
```c
FILE* fptr_logs = fopen("running.log", "a");
fprintf(fptr_logs, "Hapus : %s (%s)\r\n", arg1, sess_userpass);
fclose(fptr_logs);
```

## Soal 2

pada soal nomor 2 ini kita diminta untuk membuat program dalam bahasa C dengan ketentuan sebagai berikut : 

### A

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
### B

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

### C

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
