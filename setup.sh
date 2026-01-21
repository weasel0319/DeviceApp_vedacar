#!/bin/bash
echo "tank service 등록 및 실행"

# 1. 실행 권한 부여
sudo chmod +x ./wificonnect.sh
sudo chmod +x ./device_app

# 2. 서비스 파일 복사 (현재 디렉토리에 tank.service가 있다고 가정)
sudo cp ./tank.service /etc/systemd/system/

# 3. 서비스 활성화
sudo systemctl daemon-reload
sudo systemctl enable tank.service
sudo systemctl restart tank.service

echo "서비스 등록 완료."