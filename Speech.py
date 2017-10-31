# Speech.py
# If no sound, in the cmd line type : "start-pulseaudio-x11"


# Purpose: says the chosen Phrase
import sys, getopt
import pyttsx

def speak(phrase):
    Device = pyttsx.init()
    Device.say(phrase)
    Device.runAndWait()

def main(argv):
    speak(sys.argv[1])
    return
if __name__ == '__main__': #main(argv)
    main(sys.argv[1:])
