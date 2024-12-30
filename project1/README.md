# Concurrent File Server using Named Pipe

## 프로젝트 개요

이 프로젝트는 Client-Server 모델을 기반으로 프로세스 간 통신을 수행하며, Named Pipe를 이용한 Concurrent File Server를 구현합니다. 서버는 클라이언트로부터 파일 읽기/쓰기 요청을 받아 Fork를 통해 자식 프로세스에서 작업을 처리하고, 클라이언트에 결과를 반환합니다. 이 시스템은 동시성(Concurrency)과 안정성을 고려하여 설계되었습니다.


## 주요 기능

1.	Named Pipe 기반 Client-Server 통신:
    서버와 클라이언트 간 요청 및 응답 데이터 교환.

2.	파일 읽기 요청 (Read):
    클라이언트가 요청한 파일에서 지정된 바이트만큼 데이터를 읽어 반환.

3.	파일 쓰기 요청 (Write):
    클라이언트가 지정한 파일에 데이터를 작성. 파일이 없을 경우 생성 후 작성.

4.	동시성 지원:
    요청마다 fork를 통해 자식 프로세스를 생성하여 병렬 처리.
	
5.	서버 종료 요청:
    클라이언트에서 종료 신호를 보내 서버가 안전하게 종료.

## 실행 방법

1. 서버 실행
```sh
gcc -o server server.c
./server
```

caution: 서버는 클라이언트보다 먼저 실행되어야 합니다.


2. 클라이언트 실행
```sh
gcc -o client client.c
./client
```


## 예제 실행 시나리오

1. 파일 읽기 요청 (Read Mode)

클라이언트에서 파일 이름과 읽을 바이트 수를 입력:
```
Enter the file name: example.txt
Type the file access type (r: read, w: write): r
Enter the number of bytes to read: 20
```

서버가 요청을 처리하고, 해당 바이트의 내용을 반환:
```
File content: This is an example.
```

2. 파일 쓰기 요청 (Write Mode)

클라이언트에서 파일 이름과 작성할 데이터를 입력:
```
Enter the file name: newfile.txt
Type the file access type (r: read, w: write): w
Type the string to write: Hello, Named Pipe!
```

서버가 데이터를 작성하고 결과를 반환:

```
Data written: 18 bytes
```

3. 서버 종료 요청

클라이언트에서 종료 신호를 전송:
```
Do you want to terminate this code? (y: yes, n: no): y
```

서버 종료:
```
Received termination signal. Closing server.
```

## 참고 자료
    - Abraham Silberschatz, Peter B. Galvin, Greg Gagne, Operating System Concepts (10th Edition), Chapter 3, 4, 5.
    - Linux Man Pages: mkfifo, open, read, write, unlink.
