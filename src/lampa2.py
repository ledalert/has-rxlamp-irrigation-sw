#encoding=utf-8

import serial, sys, time, struct, math, threading
from scipy import fft
s=serial.Serial(sys.argv[1], 1000000)
s.setDTR(True)
s.setRTS(True)
s.setRTS(False)

def send_led_data(data):
	s.write( data )
	s.flush()
	time.sleep(delay)

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

while 1:

	fft_data = get_fft_data()

	r -= 0.05
	g -= 0.05
	b -= 0.05
	r, g, b = max(r, 0),max(g, 0), max(b, 0)

	if fft_data:						
#		nr = get_avg(fft_data, 129, 130)

		low = get_top(fft_data, 128, 132) *15.0
		low_mid =  get_top(fft_data, 128, 200) *1.0
		mid = get_top(fft_data, 140, 170) *4.0

		mid_high = get_top(fft_data, 200, 250) * .005
		high = get_top(fft_data, 250, 255) * .001

		#nr = max(mid * low - mid_high * high, 0)  #max(low - mid_high, 0)
		nr = max(mid_high, low)
		ng = max(mid, mid_high)
		nb = max(high, mid_high)


		
		r = max(nr, r)
		g = max(ng, g)
		b = max(nb, b)


	r, g, b = min(max(r, 0), 1), min(max(g, 0), 1), min(max(b, 0), 1)

	data = (chr(int(r**math.e * 255)) + chr(int(g**math.e*255)) + chr(int(b**math.e*255)))*42

	s.write(data)	
	time.sleep(0.01)


