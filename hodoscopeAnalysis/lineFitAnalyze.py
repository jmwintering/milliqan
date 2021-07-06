import os,re
import ROOT as r
import organizePredictedChannels as pred
import utilities as u
import math 

fileToImportToMathematica = 'fitLinePoints'

rerunEff =  True
rerunMath = True
testEff = False

if rerunEff:
    if os.path.exists(fileToImportToMathematica+'.txt'):
        os.remove(fileToImportToMathematica+'.txt')
    if os.path.exists(fileToImportToMathematica+'noMath.txt'):
        os.remove(fileToImportToMathematica+'noMath.txt')
    os.system('root -l -b -q fitLineEfficiency.cpp')
if not os.path.exists(fileToImportToMathematica+'.txt'):
    print 'ERROR: Please run fitLineEfficiency.cpp to generate txt files containing points'
    exit()
if testEff: exit()


if os.path.exists(fileToImportToMathematica+'_output.txt'):
    if rerunMath:
        os.remove(fileToImportToMathematica+'_output.txt')
else:
    if not rerunMath:
        print 'Error: mathematica script has not been run yet...'
        
        successfullyAnswered = False
        while successfullyAnswered:
            answer = raw_input('Run it now? (y/n)')
            if answer == 'y':
                pred.runMathematica(fileToImportToMathematica)
                successfullyAnswered = True
            elif answer == 'n':
                exit()
                successfullyAnswered = True
            else:
                print 'Please enter y for yes or n for no'


if rerunMath:
    pred.runMathematica(fileToImportToMathematica)

filesForPred = [fileToImportToMathematica + '_output.txt', fileToImportToMathematica+'noMath.txt']


filesForChain = '/data/hsEfficiency2/*.root'
c = r.TChain('t')
c.Add(filesForChain)
n = c.GetEntries()
if n == 0:
    print 'Error: chain has 0 entries'
    exit()
else:
    print 'Added', n, 'entries from', filesForChain

nPredHits = 0
nNotPredHits = 0
muonNPE = 400

muonEvents = open('muon_events.txt')

events = []
for line in muonEvents:
    run = line.split()[0]
    file = line.split()[1]
    event = line.split()[2]
    pChans = []
    if len(line.split()) < 6: continue
    pChans.append(int(line.split()[3]))
    if len(line.split()) >= 5:
        pChans.append(int(line.split()[4]))
    if len(line.split()) >= 6:
        pChans.append(int(line.split()[5]))
    entry = [int(run), int(file), int(event), pChans]
    events.append(entry)

hPass = r.TH1D('hPass', 'hPass', 1, 0, 1)
hTotal = r.TH1D('hTotal', 'hTotal', 1, 0, 1)

for iE in range(n):
    c.GetEntry(iE)
    u.displayProgress(iE, n)

    #if not c.beam: continue

    # consider only events with HS data
    skipEvent = False
    if -1 in c.v_hs: skipEvent = True
    if skipEvent: continue

    otherBarNPE = 0
    for ipulse, hittype, nPE, chan, layer, row, col in zip(c.ipulse, c.type, c.nPE, c.chan, c.layer, c.row, c.column):
        if ipulse != 0: continue
        if hittype == 0:
            if nPE > muonNPE:
                pass
            else: 
                otherBarNPE += nPE
    # if otherBarNPE > 200: continue

    chansHit = []
    foundEvent = False
    for e in events:
        if [e[0], e[1], e[2]] == [c.run, c.file, c.event]:
            foundEvent = True
            chansHit = e[3]
        
    
    if not foundEvent: continue
    
    predictedChannels = []
    predictedRatios = []
    predictedChannels, predictedRatios = pred.getPredictedChannels(filesForPred, c.run, c.file, c.event) 

    # print predictedChannels, chansHit
    # if sorted(list(predictedChannels)) != sorted( chansHit):
    #     print predictedChannels, chansHit  

    missedPulse = False
    for ich in chansHit:
        if ich in predictedChannels:
            index = list(predictedChannels).index(ich)
            nPredHits += predictedRatios[index]
            hPass.Fill(0.0, predictedRatios[index])
            hTotal.Fill(0.0, predictedRatios[index])
        else:
            nNotPredHits += 1  
            missedPulse = True
            hTotal.Fill(0.0)
        
    # if missedPulse:
    #     print c.run, c.file, c.event, predictedChannels, chansHit 
            
a = float(nPredHits)
b = float(nPredHits + nNotPredHits)
error = -1
eff = -1
try: 
    eff = a/b
except:
    print 'Warning in <lineFitAnalyze>: nPredHits + nNotPredHits is 0'

try: 
    error = u.calcIntervalErrors(hPass,hTotal)
except: 
    print 'Error in <utilities.calcIntervalErrors>: Error calculating error bounds. Error will be -1'
    
print 'nPred', a
print 'nNotPred', b-a
print 'eff of HS fit:', eff, '-+',  u.propagatePoissErrors(a,b)