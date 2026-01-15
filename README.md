# DeviceApp
tank 프로젝트에서 라즈베리 파이 제어 및 통신 수신부, rtsp 서버를 담당하는 코드입니다.

## 주요 기술 스택
- **언어**: C (C11)
- **통신**: TCP/IP 소켓 프로그래밍, RTSP
- **병렬 처리**: Semaphore (동기화)
- **필요 하드웨어, 라이브러리**: Raspberry Pi 4, GStreamer Library
## 설치 방법

1. scp -r ../DeviceApp rpiname@<ip>:~ 터미널에 입력
2. 라즈베리 파이에서 DeviceApp 폴더로 이동 후

sudo apt update

sudo apt install -y \
    build-essential \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    libgstrtspserver-1.0-dev \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-tools \
    gstreamer1.0-x \
    gstreamer1.0-alsa \
    gstreamer1.0-gl \
    gstreamer1.0-libav

sudo apt install -y libgstrtspserver-1.0-0

sudo apt install -y gstreamer1.0-plugins-ugly

make

수행 후 device_app이 생성되었는지 확인

3. 디바이스 드라이버 쪽 파일도 진행 후  
./device_app <ip 주소> 포트       
로 실행

