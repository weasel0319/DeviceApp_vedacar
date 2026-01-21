#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "device_app.h"

int main(int argc, char **argv) {
    if (argc < 3) { // 포트 번호를 인자로 받기
        printf("Usage : %s ip port\n", argv[0]);
        fflush(stdout);
        return 1;
    }

    char current_path[100];
    if (getcwd(current_path, sizeof(current_path)) != NULL) {
        printf("pwd: %s\n", current_path);
        fflush(stdout);
    } else {
        perror("getcwd failed"); // 에러 발생 시 에러 메시지 출력
        fflush(stdout);
        return 1;
    }
    strcat(current_path, "/wificonnect.sh");

    int result = system(current_path);
    int exit_status = WEXITSTATUS(result);
    if (exit_status != 0) {
        printf("WiFi 연결 실패 에러 코드: %d\n", exit_status);
        fflush(stdout);
        return 1;
    }

    printf("main code\n");
    fflush(stdout);
    rxData *rdata = (rxData*)malloc(sizeof(rxData)); // 전체적인 제어 데이터 담당 구조체 생성
    memset(rdata, 0, sizeof(rxData));
    //makeSvThread(rdata); //rtsp 테스트용 혹시 몰라 남겨둠
    //makeThread(rdata);                               // 모터 스레드 생성(추가 기능 있을 시 여기만 변경 예정)
    makeConnection(rdata, argv);           // 통신 담당 함수 호출
    return 0;
}