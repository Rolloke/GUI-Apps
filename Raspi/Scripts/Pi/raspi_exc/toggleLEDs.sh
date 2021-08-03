#!/bin/sh

toggle()
  {
  # Zustand des Pins einlesen
  LedVal=$(cat /sys/class/gpio/gpio$1/value)
  # LED toggeln
  if [ $LedVal -eq "0" ]; then
    LedVal="1"
  else
    LedVal="0"
  fi
  # Kontrollausgabe auf der Konsole
#  echo "Port $1 := $LedVal"
  # Pin auf 0 oder 1 setzen
  echo $LedVal > /sys/class/gpio/gpio$1/value
  }


while :
do
  # Taster einlesen und anzeigen
# for Port in  17 27
# do
#  LED=$(cat /sys/class/gpio/gpio${Port}/value)
#  echo "Port $Port = $LED"
# done

  # mit den LEDs blinken
#  for Port in 22 23 24 25
#  do

  echo 0 > /sys/class/gpio/gpio23/value
  sleep 0.00001  
  echo 1 > /sys/class/gpio/gpio23/value
  sleep 0.00001  
#    toggle "23"
#  done
#  echo ""
#  sleep 1
done
