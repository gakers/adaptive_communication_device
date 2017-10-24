# Speech.py
# If no sound, in the cmd line type : "start-pulseaudio-x11"

# Purpose: says the chosen Phrase
import sys
import pyttsx

def speak(phrase):
    Device = pyttsx.init()
    Device.say(phrase)
    Device.runAndWait()
