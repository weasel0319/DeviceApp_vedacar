#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "device_app.h"

// 모터 제어 관련 함수
static int clamp(int v, int lo, int hi);
static void apply(int fd, mtVal tmp);

void makeThread(rxData *rdata) { // 나중에 추가 기능 생기면 함수 호출 식으로 변경할 예정
    int fd = open("/dev/tankmotor", O_RDWR);
    if (fd < 0) {
        perror("open(/dev/tankmotor)");
        fflush(stdout);
        fprintf(stderr, "Did you insmod tankmotor_drv.ko and create /dev/tankmotor?\n");
        exit(1);
    }
    rdata->mtfd = fd;
    pthread_t motorthread;

    if (sem_init(&rdata->semid, 0, 1) != 0){
        perror("sem_init failed");
        fflush(stdout);
    }
        
    pthread_create(&motorthread, NULL, MotorControl, rdata);
    pthread_detach(motorthread); 
}

void makeSvThread(rxData *rdata) { // 연결 수립시 생성되는 스레드
    // 현재 tcp로 서버와 연결 스레드 생성
    pthread_t serverthread;

    pthread_create(&serverthread, NULL, server_connection, rdata);
    pthread_detach(serverthread);
    // 01.14 rtsp 스레드 생성
    pthread_t rtsp_thread;
    
    pthread_create(&rtsp_thread, NULL, rtsp_connection, NULL); // rdata가 필요 없다면 NULL
    pthread_detach(rtsp_thread);
}

void* MotorControl(void *data) {    // 모터 제어 스레드
    rxData *rdata = (rxData *)data;
    int fd = rdata->mtfd;
    mtVal tmp = {0, 0};
    while (1) {
        sem_wait(&rdata->semid);
        if (rdata->mtstate == 1){
            tmp = rdata->mtval;
            rdata->mtstate = 0;
        }
        sem_post(&rdata->semid);

        apply(fd, tmp);
        usleep(100000);
        // 나중에 뭐 서버로부터 종료 명령 들어오면 break 하게끔 구현할지도? 일단 정상 종료시 멈추게끔
    }
    tmp.fb = 0; tmp.lr = 0;
    apply(fd, tmp);     // 정차시 멈춤
}

static void apply(int fd, mtVal tmp) {
    tmp.fb = clamp(tmp.fb, -100, 100);
    tmp.lr = clamp(tmp.lr, -100, 100);
    if (ioctl(fd, TANKMOTOR_IOCTL_SET, &tmp) < 0) {
        perror("ioctl(TANKMOTOR_IOCTL_SET)");
        fflush(stdout);
        return;
    }
    printf("APPLY: m1=%d m2=%d\n", tmp.fb, tmp.lr);
    fflush(stdout);
    return;
}

static int clamp(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}