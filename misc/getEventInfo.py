import os,re
import ROOT as r 
import sys

def strVector(tempList):
    vector = '{'
    for i, element in enumerate(tempList):
        vector += ' '+str(element)+' '
        if i != len(tempList)-1: 
            vector += ','
    vector += ' }'
    return vector

if __name__ == "__main__":
    
    if len(sys.argv) != 4:
        print 'Usage: python displayEvent.py run file event'
        exit()
        
    run = sys.argv[1]
    file  = sys.argv[2]
    event = sys.argv[3]

    run = int(run)
    file = int(file)
    event = int(event)

    runPath = '/store/user/llavezzo/hsTrees_v16/'

    run = str(run)
    file = str(file)
    event = str(event)

    c = r.TChain("t")
    subdir = file[0:1]

    config = ''
    for fname in os.listdir(runPath + "HS_Run" + str(run) +"/1/"):
        if '.1_' not in fname: continue
        config = fname[fname.find('_',17)+1:fname.find('v16',25)-1]
    if config == '': 
        print 'Error cannot find a config'
        exit()


    filePath = runPath + "HS_Run" + str(run) +"/" + str(subdir) + "/UX5MilliQan_Run"+ str(run) + "." + file +"_"+config+"_v16.root"

    nFiles = 0
    if os.path.exists(filePath):
        nFiles = c.Add(filePath)
    else: 
        print 'Error: File', filePath, ' does not exist'
        exit()
    if nFiles == 0:
        print 'Error adding files to TChain'

    chans = []
    hsChans = []
    tpChans = []
    predChans = []
    nPEs = []
    times = []

    for iE in range(1001):
        c.GetEntry(iE)

        if c.event != int(event): continue
        
        for ipulse, chan, nPE, time in zip(c.ipulse, c.chan, c.nPE, c.time_module_calibrated):
            if ipulse != 0: continue

            chans.append(chan)
            nPEs.append(nPE)
            times.append(time)

        for hChan in c.v_hs:
            hsChans.append(hChan)

        for tChan in c.v_tp:
            tpChans.append(tChan)

    is4Layer = 0
    if int(run) > 1500: is4Layer = 1
    chansVector = strVector(chans)
    hsChansVector = strVector(hsChans)
    tpChansVector = strVector(tpChans)
    nPEsVector = strVector(nPEs)
    timesVector = strVector(times)

    outFile = open('displayEventInfo.txt', 'w')
    outFile.write('Chan '+chansVector+'\n')
    outFile.write('HSChan '+hsChansVector+'\n')
    outFile.write('TPChan '+tpChansVector+'\n')
    outFile.write('nPE '+nPEsVector+'\n')
    outFile.write('time '+timesVector+'\n')
    outFile.write('is4layer '+str(is4Layer)+'\n')
    outFile.write(run+' '+file+' '+event)
    print 'Wrote event to file displayEventInfo.txt'