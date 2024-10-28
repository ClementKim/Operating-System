#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#define SIZE 512
#define TRUE 1

#define PIPE "./namedpipe"

int main(void){
    int named_pipe;
    int number_of_reading = 0;
    int bytes_of_file_write = 0;
    int bytes_of_file_read;
    int fd; // file descriptor

    pid_t child; // for fork

    char received_file_name[SIZE] = {0};
    char access_mode;
    char buff[SIZE] = {0};

    // unlink namedpipe if already exists
    if (access(PIPE, F_OK) == 0){
        unlink(PIPE);
        exit(1);
    }

    // create namedpipe
    if (mkfifo(PIPE, 0666) == -1){
        printf("Filed to call named pipe\n");
        exit(1);
    }

    // open namedpipe with read write mode
    if ((named_pipe = open(PIPE, O_RDWR)) < 0){
        printf("Failed to open named pipe\n");
        unlink(PIPE);
        exit(1);
    }

    printf("----------server----------\n\n");

    while (TRUE){
        // exit when the named pipe doesn't exist
        if (access(PIPE, F_OK) < 0){
            exit(1);
        }

        // memset(buff, 0, SIZE * sizeof(char));
        // receive access request from client
        // types of access request: request, terminate
        if (read(named_pipe, buff, SIZE) < 0){
            printf("failed to call read");
            unlink(PIPE);
            exit(2);
        }

        sleep(1);

        // check if received access request is "request" or not
        if (!(strcmp(buff, "request\n")))
            printf("received request from the client \n");

        // check if received access request is "terminate" or not
        else if (!(strcmp(buff, "terminate\n"))){
            printf("received terminate signal from the client \n");

            // unlink named pipe and then break the loop
            unlink(PIPE);
            printf("----------terminating server----------\n\n");
            break;
        }

        // when neither "request" nor "terminate"
        else{
            printf("wrong request from the client \n");
            continue;
        }

        // receiving access mode from client
        if (read(named_pipe, &access_mode, sizeof(char)) < 0) {

            // when it fails
            printf("failed to call read() \n");
            unlink(PIPE);
            exit(2);
        }

        sleep(1);

        // print out the access mode for checking
        printf("received access mode: %c\n", access_mode);

        // when the access mode is "r"
        if (access_mode == 'r') {

            // first of all, receive number of bytes to read
            if (read(named_pipe, &number_of_reading, sizeof(int)) < 0){

                // when it fails, unlink named pipe
                printf("failed to receive number of bytes to read from client \n");
                unlink(PIPE);
                exit(2);
            }

            // print out received number of reading for checking
            printf("received number of bytes to read: %d \n", number_of_reading);

            // receiving file name from the client
            if (read(named_pipe, received_file_name, SIZE) < 0){

                // when it fails
                printf("failed to receive file name from client \n");
                continue;
            }

            sleep(1);

            // print out received file name for checking
            printf("received file name: %s \n", received_file_name);

            // fork child process
            child = fork();
            if (child == 0){

                // file open
                fd = open(received_file_name, O_RDONLY);
                if (fd < 0){

                    // when it fails
                    printf("There's no file named %s\n", received_file_name);

                    // send fail signal to the client and then unlink named pipe
                    if (write(named_pipe, "fail\n", 5 * sizeof(char)) < 0) {

                        // when it fails
                        unlink(PIPE);
                        exit(3);
                    }

                    sleep(1);

                    unlink(PIPE);

                    exit(4);
                }

                // receiving letters, its length is same with the number of reading, in the file
                if ((bytes_of_file_read = read(fd, buff, number_of_reading)) < 0){

                    // when it fails, unlink the named pipe
                    printf("failed to read file\n");
                    unlink(PIPE);
                    exit(2);
                }

                // when the bytes of file read is smaller than number of reading
                if (bytes_of_file_read < number_of_reading)
                    buff[bytes_of_file_read] = '\0';

                // else case
                else
                    buff[number_of_reading] = '\0';

                // send letters read from the file to the client
                if (write(named_pipe, buff, SIZE) < 0){

                    // when it fails, unlink the named pipe
                    printf("failed to send data string\n");
                    unlink(PIPE);
                    exit(3);
                }

                sleep(1);

                // close file descriptor
                close(fd);

                // initialization
                memset(received_file_name, 0, SIZE * sizeof(char));
                memset(&access_mode, 0, sizeof(char));
                memset(buff, 0, SIZE * sizeof(char));
                bytes_of_file_write = 0;
                bytes_of_file_read = 0;

                // child termination
                exit(0);
            }

            // when it fails to fork, unlink the named pipe
            else if (child == -1) {
                printf("failed to fork \n");
                unlink(PIPE);
                exit(-1);
            }

            // when it fails to fork, unlink the named pipe
            else {
                if ((wait(0) == -1)) {
                    printf("failed to fork\n");
                    unlink(PIPE);
                    exit(-1);
                }
            }
        }

        // when the access mode is "w"
        else if (access_mode == 'w'){

            // receiving the file name from the client
            if ((read(named_pipe, received_file_name, SIZE)) < 0){

                // when it fails
                printf("Failed to receive file name from the client \n");
                continue;
            }

            sleep(2);

            // print out the file name for checking
            printf("received file name from client: %s \n", received_file_name);


            // receiving the bytes of file writes from the client
            bytes_of_file_write = read(named_pipe, buff, SIZE);

            // when it fails, unlink named pipe
            if (bytes_of_file_write < 0){
                printf("Failed to receive data string from the client \n");
                unlink(PIPE);
                exit(2);
            }

            // the first status of the bytes of file write is 512
            // therefore change it to actual bytes of file write
            bytes_of_file_write = strlen(buff);
            buff[bytes_of_file_write] = '\n'; // change eof to new line

            // fork child process
            child = fork();
            if (child == 0){

                // open file with RW-RW-RW permission (no execution permission)
                // when it already exists, O_WRONLY
                // when it doesn't exist, O_CREAT
                fd = open(received_file_name, O_WRONLY | O_CREAT, 0666);
                if (fd < 0){

                    // when it fails, send fail signal to client and then unlink the named pipe
                    printf("Failed to write and creat file named %s \n", received_file_name);
                    if (write(named_pipe, "fail\n", 5 * sizeof(char)) < 0) {

                        // when it fails
                        unlink(PIPE);
                        exit(3);
                    }

                    sleep(1);

                    unlink(PIPE);

                    exit(4);
                }

                // making file descriptor to point the end of the file
                lseek(fd, 0, SEEK_END);

                // write on the file
                if (write(fd, buff, bytes_of_file_write+1) < 0){

                    // when it fails, unlink the named pipe
                    printf("Failed to write data string \n");
                    unlink(PIPE);
                    exit(3);
                }

                sleep(1);

                // close file descriptor
                close(fd);

                // when it successes, send success signal to the client
                if (write(named_pipe, "succ\n", 5) < 0) {

                    // when it fails, unlink the named pipe
                    printf("Failed to send number of written bytes \n");
                    unlink(PIPE);
                    exit(3);
                }

                sleep(1);

                // print out how many bytes are used
                printf("bytes of file write: %d \n", bytes_of_file_write);

                // send bytes of file write to the client
                if ((write(named_pipe, &bytes_of_file_write, sizeof(int))) < 0){

                    // when it fails, unlink the named pipe
                    printf("Failed to send number of written bytes \n");
                    unlink(PIPE);
                    exit(3);
                }

                sleep(1);


                // initialization
                memset(received_file_name, 0, SIZE * sizeof(char));
                memset(&access_mode, 0, sizeof(char));
                memset(buff, 0, SIZE * sizeof(char));
                bytes_of_file_write = 0;

                sleep(1);

                // child termination
                exit(0);
            }

            // when it fails to fork, unlink the named pipe
            else if (child == -1) {
                printf("failed to fork \n");
                unlink(PIPE);
                exit(-1);
            }

            // when it fails to fork, unlink the named pipe
            else {
                if ((wait(0) == -1)) {
                    printf("failed to fork\n");
                    unlink(PIPE);
                    exit(-1);
                }
            }

        }

        // when the access mode is neither "r" nor "w"
        else {
            printf("wrong access mode \n");
            continue;
        }

        sleep(1);

    }

    return 0;
}
