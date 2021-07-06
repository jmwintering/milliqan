import os,re
import ROOT as r
import utilities as u
import sys
from ROOT import *

runToAdd = '132'
if len(sys.argv) > 1:
    runToAdd = sys.argv[1]
filesToAdd =  []
# filesToAdd.append('/data/barEfficiency')
for i in range(1,10):
    filesToAdd.append('/store/user/llavezzo/hsTrees_v16/HS_Run'+runToAdd+'/'+str(i)+'/*.root')


c = r.TChain('t')
for file in filesToAdd:
    print file
    c.Add(file)
n = c.GetEntries()
print 'Added', n, 'entries'

if n == 0:
    print("Invalid number of entries: 0")
    exit()

outputFile = open('cosmicData.csv', 'w')

nPEcut = 1000
hnPE = r.TH1D('hnPE', 'hnPE', 100, 0, 2000)

for iE in range(n):
    c.GetEntry(iE)

    cosmicValue = 0

    chansActiveLayer1 = []
    chansActiveLayer2 = []
    chansActiveLayer3 = []

    chanSet = set(c.chan)

    for ipulse, type, chan, nPE, layer, row, col in zip(c.ipulse, c.type, c.chan, c.nPE, c.layer,c.row, c.column):
		if ipulse != 0: continue
		if type != 2: continue

		hnPE.Fill(nPE)

        if layer == 1 and nPE > nPEcut:
            chansActiveLayer1.append(chan)

        if layer == 2 and nPE > nPEcut:
            chansActiveLayer2.append(chan)
        
        if layer == 3 and nPE > nPEcut:
            chansActiveLayer3.append(chan)
        
    # print chansActiveLayer1, chansActiveLayer2, chansActiveLayer3
    if chansActiveLayer1 == u.channelMap[0][0] or chansActiveLayer1 == u.channelMap[0][1]:
        cosmicValue = 1
    if chansActiveLayer2 == u.channelMap[1][0] or chansActiveLayer2 == u.channelMap[1][1]:
        cosmicValue = 1
    if chansActiveLayer3 == u.channelMap[2][0] or chansActiveLayer3 == u.channelMap[2][1]:
        cosmicValue = 1
    if cosmicValue == 1:
        print chanSet
    
hnPE.Draw()
raw_input()


        
        

        
