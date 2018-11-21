#include <stdio.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "../include/utils.h"

#define LOCAL_HOST "127.0.0.1"

int connect_socket(char *host, int portno)
{
    struct sockaddr_in serv_addr;
    int sockfd;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    memset(&serv_addr, 0, sizeof serv_addr);
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    return sockfd;
}

int main(int argc, char *argv[])
{
    int sock_fd_remote, sock_fd_local;

    if (argc < 4) {
       fprintf(stderr,"usage %s hostname port local_port\n", argv[0]);
       exit(0);
    }

    signal(SIGCHLD, SIG_IGN); 
    
    sock_fd_remote = connect_socket(argv[1], atoi(argv[2]));

    do {
        sock_fd_local = connect_socket(LOCAL_HOST, atoi(argv[3]));
    } while (!pipe_fd(sock_fd_remote, sock_fd_local) && !close(sock_fd_local));

    return 0;
}
