import pickle
import os,re
import sys
import time
from decimal import Decimal
import glob
import subprocess

if os.path.exists('lumis.txt'):
    os.remove('lumis.txt')
lumis = open('lumis.txt', 'w')
validation = open('validationHtml.txt', 'r')

# grab and store run and associated lumi
lumiInfo = []
for line in validation:
    line.split()

    if '/index.html' in line:
        if 'milliqanValidationPlots' in line:
            continue
        run = line[line.find('Run')+3:line.find('/')]

    if 'pb' in line:
        lumi = line[line.find('L'):line.find('pb')]

        if 'Lumi' in lumi:
            lumi = lumi.replace('Lumi', '')
        if 'L' in lumi:
            lumi = lumi.replace('L', '')
        if '=' in lumi:
            lumi = lumi.replace('=', '')
        if '/' in lumi:
            lumi = lumi.replace('/', '')
        if ' ' in lumi:
            lumi = lumi.replace(' ', '')
        if lumi == '':
            lumi = 'Invalid'
        lumiInfo.append([run, lumi])
validation.close()

# write in a readable format
for iL in lumiInfo:
    for i, iE in enumerate(iL):
        if i == 1:
            lumis.write(iE + ' \n')
        else:
            lumis.write(iE + ' ')

# check against 'bad runs -- runs deemed bad via ucsb or  started/ended in middle of fill' to calculate total lumi
bad_runs = []
badRunFile = open('bad_runs.txt', 'r')
for line in badRunFile:
    string = line.split()
    if string == []:
        continue
    if 'fill' in string or 'runs' in string or 'in' in string:
        continue
    run = string[0]
    bad_runs.append(run)
badRunFile.close()

totalLumi = 0
for iL in lumiInfo:
        for i, iE in enumerate(iL):
            if i == 0:
                if iE not in bad_runs:
                    if iL[1] != 'Invalid':
                        totalLumi += Decimal(iL[1])

lumis.write('total: ' + str(totalLumi) + ' /pb  or '+ str(float(totalLumi)/1e3) + ' /fb')
lumis.close()
