#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <netinet/in.h>

#include "../include/utils.h"

void create_server(int portno, int fd, void (*handler)(int, int))
{
    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in server_socket, client_socket;
    int fork_pid;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");
    memset(&server_socket, 0, sizeof server_socket);
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
}

void piper(int target, int newsocket)
{
    if (!pipe_fd(target, newsocket, "target", "newsocket")) {
        close(newsocket);
    }
}

void init_pipe(int IGNORE, int tunnel_requester)
{
    create_server(0, tunnel_requester, &piper);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    signal(SIGCHLD, SIG_IGN); 

    create_server(atoi(argv[1]), 0, &init_pipe);
    return 0; 
}
