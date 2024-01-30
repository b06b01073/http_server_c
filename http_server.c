#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>


int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Please provide the port number. You should run this process by ./http_server.o <port number>");
        return 1;
    }


    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        printf("Failed to create socket.");
        return 1;
    }

    struct sockaddr_in server_sa;
    bzero(&server_sa, sizeof(server_sa));

    unsigned short port_number = atoi(argv[1]); 
    server_sa.sin_family = PF_INET;
    server_sa.sin_addr.s_addr = INADDR_ANY;
    server_sa.sin_port = htons(port_number);


    if(bind(sockfd, (struct sockaddr*)&server_sa, sizeof(server_sa)) == -1) {
        printf("Failed to bind.");
        return 1;
    }


    if(listen(sockfd, 5) == -1) {
        printf("Failed to listen.");
        return 1;
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

        send(client_fd, hello, sizeof(hello), 0);
    }
}