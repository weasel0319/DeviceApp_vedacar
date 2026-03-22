DeviceApp 프로젝트 상세 요약
이 문서는 Raspberry Pi를 이용한 무선 조종 탱크(Tank)의 제어, 영상 스트리밍(RTSP), 그리고 통신 수신을 담당하는 DeviceApp 프로젝트에 대한 전반적인 구조와 역할을 요약한 문서입니다.

🚀 1. 프로젝트 주요 기능 및 스펙
주 언어: C (C11)
주요 기능:
서버와의 TCP 소켓 통신 (제어 명령 수신).
수신된 명령의 무결성 검증 (CRC16 CCITT).
모터 제어를 위한 디바이스 드라이버 명령 전달.
라즈베리 파이 카메라를 활용한 GStreamer 기반 RTSP 영상 스트리밍.
주변 환경 지원:
systemd 를 통한 백그라운드 서비스 등록 및 자동 재시작.
NetworkManager (nmcli) 기반 자동 와이파이 연결 쉘 스크립트.
📂 2. 주요 파일 및 디렉터리 역할
프로젝트 디렉터리는 바이너리 빌드를 위한 소스코드와 편의용 스크립트로 분리되어 있습니다.

2.1 주요 소스 코드 (.c, .h)

device_app.c

역할: 응용 프로그램의 메인 진입점.
동작: 시작 시 

wificonnect.sh
 스크립트를 호출하여 와이파이 연결을 확인한 뒤, 메모리상 구조체(rxData)를 할당하고 스레드 생성(

makeThread
) 및 서버 통신(

makeConnection
)을 차례로 호출하여 애플리케이션의 핵심 루틴을 기동시킵니다.

makeconnection.c
 / 

serverconnection.c

역할: 소켓 프로그래밍 및 제어 패킷의 파싱·검증.
동작:

makeConnection
: 매개변수로 주어진 IP(arg[1])와 Port(arg[2])로 TCP 소켓을 생성(TCP Client)하여 원격 서버로 연결을 시도합니다. 연결이 수립되면 백그라운드 스레드들(

makeSvThread
)을 기동합니다.

server_connection
: 서버에서 수신되는 패킷을 읽어 그 유효성을 CRC16 방식으로 검증한 후, fb(앞/뒤), lr(좌/우) 명령으로 분리하여 모터 제어 스레드와 공유하는 메모리(rdata->mtval)에 할당합니다.

makethread.c

역할: 멀티 스레드 환경 구성 (모터 제어, 시스템 스레드 연결).
동작:
모터 제어 스레드(

MotorControl
): 공유 메모리에 전달된 새로운 mtval(모터 이동 명령)이 있는지 확인(sem_wait 세마포어 적용)합니다. 명령이 있으면 ioctrl로 /dev/tankmotor 디바이스에 값을 쓰며(write) 라즈베리 파이의 핀을 제어합니다.

rtsp_connection.c

역할: GStreamer 및 gst-rtsp-server 기반 RTSP 송출 모듈.
동작: 라즈베리 파이 카메라(/dev/video0) 하드웨어 인코더 영상을 읽어(v4l2src), RTSP Server(rtsp://0.0.0.0:8554/live)로 스트리밍합니다. Main Loop를 돌며 스레드 내에 상주합니다.

include/device_app.h

역할: 공통 자료구조와 함수 원형 선언.
동작: 패킷 데이터 구조인 recvPacket, 각 스레드 간 자원 공유를 위한 rxData 구조체, /dev/tankmotor 접근을 위한 매크로 등이 정의되어 있습니다.
2.2 설정 및 유틸리티 스크립트 (.sh, .service, Makefile)

Makefile
기능: 교차 컴파일용(aarch64-linux-gnu-gcc) 빌드 설정 파일. GStreamer와 Glib 등의 외부 의존성이 기술되어 있어, 

make
 명령어 하나로 

device_app
 실행 파일을 빌드합니다.

wificonnect.sh
기능: 부팅 단계(또는 앱 실행 직전)에서, 지정된 SSID("tank")와 패스워드로 라즈베리 파이가 무선 네트워크에 자동 연결되게끔 지원하는 유틸리티 스크립트.

tank.service
 & 

setup.sh
기능: 리눅스 시스템 데몬 구성. 

setup.sh
으로 스크립트 및 바이너리 권한 부여 후 서비스를 등록하며, 

tank.service
 파일 내에서 라즈베리파이 부팅 시 ./device_app 192.168.137.1 25000 형태로 장비가 클리어하게 실행되도록 systemd 서비스 설정을 정의합니다 (장치가 끊겨도 Restart=always 속성에 따라 자동으로 복구됩니다).

sendfile.sh
기능: 개발 환경에서 테스트가 용이하도록 컴파일된 실행 파일과 스크립트들을 scp 명령어를 이용해 라즈베리 파이 타겟 장치로 간편하게 원격 배포하는 기능입니다.
🛠 3. 애플리케이션 실행 흐름 요약
부팅 후 systemd가 

tank.service
 시작.
애플리케이션 메인(

device_app.c
) 시작 전 모터 커널 모듈 적재 대기.

wificonnect.sh
 스크립트를 통해 지정 와이파이에 자동 접속 시도.

makeThread
 가 모터 제어용 백그라운드 스레드를 대기 상태로 활성화.
TCP 소켓으로 서버에 접속한 후 메인 통신 연결이 확립되면, 스레드(서버 패킷 수신 스레드, RTSP 송출 스레드)들이 백그라운드 동작 시작.
서버로부터 제어명령(5Bytes 헤더) 패킷을 받으면 CRC 검증 후 rdata 구조체를 통해 모터 제어 스레드로 명령을 넘기고 ioctl()로 탱크 모터를 컨트롤함과 동시에 RTSP 스레드는 별도로 끊김없이 지속적으로 카메라 영상을 8554 포트 통신으로 스트림합니다.
💡 종합 의의
DeviceApp은 하드웨어 종속적인 모터 제어 및 영상 송출과 더불어 서버와의 TCP 통신을 Pthreads를 활용해 멀티 스레드로 깔끔히 분리·제어하는 프로그램입니다. 무선 연결부터 서비스 데몬화까지 고려하여, 전원이 인가되었을 때 사용자 개입 없이 스스로 동작할 수 있는 우수한 임베디드 장치용 소프트웨어 구조를 지니고 있습니다.
