from __future__ import print_function
import RPi.GPIO as GPIO
from flask import Flask, render_template, request
import datetime
import time
import syslog
from thread import start_new_thread


app = Flask(__name__)

GPIO.setmode(GPIO.BCM)

# time of switch off is 20 minutes
#                             day  sec micro milli minute hour week
add_time = datetime.timedelta(0,   0,  0,    0,    20,    0,   0)

syslog.syslog('started printer control')

# Create a dictionary called pins to store the pin number, name, and pin state:
pins = {
   17 : {'name' : 'Brother Laser', 'state' : GPIO.LOW, 'switch_off' : None, 'display_off' : '' },
   27 : {'name' : 'Canon IP 4700', 'state' : GPIO.LOW, 'switch_off' : None, 'display_off' : '' },
#   22 : {'name' : 'Steckdose' , 'state' : GPIO.LOW, 'switch_off' : None, 'display_off' : '' }
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
      # calculate switch off time and store in template:
      switch_off_time = datetime.datetime.now() + add_time
      display_off = switch_off_time.strftime("%H:%M:%S")
      pins[changePin]['display_off'] = display_off
      pins[changePin]['switch_off']  = switch_off_time
      message = "Turned " + deviceName + " on at "
      # start the survey thread for this pin
      start_new_thread(check_time, (changePin,))

   if action == "enhance":
      # enhance time for switching off and store in template
      switch_off_time = datetime.datetime.now() + add_time
      display_off = switch_off_time.strftime("%H:%M:%S")
      pins[changePin]['display_off'] = display_off
      pins[changePin]['switch_off']  = switch_off_time
      message = "Enhanced time for " + deviceName + " to "
 
   if action == "off":
      # switching off and store 'None' in template to stop thread
      pins[changePin]['display_off'] = ''
      GPIO.output(changePin, GPIO.LOW)
      pins[changePin]['switch_off']  = None
      message = "Turned " + deviceName + " off at "
   
   now = datetime.datetime.now()
   output = now.strftime("%H:%M:%S") 
   syslog.syslog( message + output)
   # For each pin, read the pin state and store it in the pins dictionary:
   for pin in pins:
      pins[pin]['state'] = GPIO.input(pin)

   # Along with the pin dictionary, put the message into the template data dictionary:
   templateData = {
      'pins' : pins
   }

   return render_template('main.html', **templateData)

def check_time(pin):

   while True:

      # get time to compare with
      time.sleep(5)
      now = datetime.datetime.now()
      output = now.strftime("%H:%M:%S") 

      if pins[pin]['switch_off'] is None:
         # time was emptied by switch off manually
         syslog.syslog('No switch of time for: ' + pins[pin]['name'] )
         break
      else:
         if now > pins[pin]['switch_off']:
            # timeout occurred, switch off
            GPIO.output(pin, GPIO.LOW)
            pins[pin]['switch_off'] = None
            break

   syslog.syslog('switched off: ' + pins[pin]['name'] + ' at ' + output )

   return 0

if __name__ == "__main__":

   app.run(host='0.0.0.0', port=8080, debug=False)
   


