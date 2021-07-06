#
# Calcualte bar efficiencies using bars and slabs by selecting straight line paths
# through the detector in 2/3 layers and slabs, and then checking the third layer
#

import ROOT as r
import pickle
import os,re
import numpy as np
import glob
import sys
from array import array
import math
import utilities as u

c1 = r.TChain("t")
c1.Add("/data/slabskims/*.root");

n = c1.GetEntries()
if n == 0:
	print 'Error in <barEfficiencyMQ.py>: TChain has 0 entries'
	exit()
print("Added {0} entries".format(n))

slabs = [18, 20, 28, 21]
slabCuts = [300, 300, 150 ,300]
muonBarCuts = [1000]*32
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

missedMap = [ [[ 0 for y in range( 3 ) ] for x in range( 2 )] for z in range(3)]
hitMap = [ [[ 0 for y in range( 3 ) ] for x in range( 2 )] for z in range(3)]

nTotal = 0
nPass = 0

for iE in range(n):
	u.displayProgress(iE, n)
	c1.GetEntry(iE)

	chanSet = tuple(sorted(set(c1.chan)))

	barChanSet = []
	barCuts = [0]*32

	if len(c1.v_hs) < 4: continue

	if c1.beam:
		barCuts = muonBarCuts	 
	else: 
		barCuts = cosmicBarCuts

	#Double check all slabs are hit
	missedSlab = False
	for slab in slabs:
		if slab not in chanSet:
			missedSlab = True
	if missedSlab: continue
	
	otherBarsNPE = 0
	layers = []
	rows = []
	cols = []
	slabNPE = [0,0,0,0]
	barsNPE = [[] for x in xrange(32)]
	sheethit = False
	for ipulse, hittype, nPE, chan, layer, col, row in zip(c1.ipulse, c1.type, c1.nPE, c1.chan, c1.layer, c1.column, c1.row):
		if ipulse != 0 or hittype != 2:
			continue
		if nPE > 5:
			sheethit = True
	if sheethit: continue

	#Store largest hit per slab
	for ipulse, hittype, nPE, chan, layer, col, row in zip(c1.ipulse, c1.type, c1.nPE, c1.chan, c1.layer, c1.column, c1.row):
		if ipulse != 0 or hittype != 1:
			continue
		for i, slab in enumerate(slabs):
			if chan == slab:
				if nPE > slabNPE[i]:
					slabNPE[i] = nPE
		
	#Check that slabs pass slab cuts
	slabsQuiet = False
	for i,slabhit in enumerate(slabNPE):
		if slabhit < slabCuts[i]:
			slabsQuiet = True
	if(slabsQuiet): continue

	#Store the bars that pass bar cuts, cut on bars that don't
	for ipulse, hittype, nPE, chan, layer, col, row in zip(c1.ipulse, c1.type, c1.nPE, c1.chan, c1.layer, c1.column, c1.row):
		if ipulse != 0 or hittype != 0:
			continue

		if nPE > barCuts[chan]:
			layers.append(layer)
			rows.append(row)
			cols.append(col)
			barChanSet.append(chan)
		else:
			otherBarsNPE += nPE
	barChanSet = tuple(sorted(set(barChanSet)))
	if otherBarsNPE > 200: continue
	
	#Only look at events with HS path in a straight line
	xCoords = []
	yCoords = []
	for hs in c1.v_hs:
		iCoord = u.convertRawToPhysCh(hs)
		if len(iCoord) < 3: continue

		if iCoord[0] != 0:
			xCoords.append(u.convertPhysChToCoord(iCoord)[0])
		if iCoord[1] != 0:
			yCoords.append(u.convertPhysChToCoord(iCoord)[0])

	if len(set(xCoords)) != 1: continue
	if len(set(yCoords)) != 1: continue
	
	# Check that HS channels are in front of bars
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
	if len(barChanSet) !=0:
		for ich in barChanSet:
			if u.getLayer(ich) == 1:
				barCoordsL1_x.append(u.convertBarChToPhysCoord(ich)[0])
				barCoordsL1_y.append(u.convertBarChToPhysCoord(ich)[1])
			
			if u.getLayer(ich) == 3:
				barCoordsL3_x.append(u.convertBarChToPhysCoord(ich)[0])
				barCoordsL3_y.append(u.convertBarChToPhysCoord(ich)[1])
        
	withinBarCenter = True;
	if len(barChanSet) != 0:
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
	if not withinBarCenter: continue

	layer1 = layers.count(1)
	layer2 = layers.count(2)
	layer3 = layers.count(3)
	# layers = set(layers)

	print layers
	if layer1 > 1 or layer2>1 or layer3>1: continue
	print layer1, layer2, layer3

	straightPath = len(set(rows)) == 1 and len(set(cols)) == 1

	if not straightPath: continue

	if len(layers) != 3:
		print layers, chanSet, set(c1.v_hs), barChanSet	
	
	nTotal += 3
	nPass += len(layers)

nPass = float(nPass)
nTotal = float(nTotal)
print '\nnPass', nPass
print 'nTotal', nTotal
try:
	print '\t pass/total:', nPass/nTotal
except:
	print '\nError in <barEfficiencyMQ.py>: Division by 0: nTotal = 0'
	
	