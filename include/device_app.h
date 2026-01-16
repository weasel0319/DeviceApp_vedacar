#ifndef _DEVICEAPP_H
#define _DEVICEAPP_H

#include <semaphore.h>
#include <stdint.h>
#include <linux/ioctl.h>

#define TANKMOTOR_IOC_MAGIC  'T'
#define TANKMOTOR_IOCTL_SET  _IOW(TANKMOTOR_IOC_MAGIC, 0x01, mtVal)

#define MAXDATASIZE 255

typedef struct{
    uint8_t     id;       // id   1 Bytes
    uint16_t    size;     // size 2 Bytes
    int8_t     fb;       // data 2 Bytes
    int8_t     lr;
    uint16_t    CRC16;     // CRC16 2 Bytes
    // 통신 프로토콜 정해지면 마저 작성
}__attribute__((packed)) recvPacket;

typedef struct{
    int8_t fb;
    int8_t lr;
} mtVal;

typedef struct{
    sem_t semid;
    int sd;
    mtVal mtval;
    int mtfd; 
    uint8_t mtstate;
} rxData;

// 제어 스레드 함수 
void *MotorControl(void *);

// 서버 스레드 함수
void *server_connection(void *);
// rtsp 서버 스레드 함수
void *rtsp_connection(void *);

// 기타 함수
void makeThread(rxData *rdata);
void makeSvThread(rxData *rdata);
void makeConnection(rxData *, char **arg);
#endif