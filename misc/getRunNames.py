import pickle
import os,re
from decimal import Decimal

runs = []
iFile = open('runs.txt', 'w')

for file in os.listdir('/data/trees_v16/'):
    fileName = ' " '
    if 'Run' in file:
        fileName += file+' " , \n'
        iFile.write(fileName)
iFile.close()
