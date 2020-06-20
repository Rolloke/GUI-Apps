#!/bin/bash
# monitor cpu temperature
# export GPIO pin 2 and 
# switch fan on, if too hot
# switch fan off, if cold enough

GPIOPIN=2
TEMP_FOR_FAN=43
THRESHOLD=4
FAN_ON=0

# export pin
echo "$GPIOPIN" > /sys/class/gpio/export
# set direction
echo "out" > /sys/class/gpio/gpio$GPIOPIN/direction
# switch of fan
echo "0" > /sys/class/gpio/gpio$GPIOPIN/value

while [ true ] ; do
   TEMPERATURE=$(vcgencmd measure_temp)
   TEMP=$( echo "$TEMPERATURE" | sed -n 's/.*\([0-9][0-9]\).*/\1/p' )
   # echo "temperature is $TEMP"
   if [ $FAN_ON -eq 0 ] && [ $TEMP -gt $(($TEMP_FOR_FAN+$THRESHOLD)) ] ; then
        # echo "switch on fan"
        echo "1" > /sys/class/gpio/gpio$GPIOPIN/value
        FAN_ON=1
   elif [ $FAN_ON -eq 1 ] && [ $TEMP -lt $(($TEMP_FOR_FAN-$THRESHOLD)) ] ; then
        # echo "switch off fan"
        echo "0" > /sys/class/gpio/gpio$GPIOPIN/value
        FAN_ON=0
   fi
   sleep 1
done
