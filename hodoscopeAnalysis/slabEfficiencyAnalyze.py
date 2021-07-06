import os,re
import ROOT as r
from decimal import Decimal
import utilities as u
import math
import matplotlib.pyplot as plt

print 'Running slabEfficiencyAnalyze.py...'

class Event:
    def __init__(self, run, file, event, channels = None, ratios = None):
        if channels is None:
            channels = []
        if ratios is None:
            ratios = []
        self.run = run
        self.file = file
        self.event = event
        self.channels = channels
        self.ratios = ratios
    def appendChans(self, l = None):
        if l is None:
            l = []
        self.channels = self.channels + l

    def appendRatios(self, l):
        if l is None:
            l = []
        self.ratios = self.ratios + l

    def printInfo(self):
        print self.run
        print self.file
        print self.event
        print self.channels
        print self.ratios

    def simplify(self):
        chanSet = set(self.channels)
        ratioSet = [0]*len(chanSet)
        ratioCount = [0]*len(chanSet)
        for i, chan in enumerate(chanSet):
            for j, ich in enumerate(self.channels):
                if ich == chan:
                    ratioSet[i] += self.ratios[j]
                    ratioCount[i] += 1
            ratioSet[i] = ratioSet[i] / ratioCount[i]
        self.channels = chanSet
        self.ratios = ratioSet


Events = []  
tag = 'slabs'
runSelection = 'All'
filenames = ['ellipFiles/'+tag+runSelection+'_output.txt', 'ellipFiles/'+tag+runSelection+'noMath.txt'] 

for filename in filenames:
    openFile = open(filename)
    num_lines = sum(1 for line in open(filename))
    print 'Added ', num_lines, ' lines from ', filename

    ratioTotal = []
    channelTotal = []
    for iline, line in enumerate(openFile):
        #split the line and extract data
        run = line[line.find("Run")+3:line.find("File")]
        file = line[line.find("File")+4:line.find("Event")]
        event = line[line.find("Event")+5:line.find("Channels")]
        channels = line[line.find("Channels")+8:line.find("Ratios")]
        ratios = line[line.find("Ratios")+6:len(line)]

        channels = channels.replace("{","")
        channels = channels.replace("}","")
        channels = channels.replace("\"","")
        ratios = ratios.replace("\"","")
        ratios = ratios.replace("{","")
        ratios = ratios.replace("}","")

        channelNums = []
        for t in channels.split(","):
            try:
                channelNums.append(int(t))
            except ValueError:
                pass

        ratioNums = []
        for t in ratios.split(","):
            t = t.replace('*^','e')
            try:
                ratioNums.append(float(t))
            except ValueError:
                #print line
                pass
        if len(ratioNums) == 0 or len(channelNums) == 0:
            continue
       
        #TESTING
        #print run, file, event, channelNums, ratioNums
        ratioTotal += ratioNums
        channelTotal += channelNums

        found = False
        for iEvent in Events:
            if(iEvent.run != run): continue
            if(iEvent.file != file): continue
            if(iEvent.event != event): continue
            iEvent.appendChans(channelNums)
            iEvent.appendRatios(ratioNums)
            found = True
        
        if not found:
            Events.append(Event(run, file, event, channelNums, ratioNums))



# print "Printing Simplified Events"
# for event in Events:
#     event.simplify()
#     print "--------"
#     event.printInfo()

nPredicted = 0
nPredictedHit = 0
nPredictedNoHit = 0

c1 = r.TChain("t")
c1.Add("/data/barEfficiency_4hshits/*.root")
entries = c1.GetEntries()
print "Added number of entries", entries

#  bar cuts
muonBarCuts = [400]*32
cosmicBarCuts = [0]*26
cosmicBarCuts[0] = 2900
cosmicBarCuts[1] = 2700
cosmicBarCuts[2] = 2500
cosmicBarCuts[3] = 2500
cosmicBarCuts[4] = 2800
cosmicBarCuts[5] = 900
cosmicBarCuts[6] = 3400
cosmicBarCuts[7] =  2800
cosmicBarCuts[8] = 2500
cosmicBarCuts[9] = 1400
cosmicBarCuts[12] = 2900
cosmicBarCuts[13] = 2000
cosmicBarCuts[16] = 5700
cosmicBarCuts[17] = 2000
cosmicBarCuts[22] = 650
cosmicBarCuts[23] = 3000
cosmicBarCuts[24] = 2000
cosmicBarCuts[25] = 3600

for iE in range(entries):
    u.displayProgress(iE,entries)
    c1.GetEntry(iE)

    if c1.beam:
        barCuts = muonBarCuts
    else: 
        barCuts = cosmicBarCuts
    
    pred_chans = []
    pred_ratios = []

    foundMatchingEvent = False
    for iEvent in Events:
        runNum = int(iEvent.run)
        fileNum = int(iEvent.file)
        eventNum = int(iEvent.event)

        if(runNum != c1.run): continue
        if(fileNum != c1.file): continue
        if(eventNum != c1.event): continue
        foundMatchingEvent = True
        iEvent.simplify()
        pred_chans = iEvent.channels
        pred_ratios = iEvent.ratios
    if not foundMatchingEvent: continue 


    hs_phys_coord_xTop = []
    hs_phys_coord_xBot = []
    hs_phys_coord_yTop = []
    hs_phys_coord_yBot = []
    
    for ihs in c1.v_hs:

        coord_index = u.convertRawToPhysCh(ihs)
        if len(coord_index) < 3: continue

        if ihs >= 48 and ihs <= 63:
            if coord_index[0] != 0:
                hs_phys_coord_xTop.append(u.convertPhysChToCoord(coord_index)[0])

        if ihs >= 0 and ihs <= 15:
            if coord_index[0] != 0:
                hs_phys_coord_xBot.append(u.convertPhysChToCoord(coord_index)[0])
        
        if ihs >= 32 and ihs <= 47:
            if coord_index[1] != 0:
                hs_phys_coord_yTop.append(u.convertPhysChToCoord(coord_index)[1])

        if ihs >= 16 and ihs <= 31:
            if coord_index[1] != 0:
                hs_phys_coord_yBot.append(u.convertPhysChToCoord(coord_index)[1])


    barCoordsL1_x = [];
    barCoordsL1_y = [];
    barCoordsL3_x = [];
    barCoordsL3_y = [];

    #store layer 1 coords and layer 3 coords
    if len(pred_chans) !=0:
        for ich in pred_chans:
            if u.getLayer(ich) == 1:
                barCoordsL1_x.append(u.convertBarChToPhysCoord(ich)[0])
                barCoordsL1_y.append(u.convertBarChToPhysCoord(ich)[1])
            
            if u.getLayer(ich) == 3:
                barCoordsL3_x.append(u.convertBarChToPhysCoord(ich)[0])
                barCoordsL3_y.append(u.convertBarChToPhysCoord(ich)[1])
        

    withinBarCenter = True;
    if len(pred_chans) != 0:

        for hsCoord in hs_phys_coord_xBot:
            for barCoord in barCoordsL1_x:
                if abs(hsCoord-barCoord) > 3.5:
                    withinBarCenter = False
       
        for hsCoord in hs_phys_coord_yBot:
            for barCoord in barCoordsL1_y:
                if abs(hsCoord-barCoord) > 3.5:
                    withinBarCenter = False

        for hsCoord in hs_phys_coord_xTop:
            for barCoord in barCoordsL3_x:
                if abs(hsCoord-barCoord) > 3.5:
                    withinBarCenter = False

        for hsCoord in hs_phys_coord_yTop:
            for barCoord in barCoordsL3_y:
                if abs(hsCoord-barCoord) > 3.5:
                    withinBarCenter = False


    chansHit = []
    chansHitRows = []
    chansHitCols = []
    chansHitLayers = []
    otherBarNPE = 0
    layersHit = [False, False, False]
    # corresponds to slabs  18, 20, 28, 21
    slabsHit = []
    slabsCuts = [300, 350, 150, 350]

    for ipulse, hittype, nPE, chan, layer, row, col in zip(c1.ipulse, c1.type, c1.nPE, c1.chan, c1.layer, c1.row, c1.column):
        if ipulse != 0:
            continue
        if chan == 18 and nPE > 300: slabsHit.append(chan)
        if chan == 20 and nPE > 300: slabsHit.append(chan)
        if chan == 28 and nPE > 300: slabsHit.append(chan)
        if chan == 21 and nPE > 300: slabsHit.append(chan)

        if hittype == 0:
            if nPE > barCuts[chan]:
                chansHit.append(chan)
                chansHitCols.append(col)
                chansHitRows.append(row)
                chansHitLayers.append(layer)
                layersHit[layer-1] = True
            else: 
                otherBarNPE += nPE

    predLayers = []
    predRows = []
    predCols = []
    for i, layer in enumerate(u.channelMap):
        for j, col in enumerate(layer):
            for k, chan in enumerate(col):
                if chan in pred_chans:
                    predLayers.append(i+1)
                    predCols.append(j+1)
                    predRows.append(k+1)

    straightPredChans = len(set(predCols)) == 1 and len(set(predRows)) == 1 and len(set(predLayers)) == 3
    isStraightPath = len(set(chansHitCols)) == 1 and len(set(chansHitRows)) == 1


    # CUTS 
    if otherBarNPE > 200: continue 
    if not layersHit[0] or not layersHit[1] or not layersHit[2]: continue
    if len(chansHit) != 3: continue
    if not isStraightPath: continue
    if not withinBarCenter: continue 
    if set(pred_chans) != set(chansHit): continue
    if len(slabsHit) < 3: continue

    nPredicted += 1
    if len(slabsHit) == 4: nPredictedHit += 1

nPredicted = float(nPredicted)
nPredictedHit = float(nPredictedHit)
print 'nPredicted:', nPredicted
print 'nHit:', nPredictedHit
print '\t eff:', nPredictedHit/nPredicted, '+-', u.propagatePoissErrors(nPredictedHit, nPredicted)
    









