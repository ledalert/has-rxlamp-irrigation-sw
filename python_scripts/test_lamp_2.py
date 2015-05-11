import serial, time, random, math, sys, colour
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




def get_cpu_freq():
	with open("/proc/cpuinfo", 'r') as f:
		for l in f.readlines():
			if 'MHz' in l:
				return float(l.split(':',1)[1][:-1])
	return None


fifo = list()





class LED:

	def __init__(self):
		self.i = 0.0
		self.hue = random.random()
		self.t=0.0
		self.wt=random.random()*5.0+1.0
		self.mode = self.wait


	def wait(self, dt):
		self.t += dt
		if self.t > self.wt:
			self.t = 0.0
			self.mode = self.fade_up
			self.hue = self.mh + random.random()*.25

	def fade_up(self, dt):
		self.i += dt*.25
		if self.i > 1.0:
			self.i = 1.0
			self.mode = self.fade_down

	def fade_down(self, dt):
		self.i -= 0.02
		if self.i < 0:
			self.i = 0.0
			self.wt = random.random()*1.0+0.5
			self.mode = self.wait

	def think(self, dt):
		self.mode(dt)

	def get_struct(self):
		r,g,b = colour.hsl2rgb(( self.hue % 1.0, self.i*.5 +.5, self.i*.5))

		return chr(int(r*255)) + chr(int(g*255)) + chr(int(b*255))
		


Leds = [LED() for l in range(42)]


main_hue = 0.0
while 1:


	if len(fifo ) > 100:
		fifo.pop(0)		

	fifo.append(get_cpu_freq())

	avg = (sum([v**2 for v in fifo]) / len(fifo) )**0.5

	main_hue = (main_hue + avg / 5000000.0) % 1.0


	data = ''
	for led in Leds:
		led.think(avg / 50000.0)
		led.mh = main_hue
		data += led.get_struct()


	send_led_data(data)