#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        // 자식 프로세스
        std::cout << "자식 프로세스 실행\n";
        // 여기서 자식 프로세스 작업을 수행
		int cnt = 1;
		while (cnt < 5) {
			cout << cnt++ << "초 대기중(자식)...\n";
			sleep(1);
		}
        exit(0); // 자식 프로세스 종료
    } else if (pid > 0) {
        // 부모 프로세스
        std::cout << "부모 프로세스 계속 실행\n";
        // 부모 프로세스 작업 계속
        // 예를 들어, 여기서 메인 이벤트 루프를 실행할 수 있음
		int cnt = 1;
		int status;
        while (waitpid(pid, &status, WNOHANG) == 0) {
            cout << cnt++ << "초 대기중(부모)...\n";
            sleep(1); // 예시를 위한 대기
        }
        cout << "자식 종료\n";
    } else {
        // fork 실패
        perror("fork 실패");
        exit(1);
    }

    return 0;
}