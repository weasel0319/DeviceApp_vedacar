#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "device_app.h"
// udp로 후에 바꿀 예정, 현 시점에서는 tcp로 구현

void makeConnection(rxData *rdata, char **arg){
    int ssock;
    struct sockaddr_in servaddr;
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(arg[1]);
    servaddr.sin_port = htons(atoi(arg[2]));
    
    while (1) {
        ssock = socket(AF_INET, SOCK_STREAM, 0);
        if (ssock == -1) {
            perror("socket error");
            sleep(1);
            continue;
        }

        int optval = 1;
        setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));   // 포트 옵션 변경

        if (connect(ssock, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) == -1) {
            perror("connect");
            exit(1);
        }
        
        rdata->sd = ssock;
        makeSvThread(rdata);

        while (rdata->sd != -1) {
            sleep(1);
        }
        // todo: 통신 스레드 닫을 때 sd도 -1로 초기화 해줘야 함
        // close(ssock); 이거는 나중에 스레드에서 종료될때 닫는게 나을듯.
    }                           
}