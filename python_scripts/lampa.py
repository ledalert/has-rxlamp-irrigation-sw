#encoding=utf-8

import serial, sys, time, struct, math
from scipy import fft as fft
s=serial.Serial(sys.argv[1], 1000000)
s.setDTR(True)
s.setRTS(True)
s.setRTS(False)

def send_led_data(data):
	s.write( data )
	s.flush()
	time.sleep(delay)


ds=6			#downsampling

def get_avg(samples, bar1, bar2):
	value = 0.0
	for bar in range(bar1, bar2):
		try:
			value += abs(samples[bar]) / (bar2-bar1)
		except:
			pass

	return value


#44100
while 1:

	freq_domain = [0.0] * 256

	for downsample in range(ds):
		samples = []
		for v in range(256):
			left, right = struct.unpack('hh',sys.stdin.read(4))
			samples.append(left/32767.0)

		freq_domain = [ old + v.real for v, old in zip(fft(samples), freq_domain)]



	r = min(max(get_avg(freq_domain, 128, 150) * 3.0, 0), 1) ** math.e
	g = min(max(get_avg(freq_domain,150, 180) * 2.0, 0), 1) ** math.e
	b = min(max(get_avg(freq_domain,180, 250) * 0.5, 0), 1) ** math.e

	data = (chr(int(r*255)) + chr(int(g*255)) + chr(int(b*255)))*42

	s.write(data)