#!/bin/bash
# monitor GPIO pin X for shutdown signal
# export GPIO pin X and set to input with pull-up

# monitored shutdown pin for on/off switch
PIN_BTN_SHUTDOWN=3
# monitored display pin for on/off HMI-display
PIN_BTN_DISPLAY=27
# controlled LED pin to display button reaction
# on/off for display on off
# blinking for shutdown
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

sleep 2
echo "$LED_STATE" > /sys/class/gpio/gpio$PIN_LED_STATE/value


# wait for pin to go low
while [ true ] ; do
   if [ "$(cat /sys/class/gpio/gpio$PIN_BTN_SHUTDOWN/value)" -eq '0' ] ; then
      logger "User $USER shutting down Raspi by gpio pin$PIN_BTN_SHUTDOWN!"
      echo "0" > /sys/class/gpio/gpio$PIN_LED_STATE/value
      sleep 0.5
      echo "1" > /sys/class/gpio/gpio$PIN_LED_STATE/value
      sleep 0.5
      echo "0" > /sys/class/gpio/gpio$PIN_LED_STATE/value
      sleep 0.5
      echo "1" > /sys/class/gpio/gpio$PIN_LED_STATE/value
      sleep 0.5
      shutdown -h now
      exit 0
   fi
   if [ "$(cat /sys/class/gpio/gpio$PIN_BTN_DISPLAY/value)" -eq '1' ] ; then
      if [ "$LED_STATE" -eq '1' ] ; then
         LED_STATE=0
         vcgencmd display_power 0
         /opt/vc/bin/tvservice -o
      else
         LED_STATE=1
         vcgencmd display_power 1
         /opt/vc/bin/tvservice -p
         /bin/chvt 6
         /bin/chvt 7
      fi
      echo "$LED_STATE" > /sys/class/gpio/gpio$PIN_LED_STATE/value
   fi
   sleep 1
done

