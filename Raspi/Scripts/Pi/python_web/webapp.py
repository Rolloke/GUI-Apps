from __future__ import print_function
import RPi.GPIO as GPIO
from flask import Flask, render_template, request
import datetime
import time
from thread import start_new_thread


app = Flask(__name__)

GPIO.setmode(GPIO.BCM)

now = datetime.datetime.now()
timeString = now.strftime("%H:%M")


# Create a dictionary called pins to store the pin number, name, and pin state:
pins = {
   18 : {'name' : 'Brother Laser', 'state' : GPIO.LOW, 'switch_off' : None, 'display_off' : '' },
   23 : {'name' : 'Canon IP 4700', 'state' : GPIO.LOW, 'switch_off' : None, 'display_off' : '' } 
   }


# Set each pin as an output and make it low:
for pin in pins:
   GPIO.setup(pin, GPIO.OUT)
   GPIO.output(pin, GPIO.LOW)


@app.route("/")
def main():
   # For each pin, read the pin state and store it in the pins dictionary:
   for pin in pins:
      pins[pin]['state'] = GPIO.input(pin)
   # Put the pin dictionary into the template data dictionary:
   templateData = {
      'pins' : pins
      }
   # Pass the template data into the template main.html and return it to the user

   return render_template('main.html', **templateData)

# The function below is executed when someone requests a URL with the pin number and action in it:
@app.route("/<changePin>/<action>")
def action(changePin, action):
   # Convert the pin from the URL into an integer:
   changePin = int(changePin)
   # Get the device name for the pin being changed:
   deviceName = pins[changePin]['name']
   # If the action part of the URL is "on," execute the code indented below:
   if action == "on":
      # Set the pin high:
      GPIO.output(changePin, GPIO.HIGH)
      # Save the status message to be passed into the template:
      switch_off_time = datetime.datetime.now() + datetime.timedelta(0, 20, 0, 0, 0, 0, 0)
      display_off = switch_off_time.strftime("%H:%M:%S")
      pins[changePin]['display_off'] = display_off
      pins[changePin]['switch_off']  = switch_off_time
      message = "Turned " + deviceName + " on."
      start_new_thread(check_time, (changePin,))

   if action == "off":
      pins[changePin]['display_off'] = ''
      GPIO.output(changePin, GPIO.LOW)
      message = "Turned " + deviceName + " off."

   # For each pin, read the pin state and store it in the pins dictionary:
   for pin in pins:
      pins[pin]['state'] = GPIO.input(pin)

   # Along with the pin dictionary, put the message into the template data dictionary:
   templateData = {
      'pins' : pins,
      'starttime' : timeString
   }

   return render_template('main.html', **templateData)

def check_time(pin):

   #print( 'entering thread for pin: ', pin ) 
   while True:

      time.sleep(5)
      
      now = datetime.datetime.now()
      output = now.strftime("%H:%M:%S") 
      # print( 'checking at ', output ) 

      if pins[pin]['switch_off'] is None:
         print( 'No switch of time for: ', pin )
         break
      else:
         # print( 'pin off: ', pins[pin]['display_off'] )
         if now > pins[pin]['switch_off']:
            GPIO.output(pin, GPIO.LOW)
            print( 'pin off: ', pins[pin]['display_off'] )
            pins[pin]['switch_off'] = None
            break

   #print( 'leaving thread for pin: ', pin )
#   templateData =  { 'pins' : pins  }
#   return render_template('main.html', **templateData)
   return 0

if __name__ == "__main__":

   app.run(host='0.0.0.0', port=8080, debug=False)
   


