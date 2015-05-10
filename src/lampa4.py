#encoding=utf-8
import calc_lamp, v3, random, colour
import serial, sys, time, struct, math, threading
from scipy import fft
s=serial.Serial(sys.argv[1], 1000000)
s.setDTR(True)
s.setRTS(True)
s.setRTS(False)


max_hue = 1.0 / 6.0 	#fire!
max_hue = 2.0 			#Rainbow!

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
			sys.stdin.read(400)
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
	def __init__(self, color, pos=None, dr=None):

		self.pos = pos
		self.dir = dr

		if self.pos == None:
			self.pos = v3.vec3(0, 0, 0)

		if self.dir == None:
			self.dir = v3.random_direction() * 0.25
		self.color = color
		self.radius = 30.0


balls = set()
#calc_lamp.led_box.grow(10)

max_balls = 0
max_heat = 1.0
hue_displacement = 0.0
while 1:

	fft_data = get_fft_data()


	i = 0.0

	if fft_data:						
		#low = get_top(fft_data, 128, 132)
		mid = get_top(fft_data, 140, 170) *.1
		high = get_top(fft_data, 250, 255) *.1

		max_balls = int(min(high* 3.0 , 40)) + 5
		max_heat = min(mid * 50.0, 1.5)

		#print max_balls, max_heat


	if len(balls) < max_balls:
		a = random.random()*math.pi*2
		ball = cball(color = (1.0, 1.0, 0.0), pos=v3.vec3(x=math.sin(a)*calc_lamp.helix_radius , y=math.cos(a)*calc_lamp.helix_radius , z =-50) )		
		ball.heat = random.random() * max_heat+ 0.6
		balls.add(ball)

	print len(balls)

	#hue_displacement = (hue_displacement+0.0001) % 1.0


	#Float balls
	to_remove=set()
	for ball in balls:
		ball.pos += ball.dir * 0.1
		ball.dir.z += ball.heat * 0.3 + 0.2

		if ball.pos.z > calc_lamp.helix_height + 50:
			to_remove.add(ball)

	balls -= to_remove

	#Display balls in lamp
	data = ''
	for led in calc_lamp.led_list:

		heat = 0.0

		for ball in balls:
			dis = abs(ball.pos - led)

			i = 0
			if dis < ball.radius:
				i = (1 - dis/ball.radius)**2

			heat += i * ball.heat

			ball.heat *= 0.998	#cool down



		#heat *= 0.3
		heat = min(heat, 1.0)
		r, g, b = colour.hsl2rgb(  ((heat**0.5*max_hue + hue_displacement) % 1.0, 1.0, heat))
		#r, g, b = min(max(r, 0), 1), min(max(g, 0), 1), min(max(b, 0), 1)
		
		data += chr(int(r**math.e * 255)) + chr(int(g**math.e*255)) + chr(int(b**math.e*255))

	s.write(data)	
	s.flush()
	#time.sleep(0.01)


