#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

#include "device_app.h"

void *server_connection(void *data) {
    rxData *rdata = (rxData *)data;
    int csock = rdata->sd, numbyte, save_size = 0;
    uint8_t readbuf[MAXDATASIZE]; 
    uint8_t savebuf[MAXDATASIZE * 2];

    while (1) {
        if ((numbyte = recv(csock, &readbuf, sizeof(readbuf), 0)) == -1) {
            perror("recv error");
            exit(1);
        }
        memcpy(savebuf + save_size, readbuf, numbyte);
        save_size += numbyte;

        while (savebuf[0] == 1 && save_size >= sizeof(recvPacket)) {    // id 확인 후 구조체 선정
            recvPacket *packet = (recvPacket *)savebuf;
            //memcpy(&packet.id, &savebuf[0], 1);
            //memcpy(&packet.size, &savebuf[1], 2);
            //memcpy(&packet.size, &savebuf[3], 2);  이렇게 짜려했는데 
            //memcpy(&packet.size, &savebuf[5], 2);  위처럼 해도 된다길래 일단 이렇게 만듬
            
            // crc 체크해서 잘못된 패킷이면 continue 
            calCRCCCITT(packet, numbyte);
            // 리턴값으로 할건 나중에 구현
            //
            save_size -= sizeof(recvPacket);
            memmove(savebuf, savebuf + sizeof(recvPacket), save_size);

            mtVal mttemp = { packet->fb, packet->lr };
            sem_wait(&rdata->semid);
            rdata->mtval = mttemp;
            sem_post(&rdata->semid);
        }
    }
}

uint16_t calCRCCCITT(recvPacket *packet, int len){
    uint16_t crc = 0xFFFF;
    uint8_t * data = (uint8_t *)packet;
    for (int i = 0; i < len; i++) {
        // 현재 CRC의 상위 바이트와 데이터를 XOR하여 인덱스를 찾음
        uint8_t index = (uint8_t)((crc >> 8) ^ data[i]);
        // 기존 CRC를 8비트 밀고 테이블 값을 섞음
        crc = (crc << 8) ^ crc16tab[index];
    }
    return crc;
}