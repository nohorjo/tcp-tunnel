#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFF_SIZE 1024

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int pipe_fd(int fd_1, int fd_2)
{
    char buff[BUFF_SIZE + 1];
    int n, nn, read_from, write_to;
    int first_to_second = fork();
    char direction[5];
    if (first_to_second == -1) {
        error("ERROR tunnel fork");
    } else {
        if (first_to_second) {
            read_from = fd_1;
            write_to = fd_2;
            strcpy(direction, "F->S");
        } else {
            read_from = fd_2;
            write_to = fd_1;
            strcpy(direction, "S->F");
        }
        while ((nn = (n = read(read_from, &buff, BUFF_SIZE)))) {
            if (first_to_second) {
                printf(RED "%s" RESET, buff);
            } else {
                printf(GREEN "%s" RESET, buff);
            }
            for (int i = n - 1; i; i--) {
                if (buff[i] == 0x04) {
                    --n;
                }
            }
            write(write_to, &buff, n);
            if (n != nn) goto OUTER;
            memset(buff, 0, sizeof buff);
        }
        OUTER:
        write(write_to, "\x04", 1);
        printf("PIPE END %s\n", direction);
    }
    return first_to_second;
}
