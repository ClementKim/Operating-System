### 아주대학교 사이버보안학과 운영체제 프로젝트 1 - Client-Server model을 이용한 concurrent 파일 서버 구현 프로젝트

## 프로그램 요구사항

1. client-server 구조를 기반으로 하고, named pipe를 통하여 프로세스간 통신 수행한다. (server에서 named pipe를 생성하고, client와 server에서 이것을 open)

2. client는 사용자로부터 file name, file access type (r: read or w: write), 읽을 bytes 수(read의 경우), data string(write의 경우)를 입력받는다.

3. client는 named pipe를 통해 서버에게 file access 요청한다.

4. server는 named pipe를 통해 file access 요청을 받은 후, 자신이 직접 수행하는 것이 아니라 concurrency를 위해 child를 fork하여 child에게 file access를 수행하고 응답하게 함(read의 경우 data string을 응답하고, write의 경우 쓰기를 한 후 bytes 수를 응답), 응답을 완료한 child는 종료된다.

5. 2-4의 과정을 반복하여 수행한다.

6. 구체적인 사용자 인터페이스 방법과 프로세스간 교신 데이터 형식은 자율적으로 정한다.


## 구현한 프로젝트 요약

  당해 프로젝트를 수행함에 있어 Server.c와 Client.c 두 가지로 구분하여 구현하였다. Server.c에서 named pipe를 생성하고, client.c는 이에 연결한다. 단, 여기서 Server,c는 실행하기 전부터 namedpipe가 존재한다면 이를 unlink하고 종료한다. Client.c는 namedpipe를 사용해 Server.c에 요청을, Server.c는 namedpipe를 통해 받은 Client.c의 요청을 수행하고, namedpipe를 사용해 Client.c에 수행 결과를 전송한다. 위 과정을 반복 수행함에 있어 발생하는 에러는 예외 처리하였다. 모든 과정을 수행하기 앞서 Client.c는 매번 사용자에게 종료할 것인지를 물어보고, 종료 요청이 발생하면 Server.c에 종료 신호를 전송해 namedpipe를 unlink하고, 두 프로그램 모두 종료되도록 구현하였다. File access type은 read와 write 두 가지이다.

  Read의 경우, Client.c는 파일명과 읽을 byte 수를 사용자로부터 입력받은 후, 이를 Server.c에 namedpipe를 사용해 전송한다. Server.c는 namedpipe를 통해 Client.c로부터 사용자 입력을 받아 파일이 존재할 경우, 읽을 byte 수만큼 파일을 읽고 namedpipe를 사용해 다시 Client.c에 전송한다. 존재하지 않는 경우, fail signal을 namedpipe를 사용해 전송한다.

  Write의 경우, 파일명과 데이터 스트링을 사용자로부터 입력받은 후, 이를 Server.c에 namedpipe를 사용해 전송한다. Server.c는 namedpipe를 통해 Client.c로부터 사용자 입력을 받아 파일이 존재하는 경우에는 해당 파일을 열고, 존재하지 않는 경우에는 생성하여 파일을 연 후, 해당 데이터 스트링을 작성한다. 사용자가 입력한 데이터 스트링을 모두 해당 파일에 작성하였다면 namedpipe를 사용해 작성한 데이터 스트링의 길이를 Client.c에 전송한다.

## 구현한 코드 설명

# server.c: named pipe 생성 및 연결, 존재하는 경우 unlink
```c
#define PIPE "./namedpipe"

if (access(PIPE, F_OK) == 0){
    unlink(PIPE);
    exit(1);
}

if (mkfifo(PIPE, 0666) == -1){
    printf("Failed to call named pipe\n");
    exit(1);
}

if ((named_pipe = open(PIPE, O_RDWR)) < 0){
    printf("Failed to open named pipe\n");
    unlink(PIPE);
    exit(1);
}
```

Access 명령어를 사용해 nmaedpipe가 존재하면 unlink 후 프로그램을 종료한다. 존재하지 않음을 확인했다면, mkfifo 명령어를 사용해 namedpipe를 RW-RW-RW 권한으로 생성한 후, open한다.

# client.c: named pipe 연결

```c
if ((named_pipe = open(PIPE, O_RDWR)) < 0){
    printf("Failed to open named pipe\n");
    exit(1);
}
```

open명령어를 사용해 namedpipe에 연결한다. 만약 존재하지 않는 경우, "Failed to open named pipe"를 출력하도록 구현하였다.

# client.c: 사용자로부터 termination 희망 여부 입력 받기
```c
if (access(PIPE, F_OK) < 0)
    exit(1);

printf("do you want to terminate this code?(y: yes, n: no): ");
scanf("%c", &terminate);
while (getchar() != '\n');

if (terminate == 'y'){
    printf("Terminating the server and the client\n");

    while (write(named_pipe, "terminate\n", sizeof("terminate\n")) < 0);
    printf("----------terminating client----------\n\n");

    break;
}

else if (terminate == 'n'){
    NULL;
}

else{
    printf("You typed improper temination request \n");
    continue;
}
```

namedpipe가 존재하는지 상시로 체크하고, 사용자로부터 종료할 것인지를 입력받는다. 만약 사용자가 종료를 원한다면 namedpipe를 사용해 server.c에 termination signal을 전송하고 client.c를 종료한다.

# client.c: 사용자로부터 파일 이름과 file access type 입력 받기
```c
printf("Enter the file name: ");
fgets(file_name, SIZE, stdin);
file_name[strlen(file_name) - 1] = '\0'; // change new line to eof

printf("Type the file access type (r: read, w: write): ");
scanf("%c", &file_mode);
while(getchar() != '\n'); // eliminating new lines
```

client는 사용자로부터 파일 이름과 file access type을 입력받는다. 여기서 file access type은 r 또는 w 두 가지만 가능하며, 개행문자 처리를 while문을 사용해 수행하였다.

file access type이 r인 경우, 읽을 bytes 수를 사용자로부터 입력받고, 동일하게 while문을 사용해 개행문자를 처리하였다. 이후, namedpipe를 사용해 server.c에 request signal을 전송하는 것과 전송에 실패했을 경우에 처리하는 것을 구현하였다.

```c
if (file_mode == 'r'){
    printf("Enter the number of bytes to read: ");
    scanf("%d", &byte);
    while (getchar() != '\n'); // eliminating new lines

    if ((write(named_pipe, "request\n", SIZE)) < 0) {
        printf("failed to request to the server \n");
        exit(1);
    }

sleep(1);
```

file access type이 w인 경우, 작성할 data string을 사용자로부터 입력받고, 개행문자를 eof로 변경하였다. 이후, r일 때와 동일하게 namedpipe를 사용해 server.c에 request signal을 전송하는 것과 실패했을 경우에 처리하는 것을 구현하였다.

```c
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
```

file access type이 r, w 둘 다 아닌 경우의 예외 처리는 다음과 같다.

```c
else
    printf("You typed improper mode \n");
```

# server.c: request 처리 (termination signal과 request signal 모두)

Server.c는 namedpipe를 사용해 Client.c의 요청을 받는다. 요청을 처리하기 전 항상 namedpipe가 존재하는지 확인한다. Namedpipe가 존재하지 않는 경우에는 프로그램을 종료한다. 존재하는 경우에는 받은 signal을 확인한다. request를 받은 경우와 terminate를 받은 경우에는 정상적으로 받았음을 출력한다. 하지만 signal을 잘못 받은 경우에는 unlink를 수행하고, request 또는 teminate가 아닌 경우에는 잘못된 signal을server가 받았음을 사용자에게 알려준다. 특히 terminate signal을 받은 경우에는 unlink를 수행하고, Server.c를 종료하는 작업을 수행한다.

```c
if (access(PIPE, F_OK) < 0){
    exit(1);
}

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
```

# server.c: file access type받기부터 child fork, child termination까지의 과정

Server.c는 namedpipe를 사용해 Client.c에서 사용자에게 입력받은 access mode를 받는다. 만약 access mode를 받는데 실패한 경우에는 unlink 후 종료한다. 성공한 경우에는 이를 출력한다.

```c
if (read(named_pipe, &access_mode, sizeof(char)) < 0) {
    printf("failed to call read() \n");
    unlink(PIPE);
    exit(2);
}

sleep(1);

printf("received access mode: %c\n", access_mode);
```

1. file access mode가 r인 경우
우선 namedpipe를 사용해 Client.c에서 사용자로부터 입력받은 읽을 byte 수, 파일명을 받는다. 위 두 가지는 실패했을 경우, unlink 후 종료한다. 성공했을 경우에는 받은 읽을 byte 수와 파일명을 출력한 후, child를 fork한다. 

Fork한 child는 사용자로부터 입력받은 파일명을 바탕으로 파일을 RDONLY 모드로 연다. 실패한다면 namedpipe를 사용해 Client.c에 fail signal을 보내고, unlink 후 종료한다. 만약 Client.c에 fail signal을 보내는 것을 실패하였다면 동일하게 unlink후 종료한다. 

파일 열기에 성공하였다면 file descriptor를 이용해 읽을 byte 수만큼을 파일에서 읽는다. 읽기에 실패하면 unlink후 종료한다. 이후 개행문자 처리를 수행한 후, namedpipe를 사용해 읽은 byte들을 Client.c에 전송한다. 실패한 경우에는 실패했음을 알리고 unlink 후 종료한다. 전송에 성공하였다면 file descriptor를 닫고, 변수 초기화를 수행한다.

Child fork에 실패한 경우의 예외 처리는 두 가지이다. 첫째, fork를 시도했으나 실패한 경우, 둘째, fork를 하는데 시간이 너무 오래 걸린 경우이다. 두 가지 모두 실패했음을 출력한 후, unlink 및 종료하도록 구현하였다.

```c
if (access_mode == 'r') {
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

    child = fork();
    if (child == 0){
        fd = open(received_file_name, O_RDONLY);
        if (fd < 0){

            printf("There's no file named %s\n", received_file_name);

            if (write(named_pipe, "fail\n", 5 * sizeof(char)) < 0) {
                unlink(PIPE);
                exit(3);
            }

            sleep(1);

            unlink(PIPE);

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
        bytes_of_file_write = 0;
        bytes_of_file_read = 0;

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
```

2. file access type이 w인 경우
우선 namedpipe를 사용해 Client.c에서 사용자로부터 입력받은 파일명과 작성할 데이터 스트링을 받는다. 파일명의 경우 r과 동일하게 예외 처리를 수행하였다. 데이터 스트링을 받는 것에 실패하였다면 unlink 후 종료하도록 구현하였다. 여기서 read 명령어를 사용하여 데이터 스트링을 받으면 Client.c가 전송한 512가 bytes_of_file_write에 저장된다. 따라서 우선 bytes_of_file_write 변수를 사용해 받는 것에 성공하였는지 확인한 후, 실제 데이터 스트링의 길이를 저장한다. 데이터 스트링의 마지막을 eof에서 줄바꿈 문자로 바꿔준다. 이는 파일에 작성된 후, 줄바꿈으로 파일에 새로 작성된 데이터 스트링을 구별하기 위함이다. 이후 child를 fork한다.

Fork한 child는 전송받은 파일명을 바탕으로 파일을 RW-RW-RW 권한으로 연다. 이때 파일이 존재하면 O_WRONLY, 존재하지 않으면 O_CREAT로 연다. 파일 열기에 실패한 경우, R인 경우와 동일하게 예외 처리를 수행하였다. 파일 열기에 성공한 경우, file descriptor가 파일의 가장 마지막 부분을 가르키도록 한 후, 작성하도록 구현하였다. 작성에 실패한 경우, 작성에 실패했음을 출력하고 unlink 및 종료하도록 하였다. 성공한 경우에는 file descriptor를 닫고, success signal을 namedpipe를 사용해 전송하였다. Success signal 전송에 실패한 경우에는 unlink 및 종료하도록 구현하였고, 성공한 경우에는 작성한 byte 수를 namedpipe를 통해 전송하도록 하였다. 실패한 경우에는 동일하게 unlink 및 종료하도록 구현하였다. 위 과정을 수행한 후, 초기화를 수행하고 child가 terminate되도록 하였다.

Access mode가 r인 경우와 동일하게 Child fork에 실패한 경우의 예외 처리는 두 가지이다. 첫째, fork를 시도했으나 실패한 경우, 둘째, fork를 하는데 시간이 너무 오래 걸린 경우이다. 두 가지 모두 실패했음을 출력한 후, unlink 및 종료하도록 구현하였다.

```c
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
    buff[bytes_of_file_write] = '\n'; // change eof to new line

    child = fork();
    if (child == 0){
        fd = open(received_file_name, O_WRONLY | O_CREAT, 0666);
        if (fd < 0){
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
```

3. file access mode가 r과 w가 모두 아닐 때
named pipe를 통해 받은 file access mode가 r과 w 모두가 아닌 경우에는 잘못된 file access mode임을 출력하고 continue를 통해 되돌아가도록 구현하였다.

```c
else {
    printf("wrong access mode \n");
    continue;
}
```

# client.c: file access mode가 r인 경우
위에서 request signal을 namedpipe를 사용해 전송한 이후의 과정이다. 우선 access mode와 읽을 byte 수, 파일명을 namedpipe를 사용해 Server.c로 전송한다. 이 세 가지 각각은 실패했을 경우, 해당 실패 메시지를 출력 및 종료하도록 구현하였다. 그 후, Server.c의 요청 사항 처리 결과를 받는 과정을 수행한다. Namedpipe를 사용해 요청 결과를 받는데 실패하였다면 실패 메시지를 출력하고 종료한다. 만약 namedpipe를 통해 받은 요청 결과가 fail인 경우는 파일이 존재하지 않는 경우이기에 파일이 존재하지 않음을 출력한다. Fail signal이 아닌 경우에는 Server.c로부터 받은 읽을 byte 수만큼의 글을 출력한다.

```c
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
    exit(2);
}

sleep(1);

if (!(strcmp(buff, "fail\n"))){
    printf("There's no file named %s \n", file_name);
}

else
    printf("%s\n", buff);
```

# client.c: file access mode가 w인 경우
위에서 request signal을 namedpipe를 사용해 전송한 이후의 과정이다. 우선 access mode와 파일명, 데이터 스트링을 사용자로부터 입력받는다. 이 세 가지 각각은 실패했을 경우, 해당 실패 메시지를 출력 및 종료하도록 구현하였다. 그 후, Server.c의 요청 사항 처리 결과를 받는 과정을 수행한다. Access mode가 r인 경우와 동일하게 Namedpipe를 사용해 요청 결과를 받는데 실패하였다면 실패 메시지를 출력하고 종료한다. 만약 namedpipe를 통해 받은 요청 결과가 fail인 경우는 파일이 존재하지 않는 경우이기에 파일이 존재하지 않음을 출력한다. Fail signal이 아닌 경우에는 Server.c로부터 받은 작성한 문자열의 길이를 출력한다. 작성한 문자열의 길이를 받는데 실패하였다면 실패 메시지를 출력한 후 종료한다.

```c
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
    exit(2);
}

sleep(1);

if (!(strcmp(buff, "fail\n"))){
    printf("There's no file named %s \n", file_name);
}

else {
    if (read(named_pipe, &file_length, sizeof(int)) < 0){
        printf("Failed to receive the length of the file written \n");
        exit(2);
    }

    printf("received the length of the file written: %d \n", file_length);
}
```

## 시작 가이드

# 주의사항
server가 먼저 실행되어야 한다. 만약 client가 sever보다 먼저 실행된다면 named pipe가 존재하지 않기 때문에 "Failed to open named pipe"라는 오류 메시지가 출력된 후 종료한다.

# Installation
```sh
git clone git@github.com:ClementKim/Operating-System.git
cd project1
```

# Server
```sh
gcc -o server server.c
./server
```

# Client
```sh
gcc -o client client.c
./client
```

## Reference
Abraham Silberschatz, Peter Bart Gaalvin, Greg Gagne - 'Operating System Concepts (tenth edition)'

https://github.com/ClementKim/Operating-System
