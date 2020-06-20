#!/bin/bash
# monitor GPIO pin X for shutdown signal
# export GPIO pin X and set to input with pull-up

GPIOPIN=3

echo "$GPIOPIN" > /sys/class/gpio/export
echo "in" > /sys/class/gpio/gpio$GPIOPIN/direction

# wait for pin to go low
while [ true ] ; do
    if [ "$(cat /sys/class/gpio/gpio$GPIOPIN/value)" -eq '0' ] ; then
        echo "Raspberry Pi Shutting Down!"
        shutdown -h now
        exit 0
    fi
    sleep 1
done

