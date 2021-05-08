#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <malloc.h>
#include <sys/stat.h>
#include <limits.h>

time_t my_time;
struct tm * timeinfo;

#define PORT 8080
#define SIZE 1024

char cwd[PATH_MAX];
char sess_userpass[SIZE];

char* convertToCharPtr(char *str){
	int len=strlen(str);
	char* ret = malloc((len+1) * sizeof(char));
	for(int i=0; i<len; i++){
		ret[i] = str[i];
	}
	ret[len] = '\0';
	return ret;
}

char* getTimeNow(){
	time (&my_time);
	timeinfo = localtime (&my_time);

	char day[10], month[10], year[10], hour[10], minute[10], second[10];

	sprintf(day, "%d", timeinfo->tm_mday);
	if(timeinfo->tm_mday < 10) sprintf(day, "0%d", timeinfo->tm_mday);

	sprintf(month, "%d", timeinfo->tm_mon+1);
	if(timeinfo->tm_mon+1 < 10) sprintf(month, "0%d", timeinfo->tm_mon+1);

	sprintf(year, "%d", timeinfo->tm_year+1900);

	sprintf(hour, "%d", timeinfo->tm_hour);
	if(timeinfo->tm_hour < 10) sprintf(hour, "0%d", timeinfo->tm_hour);

	sprintf(minute, "%d", timeinfo->tm_min);
	if(timeinfo->tm_min < 10) sprintf(minute, "0%d", timeinfo->tm_min);

	sprintf(second, "%d", timeinfo->tm_sec);
	if(timeinfo->tm_sec < 10) sprintf(second, "0%d", timeinfo->tm_sec);

	char datetime_now[100];
	sprintf(datetime_now, "%s-%s-%s_%s:%s:%s", year, month, day, hour, minute, second);
	char* ret=convertToCharPtr(datetime_now);
	return ret;
}

char *strrev(char *str){
	  char *p1, *p2;

	  if (! str || ! *str)
			return str;
	  for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
	  {
			*p1 ^= *p2;
			*p2 ^= *p1;
			*p1 ^= *p2;
	  }
	  return str;
}

char log_msg[1024];
char response[1024];

void logs(){
	time (&my_time);
	timeinfo = localtime(&my_time);

	char *timeNow = getTimeNow();
	printf("%s : %s\n", timeNow, log_msg);
}

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

bool usernameExist(char *username){
	FILE* file = fopen("akun.txt", "r");
	char line[2050];

	while (fgets(line, sizeof(line), file)) {
		char *end_str;
		char *user = strtok_r(line, ":", &end_str);

		if(!strcmp(username,user)){
			return true;
		}
	}

	fclose(file);
	return false;
}

void storeNewUser(char *username, char *password){
	FILE* fptr = fopen("akun.txt", "a");
	fprintf(fptr, "%s:%s\n", username, password);
	fclose(fptr);
}

void reply(int client_fd, char *message){
	send(client_fd, message, strlen(message), 0);
}

char buffer[SIZE] = {0};

char* readClient(int client_fd){
	bzero(buffer, SIZE);
	read(client_fd, buffer, SIZE);
	buffer[strcspn(buffer, "\n")] = 0;
	return convertToCharPtr(buffer);
}

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

int fileExist(char *filepath){
	printf("fileExist called\n");
	FILE* file = fopen("files.tsv", "r");
	char line[2050];

	int rownum=0;
	while (fgets(line, sizeof(line), file)) {
		printf("rownum : %d\n", rownum);
		if(rownum!=0){
			char *end_str;
			char *path = strtok_r(line, "\t", &end_str); // get publisher
			printf("path : %s\n", path);
			path = strtok_r(NULL, "\t", &end_str); // get tahun publish
			path = strtok_r(NULL, "\t", &end_str); // get path
			path[strcspn(path, "\n")] = 0;

			if(!strcmp(filepath,path)){
				return rownum;
			}
		}
		rownum++;
	}

	fclose(file);
	return -1;
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

void deleteFile(int rownum, char* filename){
	char oldfilename[SIZE];
	char newfilename[SIZE];
	
	sprintf(oldfilename, "%s/FILES/%s", cwd, filename);
	sprintf(newfilename, "%s/FILES/old-%s", cwd, filename);
	rename(oldfilename, newfilename);

	// then delete in DB
	updateDBAtRow(rownum, "", true);
}

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

int main(int argc, char const *argv[]) {
	getcwd(cwd, sizeof(cwd));

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

		getpeername(server_fd, (struct sockaddr*)&server, (socklen_t*)&addrlen);
	
		sprintf(log_msg, "[!] New client connected [%d] >> %s:%d", fd_in, inet_ntoa(server.sin_addr) , ntohs(server.sin_port));
		logs();

		reply(fd_in, "Selamat datang di server!\r\n[>] Silahkan pilih:\r\n\t[1] register\r\n\t[2] login\r\n[>] Pilihan anda : ");

		char *option = readClient(fd_in);

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

		int n;
		while(true){
			readClient(fd_in);
			sprintf(log_msg, "[%d] send : %s", fd_in, buffer);
			logs();

			char *end_str;
			char *command = strtok_r(buffer, " ", &end_str);
			char *arg1 = strtok_r(NULL, " ", &end_str);

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
			}else if(!strcmp(command, "download")){ // D :: download file
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
			}else if(!strcmp(command, "delete")){ // E :: delete file
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
			}else if(!strcmp(command, "see")){
				see(fd_in);
			}else if(!strcmp(command, "find")){
				find(fd_in, arg1);
			}else if(!strcmp(command, "exit")){
				reply(fd_in, "Berhasil keluar! ~bye\r\n");
				break;
			}else{
				reply(fd_in, "Command not found\r\n[<] ");
			}
		}

		getpeername(server_fd, (struct sockaddr*)&server, (socklen_t*)&addrlen);
		sprintf(log_msg, "Client disconnected [%d] >> %s:%d", fd_in, inet_ntoa(server.sin_addr) , ntohs(server.sin_port));
		logs();

		close(fd_in);
	}
	return 0;
}
