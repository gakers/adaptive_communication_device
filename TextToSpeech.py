import sys, getopt
import pyttsx

def speak(phrase):
	Device = pyttsx.init()
	Device.say(phrase)
	Device.runAndWait()

def main(argv):
	speak(sys.argv[1])
	return
if __name__ == '__main__':
	main(sys.argv[1:])
