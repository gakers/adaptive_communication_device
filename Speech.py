# Speech.py

# Purpose: says the chosen Phrase

import pyttsx

def speak(phrase):
    Device = pyttsx.init()
    Device.say(phrase)
    Device.runAndWait()
