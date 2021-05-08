#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <limits.h>
#include <stdbool.h>


#define PORT 8080
#define SIZE 1024

char cwd[PATH_MAX];
char buffer[SIZE];

char* convertToCharPtr(char *str){
	int len=strlen(str);
	char* ret = malloc((len+1) * sizeof(char));
	for(int i=0; i<len; i++){
		ret[i] = str[i];
	}
	ret[len] = '\0';
	return ret;
}

char* readServer(int server_fd){
	bzero(buffer, SIZE);
	read(server_fd, buffer, SIZE);
	buffer[strcspn(buffer, "\n")] = 0;
	return convertToCharPtr(buffer);
}

void reply(int server_fd, char *message){
	send(server_fd, message, strlen(message), 0);
}

long int getFileSize(char file_name[]){
    FILE* fp = fopen(file_name, "r");  
    fseek(fp, 0L, SEEK_END);
    long int res = ftell(fp);
    fclose(fp);  
    return res;
}

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

int main(int argc, char const *argv[]) {
    getcwd(cwd, sizeof(cwd));
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
    if(message_in < 0){
        perror("Client Error: Connection failed");
    }

    if(1 == 2){
        bzero(buffer,256);
        while(1){
            fgets(buffer, 255, stdin);
            send(server_fd, buffer, strlen(buffer), 0);
            bzero(buffer,256);
        }

        exit(0);
    }else{
        // untuk menerima pesan dari server
        bzero(buffer,256);
        int n;
        char t_filepath[SIZE];
        while(1){
            bzero(buffer,256);
            // Read message from server first!
            if((n = recv(server_fd, buffer, 256, 0)) > 0){
                if(!strcmp(buffer, "::BEGIN::")){
                    while(true){
                        bzero(buffer,256);
                        recv(server_fd, buffer, 256, 0);
                        // buffer[strcspn(buffer, "\n")] = 0;
                        printf("%s",buffer);
                        fflush(stdout);
                        
                        if(strstr(buffer, "[<]") != NULL){
                            break;
                        }
                    }
                }else{
                    printf("[>] %s",buffer);
                    fflush(stdout);
                }

                // printf("here::\r\n"); fflush(stdout);
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
                }else if(strstr(buffer, "Downloading files >") != NULL){
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

                char message[SIZE];
                fgets(message, 255, stdin);
                reply(server_fd, message);
                if(!strcmp(message, "exit\n")){ exit(EXIT_SUCCESS); }                
            }
        }
    }
    return 0;
}
