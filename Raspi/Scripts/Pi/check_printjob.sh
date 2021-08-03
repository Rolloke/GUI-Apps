#!/bin/sh
# monitor printer queue
# export GPIO pin 18 and 
# switch printer on, if a print job is started
# switch printer off, after a while
INTERVAL=1
GPIOPIN=18
QUATER_MINUTES_ON=4

# export pin
echo "$GPIOPIN" > /sys/class/gpio/export
# set direction
echo "out" > /sys/class/gpio/gpio$GPIOPIN/direction
# switch of fan
echo "0" > /sys/class/gpio/gpio$GPIOPIN/value

while [ 1 ]
do
        if [ `lpstat -o | wc -l` != 0 ]
        then
                # power on command
                echo "1" > /sys/class/gpio/gpio$GPIOPIN/value
                logger "switch on printer"
                i=0
                while [ $i -le $QUATER_MINUTES_ON ]
                do
                        # testing print queue
                        if [ `lpstat -o | wc -l` != 0 ]
                        then
                                i=0     #reset standby counter
                                while [ `lpstat -o | wc -l` != 0 ]
                                do
                                        echo printing   #just for testing
                                        sleep $INTERVAL #check every x s if still printing
                                done
                        else
                                i=`expr $i + 1`
                                sleep 15        #update counter every quater of a minute without printjob
                        fi
                done
                echo "0" > /sys/class/gpio/gpio$GPIOPIN/value
                logger "switch off printer"
                #echo off        #insert your power off command

        else
                echo nothing    #just for testing
                sleep $INTERVAL #check for incoming printjob every x s
        fi
done
