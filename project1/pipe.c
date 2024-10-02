#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

#define MAXLINE 4096
#define STDOUT_FILENO 1

void client(int, int);
void server(int, int);

int main(int argc, char *argv[]){
    int pipe1[2], pipe2[2];
    pid_t childpid;

    pipe(pipe1); // 올바른 파이프 호출
    pipe(pipe2); // 올바른 파이프 호출

    if ((childpid = fork()) == 0) { // child
        close(pipe1[1]); // 쓰기 닫기
        close(pipe2[0]); // 읽기 닫기

        server(pipe1[0], pipe2[1]); // 읽기 및 쓰기 디스크립터 전달
        exit(0);
    }

    // parent
    close(pipe1[0]); // 읽기 닫기
    close(pipe2[1]); // 쓰기 닫기

    client(pipe1[1], pipe2[0]); // 쓰기 및 읽기 디스크립터 전달

    waitpid(childpid, NULL, 0); // wait for child to terminate

    exit(0);
}

void client(int writefd, int readfd) { // writefd와 readfd의 순서 수정
    size_t len;
    size_t n;
    char buff[MAXLINE];

    // read pathname
    fgets(buff, MAXLINE, stdin);
    len = strlen(buff);
    if (buff[len-1] == '\n')
        len--;

    write(writefd, buff, len);

    while ((n = read(readfd, buff, MAXLINE)) > 0)
        write(STDOUT_FILENO, buff, n);
}

void server(int readfd, int writefd) { // readfd와 writefd의 순서 수정
    int fd;
    size_t n;
    char buff[MAXLINE + 1];

    if ((n = read(readfd, buff, MAXLINE)) == 0) {
        printf("end-of-file");
        exit(0);
    }

    buff[n] = '\0';

    if ((fd = open(buff, O_RDONLY)) < 0) {
        snprintf(buff + n, sizeof(buff) - n, ": can't open, %s\n", strerror(errno));
        n = strlen(buff);
        write(writefd, buff, n);
    } else {
        while ((n = read(fd, buff, MAXLINE)) > 0)
            write(writefd, buff, n);
        close(fd);
    }
}
