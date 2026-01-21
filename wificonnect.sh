#!/bin/bash

SSID="tank"
SSIDPW="tank1234"

CURRENT_WIFI=$(nmcli -t -f active,ssid dev wifi | grep '^yes' | cut -d: -f2)

if [ "$CURRENT_WIFI" == "$SSID" ]; then
    echo "이미 $SSID 에 연결되어 있습니다."
    exit 0
fi

nmcli device wifi list | grep "$SSID"

if [ $? -eq 0 ]
then
    echo "wifi found"
    sudo nmcli device wifi connect "$SSID" password "$SSIDPW"
    exit 0
else
    echo "can NOT find $SSID"
    echo "please check your Network Connection"
    exit 1
fi