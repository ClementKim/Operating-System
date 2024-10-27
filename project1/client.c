#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 512
#define TRUE 1

#define PIPE "./namedpipe"

int main(void){
    int named_pipe;
    int byte;
    int file_length;

    char file_name[SIZE] = {0};
    char data_string[SIZE] = {0};
    char file_mode;
    char buff[SIZE] = {0};
    char terminate;

    // Opening named pipe
    if ((named_pipe = open(PIPE, O_RDWR)) < 0){

        // when it fails
        printf("Failed to open named pipe\n");
        exit(1);
    }

    printf("----------client----------\n\n");

    while (TRUE){

        // when the named pipe doesn't exist
        if (access(PIPE, F_OK) < 0)
            exit(1);

        // type whether user wants to terminate this program or not
        printf("do you want to terminate this code?(y: yes, n: no): ");
        scanf("%c", &terminate);
        while (getchar() != '\n');

        // If user wants to terminate this program
        if (terminate == 'y'){
            printf("Terminating the server and the client\n");

            // send termination signal to the server
            while (write(named_pipe, "terminate\n", sizeof("terminate\n")) < 0);
            printf("----------terminating client----------\n\n");

            // break the loop
            break;
        }

        // when the termination request type is "n", pass
        else if (terminate == 'n'){
            NULL;
        }

        // neither "y" nor "n"
        else{
            printf("You typed improper temination request \n");
            continue;
        }

        // receiving the file name
        printf("Enter the file name: ");
        fgets(file_name, SIZE, stdin);
        file_name[strlen(file_name) - 1] = '\0'; // change new line to eof

        // receiving file access mode
        printf("Type the file access type (r: read, w: write): ");
        scanf("%c", &file_mode);
        while(getchar() != '\n'); // eliminating new lines

        // when the access mode is "read"
        if (file_mode == 'r'){

            // receiving the number of bytes to read
            printf("Enter the number of bytes to read: ");
            scanf("%d", &byte);
            while (getchar() != '\n'); // eliminating new lines

            // send request signal to the server
            if ((write(named_pipe, "request\n", SIZE)) < 0) {

                // when it fails
                printf("failed to request to the server \n");
                exit(1);
            }

            sleep(1);

            // send access mode to the server
            if ((write(named_pipe, &file_mode, sizeof(char))) < 0){

                // when it fails
                printf("failed to send file mode \n");
                exit(1);
            }

            sleep(1);

            // send the number of bytes to read to the server
            if ((write(named_pipe, &byte, sizeof(int))) < 0){

                // when it fails
                printf("failed to send number of bytes to read \n");
                exit(1);
            }

            sleep(1);

            // send the file name to the server
            if ((write(named_pipe, file_name, SIZE)) < 0){

                // when it fails
                printf("failed to send file name \n");
                exit(1);
            }

            sleep(2);

            // receiving the result signal from the server
            // when it fails, receives "fail"
            // when it doesn't fail, receives what server read
            if ((read(named_pipe, buff, SIZE)) < 0){

                // when it fails
                printf("failed to receive data from the server \n");
                exit(2);
            }

            sleep(1);

            // when the received result is fail: user typed file name is not on this directory
            if (!(strcmp(buff, "fail\n"))){
                printf("There's no file named %s \n", file_name);
            }

            else {

                // when the received result is not fail: user typed file name is on this directory
                // print what server read
                printf("%s\n", buff);
            }
        }

        // when the access mode is "write"
        else if (file_mode == 'w'){

            // receiving what user wants to write
            printf("Type the string to write: ");
            fgets(data_string, SIZE, stdin);
            data_string[strlen(data_string)-1] = '\0'; // changes new line to eof

            // send request signal to the server
            if ((write(named_pipe, "request\n", SIZE)) < 0) {

                // when it fails
                printf("failed to request to the server \n");
                exit(1);
            }

            sleep(1);

            // send access mode to the server
            if ((write(named_pipe, &file_mode, sizeof(char))) < 0) {

                // when it fails
                printf("Failed to send file mode to server \n");
                exit(1);
            }

            // send file name to the server
            if ((write(named_pipe, file_name, SIZE)) < 0) {

                // when it fails
                printf("failed to send file name\n");
                exit(1);
            }

            sleep(2);

            // send user typed data string to the server
            if ((write(named_pipe, data_string, SIZE)) < 0){

                // when it fails
                printf("Failed to write data string \n");
                exit(1);
            }

            sleep(2);

            // receiving the result from the server
            // when it fails, receives "fail"
            // when it doesn't fail, receives the length of the file written
            if (read(named_pipe, buff, SIZE) < 0){

                // when it fails
                printf("failed to receive data from the server \n");
                exit(2);
            }

            sleep(1);

            // when the received result is fail: user typed file name is not on this directory
            if (!(strcmp(buff, "fail\n"))){
                printf("There's no file named %s \n", file_name);
            }

            // when the received result is not fail: user typed file name is on this directory
            else {

                // receives the length of the file written
                if (read(named_pipe, &file_length, sizeof(int)) < 0){

                    // when it fails
                    printf("Failed to receive the length of the file written \n");
                    exit(2);
                }

                // print the length of the file written
                printf("received the length of the file written: %d \n", file_length);
            }

        }

        // when user types improper access mode
        else{
            printf("You typed improper mode \n");
        }

        // initialization
        memset(file_name, 0, SIZE * sizeof(char));
        memset(&file_mode, 0, sizeof(char));
        memset(buff, 0, SIZE * sizeof(char));
        file_mode = 0;
        terminate = 0;

    }

    return 0;
}
