import ROOT as r 
import os,re

run = 1000
file = 135
event = 844

c = r.TChain('t')
c.Add('/store/HS_Run1000/*.root')
n = c.GetEntries()
print 'Added', n, 'entries'

for iE in range(n):
    c.GetEntry(iE)

    if c.run != run or c.file != file or c.event != event:
        continue

    for iC,p  in zip(c.chan,c.ipulse):
        if p == 0:
         print iC, 
    print ""
    for iH in c.v_hs:
        print iH, 

