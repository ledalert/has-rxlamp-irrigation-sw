#encoding=utf-8

import v3, colorsys





import serial, sys, time, struct,  threading
from math import *

s=serial.Serial(sys.argv[1], 1000000)
s.setDTR(True)
s.setRTS(True)
s.setRTS(False)

tau = pi*2


leds = 14

time.sleep(0.1)
r, g, b = 0.0, 0.0, 0.0
h=0
a=0
a2=1.4
while 1:

	h = (h + 0.001) % 1.0
	a = (a + sin(time.time())*0.01) % tau
	a2 = (a2 + sin(time.time())*0.015) % tau
	ii = sin(time.time()*.2)*.5+.5

	for i in range(leds):				
		v= (sin(a+i)*sin(a2+i*.35)*.4+.4)**2.0
		s.write(bytearray(round(v * 255) for v in colorsys.hsv_to_rgb((h+i*0.05*ii) % 1.0, 1.0, v)))

	s.flush()

	#time.sleep(0.01)
