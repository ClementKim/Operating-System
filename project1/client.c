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
    int byte;

    char file_name[SIZE] = {0};
    char file_mode;

    if ((named_pipe = open("./namedpipe", O_WRONLY)) == -1){
        printf("Failed to open named pipe\n");
        exit(1);
    }

    while (TRUE){
        printf("Enter the file name: ");
        if (fgets(file_name, SIZE, stdin) != NULL);
        else    continue;

        printf("Enter the file access type (r or w): ");
        scanf("%c", &file_mode);
        while(getchar() != '\n');

        if (strcmp(file_mode, 'r')){
            printf("Enter the number of bytes to read: ");
            scanf("%d", &byte);
            while (getchar() != '\n');
        }

        else if (strcmp(file_mode, 'w')){
            printf("Enter the number of bytes to write: ");
            scanf("%d", &bytes);
            while (getchar() != '\n');
        }

        else{
            printf("You typed wrong mode\n");

            // initialization
            memset(file_name, 0, SIZE * sizeof(char));
            file_mode = {0};
            continue;
        }
    }
}           





    }

    return 0;
}
