import RPi.GPIO as GPIO
import os
import sys

pin = int(sys.argv[1])
state = sys.argv[2]

# GPIO setup
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(pin,GPIO.OUT)

if state is "1":
   GPIO.output(pin,GPIO.HIGH)
   print("LED is on")
else:
   GPIO.output(pin,GPIO.LOW)
   print("LED is off")

GPIO.cleanup()

