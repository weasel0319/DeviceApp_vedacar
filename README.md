# DeviceApp
tank 프로젝트에서 라즈베리 파이 제어 및 통신 수신부, rtsp 서버를 담당하는 코드입니다.

## 주요 기술 스택
- **언어**: C (C11)
- **통신**: TCP/IP 소켓 프로그래밍, RTSP
- **병렬 처리**: Semaphore (동기화)
- **필요 하드웨어, 라이브러리**: Raspberry Pi 4, GStreamer Library
## 설치 방법

1. 라즈베리파이로 이동
2. 라즈베리 파이에서 아래 명령어 수행

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

3. 디바이스 드라이버 쪽 파일도 진행 후
vi(m) / nano sendfile.sh 
IP, PI 본인 환경에 맞게 수정

vi(m) / nano wificonnect.sh
SSID, SSIDPW 와이파이 환경에 맞게 수정

vi(m) / nano tank.service
/suseok 부분 본인 경로에 맞게 수정

./sendfile.sh 실행
라즈베리 파이로 4개의 파일 전부 전송 되었는지 확인
라즈베리 파이로 이동 후 setup.sh 실행
journalctl -f -u tank.service
를 통해 제대로 서비스 동작하고 있는지 확인   


서비스 종료 및 앱 종료 명령어
sudo systemctl stop tank.service
sudo pkill -9 device_app
ps -ef | grep device_app | grep -v grep 
ps 수행시 안나옴

재시작 명령어
sudo systemctl daemon-reload
sudo systemctl restart tank.service