import serial
import time

port = serial.Serial(
	port="/dev/ttyAMA0", 
	baudrate = 7200,
	timeout = 2)

def readlineCR(port):
   rv = ""
   while True:
        ch = port.read()
        rv += ch
        if ch == '\r' or ch == '':
     	         return rv

while True: 
     rcv = readlineCR(port)
     port.write("I typed " + repr(rcv))
     print("Received: " + rcv)