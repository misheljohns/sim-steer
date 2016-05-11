import time
import serial

# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
	port='/dev/ttyAMA0',
#	port='/dev/ttyUSB0',
	baudrate=115200,
#	parity=serial.PARITY_ODD,
#	stopbits=serial.STOPBITS_TWO,
#	bytesize=serial.SEVENBITS
)

#ser.open()
print ser.isOpen()

print 'Enter your commands below.\r\nInsert "exit" to leave the application.'

input=1
while 1 :
	# get keyboard input
	input = raw_input(">> ")
        # Python 3 users
        # input = input(">> ")
	if input == 'exit':
		ser.close()
		exit()
	else:
		# send the string
		ser.write(input + '\n')
		out = ''
		# let's wait one second before reading output (let's give device time to answer)
		time.sleep(1)
		while ser.inWaiting() > 0:
			out += ser.read(1)
			
		if out != '':
			print ">> " + out