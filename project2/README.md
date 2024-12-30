# Synchronization and Concurrency

## 프로젝트 개요

이 프로젝트는 운영체제의 동기화 및 동시성 문제를 해결하기 위해 다양한 시나리오에서 프로듀서-컨슈머 문제와 리더-라이터 문제를 구현합니다. 두 문제는 각각 기본 구현과 **뮤텍스(Mutex)**를 이용한 해결 방식으로 나뉩니다.

## 주요 구현 내용

1. 프로듀서-컨슈머 문제 (Producer-Consumer Problem)

A. 기본 구현 (pc.c)
	•	설명: 생산자와 소비자가 공유 버퍼를 사용하여 데이터를 생산하고 소비하는 문제를 해결.
	•	특징:
	•	단순한 버퍼 동기화 로직.
	•	동기화 도구 없이 생산자-소비자 작업을 시뮬레이션.

B. 뮤텍스를 활용한 해결 (pc_mutex.c)
	•	설명: 기본 구현에서 발생할 수 있는 경쟁 조건(race condition)을 방지.
	•	특징:
	•	뮤텍스와 조건 변수(pthread_cond_t)를 이용하여 안전한 동기화 구현.
	•	공유 자원에 대한 접근 제어.

2. 리더-라이터 문제 (Readers-Writers Problem)

A. 기본 구현 (rw.c)
	•	설명: 다수의 리더(읽기 작업)와 소수의 라이터(쓰기 작업)가 공유 자원에 접근하는 문제를 해결.
	•	특징:
	•	리더와 라이터의 접근 우선순위 처리.
	•	동기화 없이 동작 시뮬레이션.

B. 뮤텍스를 활용한 해결 (rw_mutex.c)
	•	설명: 기본 구현의 동기화 문제를 해결하여 데이터 무결성을 보장.
	•	특징:
	•	뮤텍스를 사용하여 읽기와 쓰기 간 동시성을 제어.
	•	여러 리더의 동시 접근 허용, 단 쓰기 작업은 단일 접근으로 제한.

## 실행 방법

1. 파일 컴파일
```sh
gcc -o pc pc.c -lpthread
gcc -o pc_mutex pc_mutex.c -lpthread
gcc -o rw rw.c -lpthread
gcc -o rw_mutex rw_mutex.c -lpthread
```

2. 프로그램 실행

2-1. producer-consumer problem
```sh
./pc
```

2-2. producer-consumer problem with mutex solution
```sh
./pc_mutex
```

2-3. reader-writer problem
```sh
./rw
```

2-4. reader-writer problem with mutex solution
```sh
./rw_mutex
```

## 참고 자료
	•	Linux Man Pages: pthread_mutex, pthread_cond.
	•	Silberschatz, Galvin, and Gagne, Operating System Concepts.
