#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#define SIZE 1024
#define TRUE 1
#define FALSE 0

int main(void){
    int named_pipe;

    char buff[SIZE] = {0};

    if (mkfifo("./namedpipe") == -1){
        printf("Filed to call named pipe\n");
        exit(1);
    }

    if ((named_pipe = open("./namedpipe", O_RDWR)) < 0){
        printf("Failed to open named pipe\n");
        unlink("./namedpipe");
        exit(1);
    }

    while (TRUE){
    }


    return 0;
}
