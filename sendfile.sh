#!/bin/bash
# ./scp.sh filename    ping test, $? == 0    do scp

IP=192.168.0.12 
PI=tank

ping -c 3 $IP >/dev/null 2>&1

if [ $? -eq 0 ]
then
	echo "send $1 to raspberrypi"
	scp device_app $PI@$IP:/home/$PI
    scp setup.sh $PI@$IP:/home/$PI
    scp tank.service $PI@$IP:/home/$PI
    scp wificonnect.sh $PI@$IP:/home/$PI
	echo "completed"
else
	echo "can NOT connect to raspberrypi"
	echo "please check your Network Connection"
fi