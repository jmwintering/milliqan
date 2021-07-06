import os,re
import ROOT as r
from decimal import Decimal
import utilities as u
import math
import matplotlib.pyplot as plt

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
        print (self.run)
        print (self.file)
        print (self.event)
        print (self.channels)
        print (self.ratios)

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
tag = 'noCut'
runSelection = 'All'
filename = 'ellipFiles/'+tag+runSelection+'.txt'

openFile = open(filename)
num_lines = sum(1 for line in open(filename))
# print 'Added ', num_lines, ' lines from ', filename

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
#print len(ratioTotal), len(channelTotal)


# print "Printing Simplified Events"
# for event in Events:
#     event.simplify()
#     print "--------"
#     event.printInfo()

nPredicted = 0
nPredictedHit = 0
nPredictedNoHit = 0

# old strict selection 
nPredictedTEST = 0
nPredictedHitTEST = 0
nPredictedNoHitTEST = 0

# stricter selection
nPredictedStrict = 0
nPredictedHitStrict = 0
nPredictedNoHitStrict = 0

c1 = r.TChain("t")
c1.Add("/data/skims/barEfficiency_4hshits/*.root")
entries = c1.GetEntries()
print("Added", entries, "entries")

distX = r.TH1D('distX', 'distX', 100, -20, 20)
distY = r.TH1D('distY', 'distY', 100, -20, 20)

sheetPlot = r.TH1D('sheetPlot', 'sheetPlot', 200, 0, 4000)
barPlot = r.TH1D('barPlot', 'barPlot', 200, 0, 4000)

separationXPlot = r.TH1D('sepX', 'Distance (x) between consecutive HS hit and pred bar', 200, -5, 5)
separationHSXPlot = r.TH1D('sep_hsX', 'Distance (x) between consecutive HS hits in same cluster', 200, -5, 5)
separationYPlot = r.TH1D('sepY', 'Distance (y) between consecutive HS hit and pred bar', 200, -5, 5)
separationHSYPlot = r.TH1D('sep_hsY', 'Distance (y) between consecutive HS hits in same cluster', 200, -5, 5)

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

entriesLeft = [0]*4
entriesLeftCuts = ["Total events", "Other bar nPEs sum to < 200", "HS hits are within 1 ch of bar center", "Predicted chs form a straight path"]

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

    entriesLeft[0] += 1


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
        

    withinBarCenter = True
    sepDist = 2 #3.5
    if len(pred_chans) != 0:

        for hsCoord in hs_phys_coord_xBot:
            for barCoord in barCoordsL1_x:
                separationXPlot.Fill(hsCoord-barCoord)
                if abs(hsCoord-barCoord) > sepDist:
                    withinBarCenter = False
       
        for hsCoord in hs_phys_coord_yBot:
            for barCoord in barCoordsL1_y:
                separationYPlot.Fill(hsCoord-barCoord)
                if abs(hsCoord-barCoord) > sepDist:
                    withinBarCenter = False

        for hsCoord in hs_phys_coord_xTop:
            for barCoord in barCoordsL3_x:
                separationXPlot.Fill(hsCoord-barCoord)
                if abs(hsCoord-barCoord) > sepDist:
                    withinBarCenter = False

        for hsCoord in hs_phys_coord_yTop:
            for barCoord in barCoordsL3_y:
                separationYPlot.Fill(hsCoord-barCoord)
                if abs(hsCoord-barCoord) > sepDist:
                    withinBarCenter = False

    # print pred_chans
   
    chansHit = []
    chansHitRows = []
    chansHitCols = []
    chansHitLayers = []
    otherBarNPE = 0
    layersHit = [False, False, False]

    for ipulse, hittype, nPE, chan, layer, row, col in zip(c1.ipulse, c1.type, c1.nPE, c1.chan, c1.layer, c1.row, c1.column):
        
        if ipulse != 0:
            continue

        if hittype == 2:
            sheetPlot.Fill(nPE)

        if hittype == 0:
            barPlot.Fill(nPE)
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


    #CUTS 
    # baseline:
    if otherBarNPE > 200: continue 
    entriesLeft[1] += 1
    # if not layersHit[0] or not layersHit[1] or not layersHit[2]: continue
    # if len(chansHit) != 3: continue


    for i, hs_coordX_i in enumerate(hs_phys_coord_xTop):
        for j, hs_coordX_j in enumerate(hs_phys_coord_xTop):
            if i == j: continue
            separationHSXPlot.Fill(hs_coordX_i -hs_coordX_j)
    for i, hs_coordX_i in enumerate(hs_phys_coord_xBot):
        for j, hs_coordX_j in enumerate(hs_phys_coord_xBot):
            if i == j: continue
            separationHSXPlot.Fill(hs_coordX_i -hs_coordX_j)
    for i, hs_coordX_i in enumerate(hs_phys_coord_yTop):
        for j, hs_coordX_j in enumerate(hs_phys_coord_yTop):
            if i == j: continue
            separationHSYPlot.Fill(hs_coordX_i -hs_coordX_j)
    for i, hs_coordX_i in enumerate(hs_phys_coord_yBot):
        for j, hs_coordX_j in enumerate(hs_phys_coord_yBot):
            if i == j: continue
            separationHSYPlot.Fill(hs_coordX_i -hs_coordX_j)

    # add selections
    if not withinBarCenter: continue
    entriesLeft[2] += 1
    # if not isStraightPath: continue
    if not straightPredChans: continue
    entriesLeft[3] += 1

    #  look at all pulses without requiring anything of other pulses
    nPredicted += len(pred_chans)
    for i, ipred in enumerate(pred_chans):
        if ipred in chansHit:
            nPredictedHit += pred_ratios[i]
        else:
            nPredictedNoHit += pred_ratios[i]

    # Now require at least 1 channel hit and look at other channels
    oneBarHit = False
    for hit in chansHit:
        if hit in pred_chans: oneBarHit = True
    if len(chansHitLayers) > 1:
        nChansHit = 0
        for hit in chansHit:
            if hit in pred_chans:
                nChansHit += 1
        nPredictedStrict += len(pred_chans)
        nPredictedHitStrict += nChansHit
        

        

    #  SKIP STUFF BELOW FOR NOW

    # TESTING
    missedMap = [ [[ 0 for y in range( 3 ) ] for x in range( 2 )] for z in range(3)]
    hitMap = [ [[ 0 for y in range( 3 ) ] for x in range( 2 )] for z in range(3)]

    #If 2 layers are hit in a straight path and one of the layers is not hit
    if len(chansHitLayers) == 2:
        if len(set(chansHitRows)) != 1 or len(set(chansHitCols)) != 1:
            continue
        missedlayer = -1
        for i in range(1, 4):
            if i not in chansHitLayers:
                missedlayer = i

        if missedlayer == -1:
            print('Error missed layer is -1')
            print (chansHitLayers)
            exit()

        channelOutsidePrediction = False
        for chan in chansHit:
            if chan not in pred_chans:
                channelOutsidePrediction = True
        if channelOutsidePrediction: continue

        for i, ipred in enumerate(pred_chans):
            # if ipred in chansHit:
            #     nPredictedHitTEST += pred_ratios[i]
            if ipred not in chansHit:
                # print "run, file, event ", c1.run, c1.file, c1.event
                # print "chansHit ", chansHit
                # print "pred_chans ", pred_chans
                # print "channels ", c1.chan 
                # print "nPE ", c1.nPE
                # print "chan and npe", zip(c1.chan, c1.nPE)
                # print "v_hs ", c1.v_hs
                nPredictedNoHitTEST += pred_ratios[i]
        missedMap[missedlayer-1][chansHitCols[0]-1][chansHitRows[0]-1] += 1

    #If 3 layers are hit and the path is straight
    if(len(chansHitLayers) == 3):
        missedalayer = False
        for i in range(1, 4):
                if i not in chansHitLayers:
                    missedalayer = True
        if(missedalayer): continue

        if len(set(chansHitRows)) != 1 or len(set(chansHitCols)) != 1:
            continue

        channelOutsidePrediction = False
        for chan in chansHit:
            if chan not in pred_chans:
                channelOutsidePrediction = True
        if channelOutsidePrediction: continue

        for i, ipred in enumerate(pred_chans):
            if ipred in chansHit:
                # print set(c1.chan), pred_chans, c1.v_hs, c1.run, c1.file, c1.event
                nPredictedHitTEST += pred_ratios[i]
            else:
                # print "run, file, event ", c1.run, c1.file, c1.event
                # print "chansHit ", chansHit
                # print "pred_chans ", pred_chans
                # print "channels ", c1.chan 
                # print "nPE ", c1.nPE
                # print "chan and npe", zip(c1.chan, c1.nPE)
                # print "v_hs ", c1.v_hs
                nPredictedNoHitTEST += pred_ratios[i]


        hitMap[0][chansHitCols[0]-1][chansHitRows[0]-1] += 1
        hitMap[1][chansHitCols[0]-1][chansHitRows[0]-1] += 1
        hitMap[2][chansHitCols[0]-1][chansHitRows[0]-1] += 1


print("\n\nOriginal selection")
print("Predicted", nPredicted)
print("nPredictedHit", nPredictedHit)
print("nPredictedNoHit", nPredictedNoHit)
a = nPredictedHit
b = nPredictedNoHit + nPredictedHit
if nPredictedHit + nPredictedNoHit != 0:
    print("\teff:",float(a)/float(b), '+-', u.propagatePoissErrors(float(a),float(b)))

print("\nTESTING")
print("nPredictedHit", nPredictedHitTEST)
print("nPredictedNoHit", nPredictedNoHitTEST)
a = nPredictedHitTEST
b = nPredictedNoHitTEST + nPredictedHitTEST
if nPredictedHitTEST + nPredictedNoHitTEST != 0:
    print("\teff:",float(a)/float(b), '+-', u.propagatePoissErrors(float(a),float(b)))


print("\nStricter selection")
print("nPredictedHit", nPredictedHitStrict)
print("nPredicted", nPredictedStrict)
a = nPredictedHitStrict
b = nPredictedStrict
if nPredictedStrict != 0:
    print("\teff:",float(a)/float(b), '+-', u.propagatePoissErrors(float(a),float(b)))

writeResults = False
if writeResults:
    resultsName = 'barEffResults_Math.txt'
    results = open(resultsName, 'w')
    results.write('Observed: '+ str(nPredictedHit)+ '\n')
    results.write('Not Observed: '+ str(nPredictedNoHit)+ '\n')
    results.close()
    print('Wrote results to', resultsName)


print("\n")
for i,j in zip(entriesLeft, entriesLeftCuts):
    print(j, ":", i, "({0}%)".format((i*100)/ entriesLeft[0]))

# c1 = r.TCanvas('c1')
# c1.cd()
# separationHSXPlot.Draw()
# c1.SaveAs('separationHSXPlot.pdf')

# c2 = r.TCanvas('c2')
# c2.cd()
# separationHSYPlot.Draw()
# c2.SaveAs('separationHSYPlot.pdf')

# c3 = r.TCanvas('c3')
# c3.cd()
# separationXPlot.Draw()
# c3.SaveAs('separationXPlot.pdf')

# c4 = r.TCanvas('c4')
# c4.cd()
# separationYPlot.Draw()
# c4.SaveAs('separationYPlot.pdf')