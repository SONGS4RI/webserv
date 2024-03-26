## 프로젝트 소개

> Nginx의 주요 구조 및 기능을 모델로 삼아 정적인 데이터를 다루는 간단한 웹서버를 구현해 본 프로젝트입니다.
> 

<img width="915" alt="webserv" src="https://github.com/SONGS4RI/webserv/assets/100538007/e6fa636d-69e2-4a46-b682-20c143eaaaa3">

## 프로그램 실행 방법
1. Makefile이 존재하는 디렉토리 위치에서 콘솔에 `make`
2. ./webserv [config 파일] 을 통해 실행
3. http://localhost:[port] 로 접속

## 프로젝트 개발 기간

> **`2024.02.01 ~ 2024.03.19`**
> 

## 기술 스택

> `C++` `소켓 통신` `HTTP`
> 

## 기능 소개

- I/O 멀티플랙싱 기법인 kqueue()을 사용하여 웹서버를 효율적으로 구현하였습니다.
- config 파일을 통해 웹서버의 간단한 설정을 할 수 있습니다.
- GET, DELETE
    - 리소스 접근이 허용된 URL이라면 리소스에 대한 올바른 처리를 합니다.
- POST
    - application/octet-stream 타입을 통해 바이너리 파일로 저장할 수 있습니다.
    - multipart/form-data 타입의 경우 python 스크립트의 CGI를 사용하여 파일을 저장합니다.
