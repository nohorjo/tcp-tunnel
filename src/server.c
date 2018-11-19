#include <stdio.h>
#include <sys/wait.h>
#include <netinet/in.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/utils.h"

int create_server(int portno, int fd, void (*handler)(int, int))
{
    int sockfd, newsockfd, clilen;
    struct sockaddr_in server_socket, client_socket;
    int fork_pid;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");
    bzero((char *) &server_socket, sizeof(server_socket));
    server_socket.sin_family = AF_INET;
    server_socket.sin_addr.s_addr = INADDR_ANY;
    server_socket.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &server_socket,
             sizeof(server_socket)) < 0) 
             error("ERROR on binding");
    listen(sockfd, 128);
    socklen_t len = sizeof(server_socket);
    if (getsockname(sockfd, (struct sockaddr *)&server_socket, &len) == -1)
        perror("getsockname");
    else
        printf("port %d\n", ntohs(server_socket.sin_port));
    signal(SIGCHLD, SIG_IGN); 
    while (1) {
        clilen = sizeof(client_socket);
        newsockfd = accept(sockfd, (struct sockaddr *) &client_socket, &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");
        fork_pid = fork();
        if (fork_pid == -1) {
            error("Fork error");
        } else if (fork_pid) {
            handler(fd, newsockfd);
        }
    }
    return 0; 
}

// FIXME
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFF_SIZE 1024
void pipe_fd(int fd_1, int fd_2)
{
    char buff[BUFF_SIZE];
    int n, read_from, write_to;
    int r_to_g = fork();
    if (r_to_g == -1) {
        error("ERROR tunnel fork");
    } else if (r_to_g) {
        read_from = fd_1;
        write_to = fd_2;
    } else {
        read_from = fd_2;
        write_to = fd_1;
    }
    while (n = read(read_from, &buff, BUFF_SIZE)) {
        write(write_to, &buff, n);
    }
}

void piper(int IGNORE, int tunnel_requester)
{
    create_server(0, tunnel_requester, &pipe_fd);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    return create_server(atoi(argv[1]), 0, &piper);
}
