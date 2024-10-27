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
    int bytes_of_file_read = 0;
    int bytes_of_file_write = 0;
    int fd; // file descriptor

    pid_t child;

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
        // receive access request from client
        if (read(named_pipe, buff, SIZE) < 0){
            printf("failed to call read");
            unlink(PIPE);
            exit(2);
        }

        sleep(1);

        if (!(strcmp(buff, "request\n")))
            printf("received request from the client \n");

        else if (!(strcmp(buff, "terminate\n"))){
            printf("received terminate signal from the client \n");
            unlink(PIPE);
            printf("----------terminating server----------\n\n");
            break;
        }

        else{
            printf("wrong request from the client \n");
            continue;
        }

        // receive access mode from client
        if (read(named_pipe, &access_mode, sizeof(char)) < 0) {
            printf("failed to call read() \n");
            exit(2);
        }

        sleep(1);

        printf("received access mode: %c\n", access_mode);

        // when the access mode is "r"
        if (access_mode == 'r') {

            // first of all, receive number of bytes to read
            if (read(named_pipe, &number_of_reading, sizeof(int)) < 0){
                printf("failed to receive number of bytes to read from client \n");
                unlink(PIPE);
                exit(2);
            }

            printf("received number of bytes to read: %d \n", number_of_reading);

            if (read(named_pipe, received_file_name, SIZE) < 0){
                printf("failed to receive file name from client \n");
                continue;
            }

            sleep(1);

            printf("received file name: %s \n", received_file_name);

            // fork child process
            child = fork();
            if (child == 0){
                fd = open(received_file_name, O_RDONLY);
                if (fd < 0){
                    printf("There's no file named %s\n", received_file_name);

                    if (write(named_pipe, "fail\n", 5 * sizeof(char)) < 0)
                        exit(3);

                    sleep(1);
                    exit(4);
                }

                if ((bytes_of_file_read = read(fd, buff, number_of_reading)) < 0){
                    printf("failed to read file\n");
                    unlink(PIPE);
                    exit(2);
                }

                if (bytes_of_file_read < number_of_reading)
                    buff[bytes_of_file_read] = '\0';

                else
                    buff[number_of_reading] = '\0';

                if (write(named_pipe, buff, SIZE) < 0){
                    printf("failed to send data string\n");
                    unlink(PIPE);
                    exit(3);
                }

                sleep(1);
                close(fd);

                memset(received_file_name, 0, SIZE * sizeof(char));
                memset(&access_mode, 0, sizeof(char));
                memset(buff, 0, SIZE * sizeof(char));

                exit(0);
            }

            else if (child == -1) {
                printf("failed to fork \n");
                unlink(PIPE);
                exit(-1);
            }

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
            if ((read(named_pipe, received_file_name, SIZE)) < 0){
                printf("Failed to receive file name from the client \n");
                continue;
            }

            sleep(2);

            printf("received file name from client: %s \n", received_file_name);

            bytes_of_file_write = read(named_pipe, buff, SIZE);
            if (bytes_of_file_write < 0){
                printf("Failed to receive data string from the client \n");
                unlink(PIPE);
                exit(2);
            }

            bytes_of_file_write = strlen(buff);
            buff[bytes_of_file_write] = '\n';

            child = fork();
            if (child == 0){
                fd = open(received_file_name, O_WRONLY | O_CREAT, 0666);
                if (fd < 0){
                    printf("Failed to write and creat file named %s \n", received_file_name);
                    if (write(named_pipe, "fail\n", 5 * sizeof(char)) < 0)
                        exit(3);

                    sleep(1);
                    exit(4);
                }

                lseek(fd, 0, SEEK_END);

                if (write(fd, buff, bytes_of_file_write+1) < 0){
                    printf("Failed to write data string \n");
                    unlink(PIPE);
                    exit(3);
                }

                sleep(1);

                close(fd);


                if (write(named_pipe, "succ\n", 5) < 0) {
                    printf("Failed to send number of written bytes \n");
                    unlink(PIPE);
                    exit(3);
                }

                sleep(1);

                printf("bytes of file write: %d \n", bytes_of_file_write);

                if ((write(named_pipe, &bytes_of_file_write, sizeof(int))) < 0){
                    printf("Failed to send number of written bytes \n");
                    unlink(PIPE);
                    exit(3);
                }

                sleep(1);

                memset(received_file_name, 0, SIZE * sizeof(char));
                memset(&access_mode, 0, sizeof(char));
                memset(buff, 0, SIZE * sizeof(char));
                bytes_of_file_write = 0;

                sleep(1);

                exit(0);
            }

            else if (child == -1) {
                printf("failed to fork \n");
                unlink(PIPE);
                exit(-1);
            }

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
