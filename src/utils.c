#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFF_SIZE 1024

void error(char *msg)
{
    perror(msg);
    exit(1);
}

