#!/bin/sh

USER=pi
PI_ADDRESS=192.168.2.148
SOURCE=$1
DESTINATION=$2

if [ -z $3 ]; then
   echo "user is $USER"
else
   USER=$3
fi

if [ -z $1 ]; then

   echo "usage: send2pi sourcefile destination [piuser] [address]"
   echo "default RaspberryPi address :$PI_ADDRESS"

else

   scp $SOURCE $USER@$PI_ADDRESS:$DESTINATION

fi


