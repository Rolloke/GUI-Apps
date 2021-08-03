#!/bin/bash
# monitor GPIO pin X for shutdown signal
# export GPIO pin X and set to input with pull-up

PIN_BTN_SHUTDOWN=3
PIN_BTN_DISPLAY=27
PIN_LED_STATE=19

LED_STATE=1

if [ -e "/sys/class/gpio/gpio$PIN_BTN_SHUTDOWN" ]; then
   echo "/sys/class/gpio/gpio$PIN_BTN_SHUTDOWN exists"
else
   echo "$PIN_BTN_SHUTDOWN" > /sys/class/gpio/export
   echo "in" > /sys/class/gpio/gpio$PIN_BTN_SHUTDOWN/direction
fi

if [ -e "/sys/class/gpio/gpio$PIN_BTN_DISPLAY" ]; then
   echo "/sys/class/gpio/gpio$PIN_BTN_DISPLAY exists"
else
   echo "$PIN_BTN_DISPLAY" > /sys/class/gpio/export
   echo "in" > /sys/class/gpio/gpio$PIN_BTN_DISPLAY/direction
fi

if [ -e "/sys/class/gpio/gpio$PIN_LED_STATE" ]; then
   echo "/sys/class/gpio/gpio$PIN_LED_STATE exists"
else
   echo "$PIN_LED_STATE" > /sys/class/gpio/export
   echo "out" > /sys/class/gpio/gpio$PIN_LED_STATE/direction
fi

sleep 1
echo "$LED_STATE" > /sys/class/gpio/gpio$PIN_LED_STATE/value


logger "start survey GPIO$PIN_BTN_SHUTDOWN for shutdown as $USER"

# wait for pin to go low
while [ true ] ; do
   if [ "$(cat /sys/class/gpio/gpio$PIN_BTN_SHUTDOWN/value)" -eq '0' ] ; then
      logger "User $USER shutting down Raspi by gpio pin$PIN_BTN_SHUTDOWN!"
      for i in {1..10}
      do
         echo "0" > /sys/class/gpio/gpio$PIN_LED_STATE/value
         sleep 0.1
         echo "1" > /sys/class/gpio/gpio$PIN_LED_STATE/value
         sleep 0.1
      done
      shutdown -h now
      exit 0
   fi
   if [ "$(cat /sys/class/gpio/gpio$PIN_BTN_DISPLAY/value)" -eq '1' ] ; then
      logger "Display button pressed, LED: $LED_STATE"
      echo "$LED_STATE" > /sys/class/gpio/gpio$PIN_LED_STATE/value
      if [ "$LED_STATE" -eq '1' ] ; then
         LED_STATE=0
      else
         LED_STATE=1
      fi
   fi
   sleep 1
done

