#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFF_SIZE 1024

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void pipe_fd(int fd_1, int fd_2)
{
    char buff[BUFF_SIZE];
    int n, read_from, write_to;
    int r_to_g = fork();
    if (r_to_g == -1) {
        error("ERROR tunnel fork");
        return;
    } else if (r_to_g) {
        read_from = fd_1;
        write_to = fd_2;
    } else {
        read_from = fd_2;
        write_to = fd_1;
    }
    while ((n = read(read_from, &buff, BUFF_SIZE))) {
        printf("%s", buff);
        write(write_to, &buff, n);
    }
}
