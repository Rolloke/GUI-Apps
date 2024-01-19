#!/bin/sh

USER=$1
PI_ADDRESS=192.168.2.193
SOURCE=$2
DESTINATION=$3

if [ -z $1 ] 
then

   echo "usage: send2pi piuser sourcefile destination"
   echo "RaspberryPi address :"

else

   scp -r $SOURCE $USER@$PI_ADDRESS:$DESTINATION

fi


