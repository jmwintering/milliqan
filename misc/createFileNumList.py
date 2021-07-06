import pickle
import os,re
import sys
import time
from decimal import Decimal
import glob
import subprocess

run = ''
if len(sys.argv) > 1:
    for iArg in sys.argv[1:]:
        if iArg == 'help':
            #print help statement
            print 'usage: \n \t help - print this help statement'
            print '\t run - run number '
            exit()
        run = iArg
else:
    print 'Please give run number'
    exit()

if Decimal(run) < 1000 or Decimal(run) > 2000:
    print 'Run not in trees version 16'
    exit()

runDirectory = ''
for rundir in os.listdir('/store/user/llavezzo/'):
    if 'Run' not in rundir or 'HS' in rundir:
        continue
    if run not in rundir:
        continue
    runDirectory = rundir
if runDirectory == '':
    print 'Error: could not find run'
    exit()

fileNums = []
for file in os.listdir('/store/user/llavezzo/'+ runDirectory):
	fileNum = file[file.find('.')+1:file.find('_', 13)]
	fileNums.append(Decimal(fileNum))

numFiles = max(fileNums)
print numFiles
