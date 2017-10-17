""" gpio_in.py """
#reads the gpio pin 

import_result = ''

try:
    import RPi.GPIO as gpio
except ImportError:
    print("No module named RPi.GPIO.")
else:
    import_result = 'OK'

import os
#from pygame import mixer
#import Queue
import sys
#from threading import Thread
import time

#current debounce time
wait_ms = 800

#assigning GPIO numbers to their button's function. Currently using GPIO2,GPIO3,GPIO4,GPIO17,GPIO27 which correspond to pins 3, 5, 7, 11,13
up=2
down=3
right=4
left=17
select=27

#Config GPIO ports to be inputs.
GPIO.setmode(GPIO.BCM)
GPIO.setup(up, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(down, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(right, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(left, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(select, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

#temp test functions through printing
def upPushed():
    print 'up'
def downPushed():
    print 'down'
def rightPushed():
    print 'right'
def leftPushed():
    print 'left'
def selectPushed():
    print 'select'

#interupts for each button press on the rising edge
GPIO.add_event_detect(up, GPIO.RISING, callback=upPushed, bouncetime=wait_ms)
GPIO.add_event_detect(down, GPIO.RISING, callback=downPushed, bouncetime=wait_ms)
GPIO.add_event_detect(right, GPIO.RISING, callback=rightPushed, bouncetime=wait_ms)
GPIO.add_event_detect(left, GPIO.RISING, callback=leftPushed, bouncetime=wait_ms)
GPIO.add_event_detect(select, GPIO.RISING, callback=selectPushed, bouncetime=wait_ms)

#wait for event
do_something()

