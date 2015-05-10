#encoding=utf-8

import serial, sys, struct, time, pickle, subprocess, threading, queue

speak_queue = queue.Queue()

def speaker():
	while 1:
		text = speak_queue.get()
		if text == None:	#Sentinel
			break
		subprocess.call(('la_speak', text))
		time.sleep(1)	#Wait 1 sec between messages




def speak(text):
	speak_queue.put(text)

s=serial.Serial(sys.argv[1], 921600)
s.setRTS(True)
s.setDTR(True)
time.sleep(0.05)
s.read(s.inWaiting())
s.setRTS(False)

speak("Påbörjar mätning")
threading.Thread(target=speaker).start()


with open('%s.pickled' % sys.argv[2], 'wb') as logfile:
	with open('%s.log' % sys.argv[2], 'w', encoding='utf-8') as textlog:

		def print(text):
			sys.stdout.write(text + '\n')
			textlog.write(text+'\n')


		start_time = time.time()
		print("Listening for readings from controller")

		measurement=0
		while 1:
			msg_id, = struct.unpack('i', s.read(4))
			exp_time = time.time() - start_time
			exp_min = int(exp_time) // 60
			exp_sec = int(exp_time) % 60
			
			if msg_id == 1:
				measurement += 1
				t_high, t_low, time_measured = struct.unpack('f'*3, s.read(4*3))
				delta_t = t_low - t_high;
				pickle.dump(('time', exp_time, measurement, time_measured, t_high, t_low, delta_t), logfile)
				print("Reading %i: %f seconds for Δ%f°K (%f° C high,  %f°C low)" % (measurement , time_measured, delta_t, t_high, t_low))
				speak(("Mätning %i: %.2f sekunder, tid i experimentet %i minuter och %i sekunder" % (measurement, time_measured, exp_min, exp_sec)).replace('.', ' komma '))

				if measurement == 5:				
					break
			elif msg_id == 2:
				temp_measured, = struct.unpack('f', s.read(4))
				pickle.dump(('temp', exp_time, temp_measured), logfile)
				print("Current temperature: %f °C" %  temp_measured)
				speak(("%.2f grader" % (temp_measured)).replace('.', ' komma '))

			elif msg_id == 3:
				tm_year, tm_mon, tm_mday, tm_hour, tm_min, tm_sec, tm_yday, tm_wday = struct.unpack('i'*8, s.read(4*8))
				print("Time: %i-%i-%i %i:%i:%i  weekday: %i  yearday: %i" % (tm_year+1900, tm_mon+1, tm_mday, tm_hour, tm_min, tm_sec, tm_wday, tm_yday))

			else:
				print(msg_id)
				raise Exception


			

		print("All done!")
		speak("Mätning klar!")
		speak(None)