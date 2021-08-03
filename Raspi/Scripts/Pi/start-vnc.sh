#Server auf Raspi starten:
CLIENT_HOST=192.168.2.118
CLIENT_USER=rolf
ssh -NfL 5500:localhost:5500 $CLIENT_USER@$CLIENT_HOST sleep 2 && x11vnc -connect_or_exit localhost:5500 -scale 10/10
