#encoding=utf-8

import random

class vec3:
	def __init__(self, x=0.0, y=0.0, z=0.0):
		self.x, self.y, self.z = x, y, z

	def __abs__(self):
		return sum([v**2 for v in self])**0.5

	def __iter__(self):
		yield self.x
		yield self.y
		yield self.z

	def get_normalized(self):
		return self / abs(self)

	def __sub__(self, other):
		if isinstance(other, vec3):
			return vec3(*[a-b for a,b in zip(self, other)])
		return vec3(*[a-other for a in self])

	def __add__(self, other):
		if isinstance(other, vec3):
			return vec3(*[a+b for a,b in zip(self, other)])
		return vec3(*[a+other for a in self])

	def __div__(self, other):
		if isinstance(other, vec3):
			return vec3(*[a/b for a,b in zip(self, other)])
		return vec3(*[a/other for a in self])

	def __mul__(self, other):
		if isinstance(other, vec3):
			return vec3(*[a*b for a,b in zip(self, other)])
		return vec3(*[a*other for a in self])

	def __repr__(self):
		return '<vec3 (%f, %f, %f)>' % tuple(self)

	def __setitem__(self, index, value):
		#print "setitem", value
		if index == 0:
			self.x = value
		elif index == 1:
			self.y = value
		elif index == 2:
			self.z = value
		else:
			raise IndexError

class box3:
	def __init__(self, low=None, high=None):
		self.low = low
		self.high = high

	def update(self, pos):
		if self.low == None:
			self.low = pos
		if self.high == None:
			self.high = pos

		self.low = vec3(*[min(p, l) for p, l in zip(pos, self.low)])
		self.high = vec3(*[max(p, h) for p, h in zip(pos, self.high)])

	def bounce(self, pos, dr):

		#print "pos", pos, "low", self.low, "high", self.high, "dr", dr
		for i, (p, l, h, d) in enumerate(zip(pos, self.low, self.high, dr)):
			#print "bounce", i, p, l, h, d
			if p < l:
				p = l
				d *= -1

			if p > h:
				p = h
				d *= -1

			pos[i] = p
			dr[i] = d

	def grow(self, amount):
		self.low-= amount
		self.high+= amount

			
def random_direction():

	return vec3(random.random()*2.0-1.0, random.random()*2.0-1.0, random.random()*2.0-1.0).get_normalized()