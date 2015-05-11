#encoding=utf-8

import serial, sys, struct

s=serial.Serial(sys.argv[1], 921600)
s.setRTS(True)
s.setDTR(True)
s.setRTS(False)

while 1:
	sys.stdout.buffer.write(s.read(1))
	sys.stdout.flush()
	
