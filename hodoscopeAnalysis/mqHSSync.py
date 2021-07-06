import ROOT as r
import sys
import os,re
from decimal import Decimal

if __name__ == '__main__':
    nSync = 0
    nUnsync = 0
    totalnSync = 0
    totalnUnSync = 0
    run = ''
    runDirectory = ''
    for file in os.listdir('/store/user/wintering/'):
        if 'HS_Run' not in file:
            continue
        runDirectory = file
        run = file[6:len(file)]
        if runDirectory == '':
            print 'Error could not find run'
            exit()
        c = r.TChain('t')
        c.Add('/store/user/wintering/'+ runDirectory + '/*.root')
        entries = c.GetEntries()
        print 'For run', run, 'Added', entries

        for iE in range(entries):
            c.GetEntry(iE)

            if -1 in c.v_hs:
                nUnsync += 1
            else:
                nSync += 1
        totalnSync += nSync
        totalnUnSync += nUnsync
        print "Run", run, nSync, nUnsync
    print "total", totalnSync, totalnUnSync
