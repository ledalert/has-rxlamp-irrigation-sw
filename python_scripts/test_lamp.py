import serial, time, random, math, sys
s=serial.Serial("/dev/ttyUSB0", 1000000)
s.setRTS(True)
s.setDTR(True)
s.setRTS(False)

time.sleep(0.01)


freq = 100
delay = 1.0/freq


black = "000000".decode("hex")
green = "00FF00".decode("hex")
red = "FF0000".decode("hex")
blue = "0000FF".decode("hex")
white = "FFFFFF".decode("hex")



def send_led_data(data):
	s.write( data )
	s.flush()
	time.sleep(delay)





	#s.write( ''.join([chr(int ((random.random() ** math.e)*255 )) for c in range(42 * 3)] ))


def get_cpu_freq():
	with open("/proc/cpuinfo", 'r') as f:
		for l in f.readlines():
			if 'MHz' in l:
				return float(l.split(':',1)[1][:-1])
	return None


fifo = list()
pos = 0.0

while 1:

	if len(fifo ) > 100:
		fifo.pop(0)		

	fifo.append(get_cpu_freq())

	avg = (sum([v**2 for v in fifo]) / len(fifo) )**0.5


	a= 11

	ss= avg / 20000.

	pos = (pos + ss) % float(a)

	b = int(pos % float(a))
	c = (b+3) % a

	data = ''
	for v in range(42):
		if v % a == b:
			data += red
		elif v % a == c:
			data += white
		else:
			data += black
	send_led_data( data )




while 1:

	n=int(21+math.sin(time.time()*5)*21)
	if n < 0:
		n = 0
	if n > 41:
		n=41

	send_led_data( green * n + red * (42 - n))

