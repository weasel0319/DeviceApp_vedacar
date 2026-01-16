#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "device_app.h"

uint16_t calCRCCCITT_raw(unsigned char *data, int len);

uint16_t crc16tab[256] = {
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50A5,0x60C6,0x70E7,
    0x8108,0x9129,0xA14A,0xB16B,0xC18C,0xD1AD,0xE1CE,0xF1EF,
    0x1231,0x0210,0x3273,0x2252,0x52B5,0x4294,0x72F7,0x62D6,
    0x9339,0x8318,0xB37B,0xA35A,0xD3BD,0xC39C,0xF3FF,0xE3DE,
    0x2462,0x3443,0x0420,0x1401,0x64E6,0x74C7,0x44A4,0x5485,
    0xA56A,0xB54B,0x8528,0x9509,0xE5EE,0xF5CF,0xC5AC,0xD58D,
    0x3653,0x2672,0x1611,0x0630,0x76D7,0x66F6,0x5695,0x46B4,
    0xB75B,0xA77A,0x9719,0x8738,0xF7DF,0xE7FE,0xD79D,0xC7BC,
    0x48C4,0x58E5,0x6886,0x78A7,0x0840,0x1861,0x2802,0x3823,
    0xC9CC,0xD9ED,0xE98E,0xF9AF,0x8948,0x9969,0xA90A,0xB92B,
    0x5AF5,0x4AD4,0x7AB7,0x6A96,0x1A71,0x0A50,0x3A33,0x2A12,
    0xDBFD,0xCBDC,0xFBBF,0xEB9E,0x9B79,0x8B58,0xBB3B,0xAB1A,
    0x6CA6,0x7C87,0x4CE4,0x5CC5,0x2C22,0x3C03,0x0C60,0x1C41,
    0xEDAE,0xFD8F,0xCDEC,0xDDCD,0xAD2A,0xBD0B,0x8D68,0x9D49,
    0x7E97,0x6EB6,0x5ED5,0x4EF4,0x3E13,0x2E32,0x1E51,0x0E70,
    0xFF9F,0xEFBE,0xDFDD,0xCFFC,0xBF1B,0xAF3A,0x9F59,0x8F78,
    0x9188,0x81A9,0xB1CA,0xA1EB,0xD10C,0xC12D,0xF14E,0xE16F,
    0x1080,0x00A1,0x30C2,0x20E3,0x5004,0x4025,0x7046,0x6067,
    0x83B9,0x9398,0xA3FB,0xB3DA,0xC33D,0xD31C,0xE37F,0xF35E,
    0x02B1,0x1290,0x22F3,0x32D2,0x4235,0x5214,0x6277,0x7256,
    0xB5EA,0xA5CB,0x95A8,0x8589,0xF56E,0xE54F,0xD52C,0xC50D,
    0x34E2,0x24C3,0x14A0,0x0481,0x7466,0x6447,0x5424,0x4405,
    0xA7DB,0xB7FA,0x8799,0x97B8,0xE75F,0xF77E,0xC71D,0xD73C,
    0x26D3,0x36F2,0x0691,0x16B0,0x6657,0x7676,0x4615,0x5634,
    0xD94C,0xC96D,0xF90E,0xE92F,0x99C8,0x89E9,0xB98A,0xA9AB,
    0x5844,0x4865,0x7806,0x6827,0x18C0,0x08E1,0x3882,0x28A3,
    0xCB7D,0xDB5C,0xEB3F,0xFB1E,0x8BF9,0x9BD8,0xABBB,0xBB9A,
    0x4A75,0x5A54,0x6A37,0x7A16,0x0AF1,0x1AD0,0x2AB3,0x3A92,
    0xFD2E,0xED0F,0xDD6C,0xCD4D,0xBDAA,0xAD8B,0x9DE8,0x8DC9,
    0x7C26,0x6C07,0x5C64,0x4C45,0x3CA2,0x2C83,0x1CE0,0x0CC1,
    0xEF1F,0xFF3E,0xCF5D,0xDF7C,0xAF9B,0xBFBA,0x8FD9,0x9FF8
};

void *server_connection(void *data) {
    rxData *rdata = (rxData *)data;
    int csock = rdata->sd, numbyte, save_size = 0;
    char readbuf[MAXDATASIZE]; 
    char savebuf[MAXDATASIZE * 2];
    printf("connect start\n");
    while (1) {
        printf("recv bef\n");
        if ((numbyte = recv(csock, &readbuf, sizeof(readbuf), 0)) == -1) {
            perror("recv error");
            exit(1);
        }
        printf("%s\n", readbuf);
        memcpy(savebuf + save_size, readbuf, numbyte);
        save_size += numbyte;
        switch(savebuf[0]) {
        case 0:
            // 일단 만약에 종료 로직 만들어지면 여기서 나가는 로직 짤듯
        case 1:
            while (save_size >= sizeof(recvPacket)) {    // id 확인 후 구조체 선정
                printf("Raw 7 Bytes: ");
                for(int i=0; i<7; i++) printf("%02x ", (unsigned char)savebuf[i]);
                printf("\n");

                uint16_t calculatedCRC = calCRCCCITT_raw((unsigned char *)&savebuf[0], 5);
                uint16_t receivedCRC = ((unsigned char)savebuf[5] << 8) | (unsigned char)savebuf[6];
                printf("Calculated: %u, Received: %u\n", calculatedCRC, receivedCRC);
                if (calculatedCRC != receivedCRC) {
                    printf("CRC error\n");
                    save_size -= sizeof(recvPacket);
                    memmove(savebuf, savebuf + sizeof(recvPacket), save_size);
                    continue;
                }
                
                recvPacket packet;

                memcpy(&packet.id, &savebuf[0], 1);
                memcpy(&packet.size, &savebuf[1], 2);
                memcpy(&packet.fb, &savebuf[3], 1); 
                memcpy(&packet.lr, &savebuf[4], 1);  
                memcpy(&packet.CRC16, &savebuf[5], 2);  

                save_size -= sizeof(recvPacket);
                memmove(savebuf, savebuf + sizeof(recvPacket), save_size);
                
                packet.size = ntohs(packet.size);

                printf("packet id : %d\n", packet.id);
                printf("packet size : %d\n", packet.size);
                printf("packet data1 : %d\n", packet.fb);
                printf("packet data2 : %d\n", packet.lr);
                printf("packet CRC : %d\n", packet.CRC16);

                mtVal mttemp = { packet.fb, packet.lr };
                //sem_wait(&rdata->semid);
                rdata->mtval = mttemp;
                rdata->mtstate = 1;
                //sem_post(&rdata->semid);
                printf("end\n");
            }
            break;
        // 추가적인 데이터 패킷 정해지면 추가
        default:    // id가 정해진 값이 아닐때 한바이트씩 밀기
            /*save_size--;
            memmove(savebuf, savebuf + 1, save_size);
            break;*/
        }
    }
    // 만약에 사용자에서 종료 로직? 들어오면 fd랑 정리하는거 로직 짜야함
    rdata->sd = -1; // 연결 종료를 알림
    close(csock);
}

uint16_t calCRCCCITT_raw(unsigned char *data, int len){
    uint16_t crc = 0xFFFF;
    for (int i = 0; i < len; i++) {
        // 현재 CRC의 상위 바이트와 데이터를 XOR하여 인덱스를 찾음
        uint8_t index = (uint8_t)((crc >> 8) ^ data[i]);
        // 기존 CRC를 8비트 밀고 테이블 값을 섞음
        crc = (crc << 8) ^ crc16tab[index];
    }
    return crc;
}