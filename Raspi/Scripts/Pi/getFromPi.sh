#!/bin/sh

USER=$1
PI_ADDRESS=RaspiLAN
SOURCE=$2
DESTINATION=$3
if [ -z $1 ] 
then

   echo "usage: getFromPi piuser sourcefile destination"
   echo "RaspberryPi address : $PI_ADDRESS"

else

   scp $USER@$PI_ADDRESS:$SOURCE $DESTINATION

fi
