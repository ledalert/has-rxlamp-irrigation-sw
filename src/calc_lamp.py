#encoding=utf-8
import math, v3
#lamp model:
#
#	Lamp has LED strip wound around a cupper cylinder
#	The strip has a LED every S mm
#	Diameter of tube is D
#	Control points CP1 and CP2 represent two positions, these should have an angle difference of CP12AD
#	Height difference is CP12HD, try to select control points far from each other
#
#
#	What we need to know:
#	S
#	CP1, CP2 and CP12HD


S = 18.5	#mm pitch
CP1 = 0, 0 	#LED ID, turn id
CP2 = 33, 6	#LED ID, turn id
CP12HD = 66 #mm
L = 14		#Number of LED's




CP1_ID, CP1_ROW = CP1
CP2_ID, CP2_ROW = CP2
RD = CP2_ROW - CP1_ROW
N = CP2_ID - CP1_ID

leds_per_turn = float(N) / float(RD)
height_per_turn = float(CP12HD) / float(RD)


helix_circumferance = float(S) * leds_per_turn
helix_diameter = helix_circumferance / math.pi
helix_radius = helix_diameter / 2.0
helix_turns = float(L-1) / leds_per_turn
helix_height = helix_turns * height_per_turn

height_per_led = height_per_turn / leds_per_turn
radians_per_led = math.pi * 2.0 / leds_per_turn



print "leds per turn", leds_per_turn
print "helix turns", helix_turns
print "helix diameter", helix_diameter
print "helix height", helix_height

led_list = list()
led_box = v3.box3()

for led in range(L):

	angle = led * radians_per_led
	x = math.sin(angle) * helix_radius
	y = math.cos(angle) * helix_radius
	z = led * height_per_led

	pos = v3.vec3(x, y, z)

	led_box.update(pos)
	led_list.append(pos)

