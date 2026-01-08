#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#include "device_app.h"

void makeThread(rxData *rdata) { // 나중에 추가 기능 생기면 함수 호출 식으로 변경할 예정
    pthread_t motorthread;

    if (sem_init(&rdata->semid, 0, 1) != 0)
        perror("sem_init failed");

    pthread_create(&motorthread, NULL, MotorControl, rdata);
    pthread_detach(motorthread); 
}

void makeSvThread(rxData *rdata) { // 연결 수립시 생성되는 스레드
    pthread_t serverthread;

    pthread_create(&serverthread, NULL, server_connection, rdata);
    pthread_detach(serverthread);
}

void* MotorControl(void *data) {
    // 모터 드라이브 관련 형식 받고 작성 예정
}