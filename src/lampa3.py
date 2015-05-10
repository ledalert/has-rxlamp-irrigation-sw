#encoding=utf-8

import calc_lamp, v3

print calc_lamp.led_box






import serial, sys, time, struct, math, threading
from scipy import fft
s=serial.Serial(sys.argv[1], 1000000)
s.setDTR(True)
s.setRTS(True)
s.setRTS(False)

def get_avg(samples, bar1, bar2):
	value = 0.0
	for bar in range(bar1, bar2):
		try:
			value += abs(samples[bar]) / (bar2-bar1)
		except:
			pass

	return value



def get_top(samples, bar1, bar2):
	value = 0.0
	for bar in range(bar1, bar2):
		try:
			value = max(value, abs(samples[bar]))
		except:
			pass

	return value

#global vars
fft_lock = threading.Lock()
freq_domain = [0.0] * 256
freq_domain_count = 0

def process_audio():
	global fft_lock, freq_domain, freq_domain_count
	print "Processing audio data"
	samples = [0.0] * 256
	while 1:
		
		for v in range(256):
			left, right = struct.unpack('hh',sys.stdin.read(4))
			mono = (left + right) * 0.5
			samples[v] = mono/32767.0

		new = fft(samples)
		with fft_lock:			
			freq_domain_count += 1
			for index in range(len(freq_domain)):
				freq_domain[index] += new[index].real * abs(new[index])
			
			
def get_fft_data():
	global fft_lock, freq_domain, freq_domain_count
	with fft_lock:
		if not freq_domain_count:
			return
		#print "Returning average for %i FFT chunks" % freq_domain_count
		data = [v/freq_domain_count for v in freq_domain]
		freq_domain_count = 0
		freq_domain = [0.0] * 256
		return data




# Main loop

threading.Thread(target=process_audio).start()

time.sleep(0.1)
r, g, b = 0.0, 0.0, 0.0


class cball:
	def __init__(self, color):
		self.pos = v3.vec3(0, 0, 0)
		self.dir = v3.random_direction() * 0.25
		self.color = color
		self.radius = 40.0


balls = (
	cball((1.0, 0.0, 0.0)),	#red ball
	cball((0.0, 1.0, 0.0)),	#green ball
	cball((0.0, 0.0, 1.0)),	#blue ball
	cball((1.0, 1.0, 1.0)),	#white ball
)



calc_lamp.led_box.grow(10)

while 1:

	fft_data = get_fft_data()


	i = 0.0


	#Bounce the balls
	for ball in balls:
		ball.pos += ball.dir
		calc_lamp.led_box.bounce(ball.pos, ball.dir)


	#Display balls in lamp
	data = ''
	for led in calc_lamp.led_list:

		r, g, b = 0.0, 0.0, 0.0

		for ball in balls:
			dis = abs(ball.pos - led)

			i = 0
			if dis < ball.radius:
				i = (1 - dis/ball.radius)**2

			br, bg, bb = ball.color
			r += br * i
			g += bg * i
			b += bb * i

		r, g, b = min(max(r, 0), 1), min(max(g, 0), 1), min(max(b, 0), 1)
		
		data += chr(int(r**math.e * 255)) + chr(int(g**math.e*255)) + chr(int(b**math.e*255))

	s.write(data)	
	time.sleep(0.01)


