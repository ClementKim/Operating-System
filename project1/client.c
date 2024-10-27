#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#define SIZE 512
#define TRUE 1
#define FALSE 0

#define PIPE "./namedpipe"

int main(int argc, char* argv[]){
    int named_pipe;
    int byte;
    int file_length;

    char file_name[SIZE] = {0};
    char data_string[SIZE] = {0};
    char file_mode;
    char buff[SIZE] = {0};

    if ((named_pipe = open(PIPE, O_RDWR)) == -1){
        printf("Failed to open named pipe\n");
        exit(1);
    }

    while (TRUE){
        printf("Enter the file name: ");
        fgets(file_name, SIZE, stdin);
        file_name[strlen(file_name) - 1] = '\0';

        printf("Type the file access type (r or w): ");
        scanf("%c", &file_mode);
        while(getchar() != '\n');

        if (file_mode == 'r'){
            printf("Enter the number of bytes to read: ");
            scanf("%d", &byte);
            while (getchar() != '\n');

            if ((write(named_pipe, "request\n", SIZE)) < 0) {
                printf("failed to request to server \n");
                exit(1);
            }

            sleep(1);

            if ((write(named_pipe, &file_mode, sizeof(char))) < 0){
                printf("failed to send file mode \n");
                exit(1);
            }

            sleep(1);

            if ((write(named_pipe, &byte, sizeof(int))) < 0){
                printf("failed to send number of bytes to read \n");
                exit(1);
            }

            sleep(1);

            if ((write(named_pipe, file_name, SIZE)) < 0){
                printf("failed to send file name \n");
                exit(1);
            }

            sleep(2);

            if ((read(named_pipe, buff, SIZE)) < 0){
                printf("failed to receive data from the server \n");
                exit(1);
            }

            sleep(1);

            if (!(strcmp(buff, "fail\n"))){
                printf("There's no file named %s \n", file_name);
            }

            else {
                printf("%s\n", buff);
            }

            memset(file_name, 0, SIZE * sizeof(char));
            memset(&file_mode, 0, sizeof(char));
            memset(buff, 0, SIZE * sizeof(char));
        }

        else if (file_mode == 'w'){
            printf("Type the string to write: ");
            fgets(data_string, SIZE, stdin);
            data_string[strlen(data_string)-1] = '\0';

            if ((write(named_pipe, "request\n", SIZE)) < 0) {
                printf("failed to request to server \n");
                exit(1);
            }

            sleep(1);

            if ((write(named_pipe, &file_mode, sizeof(char))) < 0) {
                printf("Failed to send file mode to server \n");
                exit(1);
            }

            if ((write(named_pipe, file_name, SIZE)) < 0) {
                printf("failed to send file name\n");
                exit(1);
            }

            sleep(2);

            if ((write(named_pipe, data_string, SIZE)) < 0){
                printf("Failed to write data string \n");
                exit(1);
            }

            sleep(2);

            if (read(named_pipe, buff, SIZE) < 0){
                printf("failed to receive data from the server \n");
                exit(1);
            }

            sleep(1);

            if (!(strcmp(buff, "fail\n"))){
                printf("There's no file named %s \n", file_name);
            }

            else {
                if (read(named_pipe, &file_length, sizeof(int)) < 0){
                    printf("error \n");
                    exit(1);
                }

                printf("received number: %d \n", file_length);
            }

            memset(file_name, 0, SIZE * sizeof(char));
            memset(&file_mode, 0, sizeof(char));
            memset(buff, 0, SIZE * sizeof(char));
        }

        else{
            printf("You typed wrong mode\n");

            // initialization
            memset(file_name, 0, SIZE * sizeof(char));
            file_mode = 0;

            continue;
        }
    }

    return 0;
}
