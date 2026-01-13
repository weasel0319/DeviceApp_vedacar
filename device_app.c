#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "device_app.h"

int main(int argc, char **argv) {
    if (argc < 3) { // 포트 번호를 인자로 받기
        printf("Usage : %s ip port\n", argv[0]);
        return -1;
    }
    printf("main code\n");
    rxData *rdata = (rxData*)malloc(sizeof(rxData)); // 전체적인 제어 데이터 담당 구조체 생성
    memset(rdata, 0, sizeof(rxData));
    
    makeThread(rdata);                               // 모터 스레드 생성(추가 기능 있을 시 여기만 변경 예정)
    makeConnection(rdata, argv);           // 통신 담당 함수 호출

    return 0;
}