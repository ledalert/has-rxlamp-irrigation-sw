#encoding=utf-8
import pickle
from math import sqrt

experiments = '''

*	filename			water_volume	total_volume	description			hygrophobic		lumps

	m0.pickled			0.0				700.0			super_dry			yes				no
	m1_10ml.pickled		10.0			715.0			super_dry			yes				no
	m2_20ml.pickled		20.0			730.0			super_dry			yes				no
	m3_30ml.pickled		30.0			760.0			very_dry			yes				no
	m4_40ml.pickled		40.0			775.0			very_dry			yes				no
	m5_50ml.pickled		50.0			800.0			very_dry			yes				no
	m6_70ml.pickled		70.0			950.0			slightly_moist		yes				no
	m7_90ml.pickled		90.0			1000.0			slightly_moist		yes				no
	m8_110ml.pickled	110.0			1050.0			some_moist			yes				no
	m9_130ml.pickled	130.0			1050.0			some_moist			no				no
	m10_180ml.pickled	180.0			1050.0			moist				no				yes
	m11_210ml.pickled	210.0			1100.0			pretty_moist		no				yes
	m12_240ml.pickled	240.0			1050.0			pretty_moist		no				yes
	m13_300ml.pickled	300.0			1000.0			very_moist			no				yes
	m14_360ml.pickled	360.0			800.0			wet					no				yes

'''



def table_loader(data):
	columns = list()
	table = list()
	for line in data.replace('\r', '\n').split('\n'):
		line = line.replace('\t', ' ').strip(' ')
		while line.count('  '):
			line = line.replace('  ', ' ')

		if line:
			if line[0] == '*':
				columns = line[1:].strip(' ').split(' ')
				if len(columns) != len(set(columns)):
					raise Exception("Ambigious column names: %r" % columns)
			else:
				table.append({key: value for key, value in zip(columns, line.split(' '))})

	return table

def table_update(data, updater):
	for item in data:
		item.update(updater(item))


def table_present(data, converters):
	columns = sorted(data[0].keys())
	column_sizes = {columns.index(key): len(key) for key in columns}

	for row in data:
		for key, value in row.items():
			value = converters.get(type(value), '%s') % value
			col_id = columns.index(key)
			if col_id in column_sizes:
				column_sizes[col_id] = max(column_sizes[col_id], len(value) + 3)
			else:
				column_sizes[col_id] = len(value)+3


	item = {columns.index(key): key for key in columns}
	r = ''
	for col in sorted(item.keys()):
		if r:
			r += '   '
		r += ('%%-%is' % column_sizes[col]) % item[col]
	print(r)

	for row in data:
		item = {columns.index(key): converters.get(type(value), '%s') % value for key, value in row.items()}
		r = ''
		for col in sorted(item.keys()):
			if r:
				r += '   '
			r += ('%%-%is' % column_sizes[col]) % item[col]
		print(r)



experiments = table_loader(experiments)


def analyze_experiment(e):
	with open(e['filename'], 'rb') as f:
		times=list()
		dps=list()
		while 1:
			try:
				data = pickle.load(f)
				if data[0] == 'time':
					etime, measure, tmes, th, tl, td = data[1:]
					times.append(tmes*tmes)
					d = td / tmes
					dps.append(d*d)

			except EOFError:
				break

		avg_time = sqrt(sum(times) / len(times))
		avg_dps = sqrt(sum(dps) / len(dps))



	return {'avg_time': avg_time, 'last_time': sqrt(times[-1]), 'millidegrees_per_second': avg_dps*1000}


#Convert floats
table_update(experiments, lambda e: {k: float(v) for k, v in e.items() if k in {'water_volume', 'total_volume'}})

#Calculate water content
table_update(experiments, lambda e: {'water_content': 100.0 * e['water_volume'] / e['total_volume']})


#Analyze experiment
table_update(experiments, analyze_experiment)

#Linear estimate
table_update(experiments, lambda e: {'linear_estimate': e['millidegrees_per_second'] / 2.42})


#Present result
table_present(experiments, {float: '%.2f', int: '%i'})
