#include <stdio.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 1024

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void tunneller(int fd_tunnel_requester, int fd_tunnel_given)
{
    char buff[BUFF_SIZE];
    int r_to_g = fork();
    if (r_to_g == -1) {
        error("ERROR tunnel fork");
    } else if (r_to_g) {
        // TODO pipe these
    }
}

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

void piper(int IGNORE, int tunnel_requester)
{
    create_server(0, tunnel_requester, &tunneller);
}

void new_connection_handler(int newsockfd)
{

    int n;
    char buffer[256];
    bzero(buffer, 256);
    n = read(newsockfd, buffer, 255);
    if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n", buffer);
    n = write(newsockfd, "I got your message", 18);
    if (n < 0) error("ERROR writing to socket");
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    return create_server(atoi(argv[1]), 0, &piper);
}
