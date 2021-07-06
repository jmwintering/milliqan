import ROOT as r 
import os
import numpy as np


c1 = r.TChain("t")
c1.Add("/store/user/trees_v19/allPhysicsAndTripleChannel4LayerV19_threeLayerHitPlusSlab_200506.root")

n = c1.GetEntries()
print("Added {0} entries".format(n))


layer3Hits = [0]*2115
layer3_1PerLayer_Hits = [0]*2115
layer4Hits = [0]*2115
layer4_1PerLayer_Hits = [0]*2115

badChans = [12, 13]


for iE in range(n):
    if iE % 1000000 == 0: print(iE)
    c1.GetEntry(iE)

    badChansPresent = False

    if c1.groupTDC_b0[0] != c1.groupTDC_b1[0]: continue

    runIndex = c1.run
    layers = []

    for hittype, layer, chan in zip(c1.type, c1.layer, c1.chan):
        # if hittype != 0: continue
        if chan in badChans:
            badChansPresent = True
            break
        if chan == 28 or chan == 20: 
            layers.append(0)
        else: 
            if hittype != 0: continue
            layers.append(layer)
    if badChansPresent: continue
 
    layerSet = set(layers)
    nLayers = len(layerSet)
    nHits = len(layers)

    if nLayers == 3:
        layer3Hits[runIndex] += 1

        if nHits == 3:
            layer3_1PerLayer_Hits[runIndex] += 1

    elif nLayers == 4:
        layer4Hits[runIndex] += 1

        if nHits == 4:
            layer4_1PerLayer_Hits[runIndex] += 1


hitFile = open('threeLayers.txt','w')
for i in range(1733, len(layer3Hits)):
    hitFile.write(str(i) +" : " + str(layer3Hits[i]) + " : " + str(layer3_1PerLayer_Hits[i]) + " : " + str(layer4Hits[i]) + " : " + str(layer4_1PerLayer_Hits[i]) + "\n")
hitFile.close() 