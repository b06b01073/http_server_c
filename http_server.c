#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
 #include <fcntl.h>

#define BUFFER_SIZE 4096
#define FIELD_SIZE 512

void process_request(int client_fd) {
 
    char buffer[BUFFER_SIZE + 1];
    int read_bytes = read(client_fd, buffer, BUFFER_SIZE);

    if(read_bytes == 1) {
        printf("Failed to read request from client.\n");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", buffer);


    char *request_ptr = strtok(buffer, " ");

    char method[FIELD_SIZE] = {0};
    strcpy(method, request_ptr);


    if(strcmp("GET", method)) {

        printf("Recieving a non GET method\n");
        exit(EXIT_FAILURE);
    }

    request_ptr = strtok(NULL, " ");
    char URL[FIELD_SIZE] = {0};
    strcpy(URL, request_ptr);

    if(*(URL + 1) == '\0') {
        strcpy(URL, "/index.html");
    }


    int file_fd = open((URL + 1), O_RDONLY); // ignore the '/' prefix in URL 

    char status[] = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";
    write(client_fd, status, strlen(status));

    int c;
    while ((c = read(file_fd, buffer, BUFFER_SIZE)) > 0) {
        write(client_fd, buffer, c);
    }
}


int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Please provide the port number. You should run this process by ./http_server.o <port number>");
        exit(EXIT_FAILURE);
    }


    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        printf("Failed to create socket.");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_sa;
    bzero(&server_sa, sizeof(server_sa));

    unsigned short port_number = atoi(argv[1]); 
    server_sa.sin_family = PF_INET;
    server_sa.sin_addr.s_addr = INADDR_ANY;
    server_sa.sin_port = htons(port_number);


    if(bind(sockfd, (struct sockaddr*)&server_sa, sizeof(server_sa)) == -1) {
        printf("Failed to bind.");
        exit(EXIT_FAILURE);
    }


    if(listen(sockfd, 5) == -1) {
        printf("Failed to listen.");
        exit(EXIT_FAILURE);
    }


    struct sockaddr client_sa;
    socklen_t client_sa_len = sizeof(client_sa);

    char hello[] = "hello";
    while(1) {
        int client_fd = accept(sockfd, &client_sa, &client_sa_len);
        if(client_fd == -1) {
            printf("Failed to accept.\n");
            continue;
        }


        pid_t pid = fork();
        if(pid < 0) {
            exit(EXIT_FAILURE);
        }
        else if(pid == 0) {
            close(sockfd);
            process_request(client_fd);
            exit(EXIT_SUCCESS); // child processed the request
        }
        else {
            close(client_fd);
        }
    }
}