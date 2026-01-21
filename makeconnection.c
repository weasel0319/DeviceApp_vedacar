#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "device_app.h"
// udp로 후에 바꿀 예정, 현 시점에서는 tcp로 구현, 시간 애매해서 tcp로 할수도
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
            fflush(stdout);
            sleep(1);
            continue;
        }
        int optval = 1;
        setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));   // 포트 옵션 변경
        printf("ssock: %d\n", ssock);
        fflush(stdout);
        if (connect(ssock, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) == -1) {
            perror("connect");
            fflush(stdout);
            exit(1);        // 기조를 못정해서 return이나 continue나 exit중 아무거나 씀
        }
        
        
        rdata->sd = ssock;
        makeSvThread(rdata);

        while (rdata->sd != -1) {
            sleep(1);
        }
    }                           
}