import os

fileDir = 'cut_files/signal/noslab/'

for file in os.listdir(fileDir):
	fname = file[0:file.find('.txt')+4]
	print(fname)