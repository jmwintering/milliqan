import os
import numpy as np
import ROOT as r

chanMap =[[1,3,1,0], #0.0
    [2,3,1,0], #0.1
    [1,3,3,0], #0.2
    [2,3,3,0], #0.3
    [1,1,3,0], #0.4
    [2,1,3,0], #0.5
    [1,3,2,0], #0.6
    [2,3,2,0], #0.7
    [1,1,1,0], #0.8
    [2,1,1,0], #0.9
    [0,-2,1,2], #0.10
    [0,-2,2,2], #0.11
    [1,1,2,0], #0.12
    [2,1,2,0], #0.13
    [0,-2,3,2], #0.14 
    [0,0,0,3], #0.15, timing card
    [1,2,2,0], #1.0
    [2,2,2,0], #1.1
    [0,-1,0,1], #1.2
    [1,-2,2,2], #1.3
    [0,-1,1,1], #1.4
    [0,-1,3,1], #1.5
    [1,2,3,0], #1.6
    [2,2,3,0], #1.7
    [1,2,1,0], #1.8
    [2,2,1,0], #1.9
    [1,-2,3,2], #1.10
    [-1,-2,1,2], #1.11
    [0,-1,2,1], #1.12
    [1,-2,1,2], #1.13
    [-1,-2,2,2], #1.14
    [-1,-2,3,2] #1.15
];
extraPathsLists = [[7,12,2],[3,5,1],[0,8,7]]
extraPaths = []

layersMap = {1:[0,1,24,25,8,9],2:[6,7,16,17,12,13],3:[2,3,22,23,4,5]}
allBars = [0,1,24,25,8,9,6,7,16,17,12,13,2,3,22,23,4,5]
restrictList = set([24,25,16,17,22,23])

def preparePaths(badChans = [6,4],slabs=[18,20,28]):
    slabs = tuple(sorted(slabs))
    paths = []
    straight = []
    badStraight = []
    noBadStraight = []
    bentDiffDigi = []
    bentSameDigi = []
    badBentSameDigi = []
    noBadBentSameDigi = []
    straightPlusSlab = []
    straightPlusTwoSlab = []
    straightPlusThreeSlab = []
    straightPlusFourSlab = []
    badStraightPlusSlab = []
    noBadStraightPlusSlab = []
    bentDiffDigiPlusSlab = []
    bentSameDigiPlusSlab = []
    badBentSameDigiPlusSlab = []
    noBadBentSameDigiPlusSlab = []
    straightPlusTwoOrMoreSlabs = []
    badStraightPlusTwoOrMoreSlabs = []
    noBadStraightPlusTwoOrMoreSlabs = []
    bentSameDigiPlusTwoOrMoreSlabs = []
    badBentSameDigiPlusTwoOrMoreSlabs = []
    noBadBentSameDigiPlusTwoOrMoreSlabs = []
    bentDiffDigiPlusTwoOrMoreSlabs = []
    slabPaths = {}
    for iC1,chan1 in enumerate(layersMap[1]):
        for iC2,chan2 in enumerate(layersMap[2]):
            for iC3,chan3 in enumerate(layersMap[3]):
                path = tuple(sorted(set([chan1,chan2,chan3])))
                pathPlusSlab1 = tuple(sorted(set([18,chan1,chan2,chan3])))
                pathPlusSlab2 = tuple(sorted(set([21,chan1,chan2,chan3])))
                pathPlusSlab3 = tuple(sorted(set([28,chan1,chan2,chan3])))
                pathPlusSlab4 = tuple(sorted(set([20,chan1,chan2,chan3])))
                pathsPlus2Slab = []
                pathsPlus3Slab = []
                pathsPlus4Slab = []
                pathsPlus4Slab.append(tuple(sorted(set([18,20,21,28,chan1,chan2,chan3]))))
                pathsPlus1Slab = []
                # pathsPlus1Slab.append(pathPlusSlab1)
                # pathsPlus1Slab.append(pathPlusSlab2)
                # pathsPlus1Slab.append(pathPlusSlab3)
                # pathsPlus1Slab.append(pathPlusSlab4)
                for ix,x in enumerate([18,21,28,20]):
                    for iy,y in enumerate([18,21,28,20]):
                        if ix < iy:
                            pathsPlus2Slab.append(tuple(sorted(set([x,y,chan1,chan2,chan3]))))
                        for iz,z in enumerate([18,21,28,20]):
                            if ix < iy:
                                if iz < ix:
                                    pathsPlus3Slab.append(tuple(sorted(set([x,y,z,chan1,chan2,chan3]))))
                paths.append(path)
                paths.append(pathPlusSlab1)
                paths.append(pathPlusSlab2)
                paths.append(pathPlusSlab3)
                paths.append(pathPlusSlab4)
                paths.extend(pathsPlus2Slab)
                paths.extend(pathsPlus3Slab)
                paths.extend(pathsPlus4Slab)
                if iC1 == iC2 and iC2 == iC3:
                    straight.append(path)
                    straightPlusSlab.append(pathPlusSlab1)
                    straightPlusSlab.append(pathPlusSlab2)
                    straightPlusSlab.append(pathPlusSlab3)
                    straightPlusSlab.append(pathPlusSlab4)
                    straightPlusTwoSlab.extend(pathsPlus2Slab)
                    straightPlusThreeSlab.extend(pathsPlus3Slab)
                    straightPlusFourSlab.extend(pathsPlus4Slab)

                    straightPlusTwoOrMoreSlabs.extend(pathsPlus1Slab+pathsPlus2Slab+pathsPlus3Slab+pathsPlus4Slab)
                    if chan1 in badChans or chan2 in badChans or chan3 in badChans:
                        badStraight.append(path)
                        badStraightPlusSlab.append(pathPlusSlab1)
                        badStraightPlusSlab.append(pathPlusSlab2)
                        badStraightPlusSlab.append(pathPlusSlab3)
                        badStraightPlusSlab.append(pathPlusSlab4)
                        badStraightPlusTwoOrMoreSlabs.extend(pathsPlus1Slab+pathsPlus2Slab+pathsPlus3Slab+pathsPlus4Slab)
                    else:
                        noBadStraight.append(path)
                        noBadStraightPlusSlab.append(pathPlusSlab1)
                        noBadStraightPlusSlab.append(pathPlusSlab2)
                        noBadStraightPlusSlab.append(pathPlusSlab3)
                        noBadStraightPlusSlab.append(pathPlusSlab4)
                        noBadStraightPlusTwoOrMoreSlabs.extend(pathsPlus1Slab+pathsPlus2Slab+pathsPlus3Slab+pathsPlus4Slab)
                else:
                    if int(chan1/16) == int(chan2/16) and int(chan2/16) == int(chan3/16):
                        bentSameDigi.append(path)
                        bentSameDigiPlusSlab.append(pathPlusSlab1)
                        bentSameDigiPlusSlab.append(pathPlusSlab2)
                        bentSameDigiPlusSlab.append(pathPlusSlab3)
                        bentSameDigiPlusSlab.append(pathPlusSlab4)
                        bentSameDigiPlusTwoOrMoreSlabs.extend(pathsPlus1Slab+pathsPlus2Slab+pathsPlus3Slab+pathsPlus4Slab)
                        if chan1 in badChans or chan2 in badChans or chan3 in badChans:
                            badBentSameDigi.append(path)
                            badBentSameDigiPlusSlab.append(pathPlusSlab1)
                            badBentSameDigiPlusSlab.append(pathPlusSlab2)
                            badBentSameDigiPlusSlab.append(pathPlusSlab3)
                            badBentSameDigiPlusSlab.append(pathPlusSlab4)
                            badBentSameDigiPlusTwoOrMoreSlabs.extend(pathsPlus1Slab+pathsPlus2Slab+pathsPlus3Slab+pathsPlus4Slab)
                        else:
                            noBadBentSameDigi.append(path)
                            noBadBentSameDigiPlusSlab.append(pathPlusSlab1)
                            noBadBentSameDigiPlusSlab.append(pathPlusSlab2)
                            noBadBentSameDigiPlusSlab.append(pathPlusSlab3)
                            noBadBentSameDigiPlusSlab.append(pathPlusSlab4)
                            noBadBentSameDigiPlusTwoOrMoreSlabs.extend(pathsPlus1Slab+pathsPlus2Slab+pathsPlus3Slab+pathsPlus4Slab)
                    else:
                        bentDiffDigi.append(path)
                        bentDiffDigiPlusSlab.append(pathPlusSlab1)
                        bentDiffDigiPlusSlab.append(pathPlusSlab2)
                        bentDiffDigiPlusSlab.append(pathPlusSlab3)
                        bentDiffDigiPlusSlab.append(pathPlusSlab4)
                        bentDiffDigiPlusTwoOrMoreSlabs.extend(pathsPlus1Slab+pathsPlus2Slab+pathsPlus3Slab+pathsPlus4Slab)
    # paths.extend(extraPaths)
    # paths.append(slabs)
    allPaths={"Straight1":[(0,2,6)],"Straight2":[(1,3,7)],"Straight3":[(16,22,24)],"Straight4":[(17,23,25)],"Straight5":[(4,8,12)],"Straight6":[(5,9,13)],"Straight":straight,"badStraight":badStraight,"noBadStraight":noBadStraight,"BentSameDigi":bentSameDigi,
            "badBentSameDigi":badBentSameDigi,"noBadBentSameDigi":noBadBentSameDigi,"BentDiffDigi":bentDiffDigi,"Slabs":[slabs],"ExtraPaths":extraPaths,
            "StraightPlusSlab":straightPlusSlab,"StraightPlusTwoSlab":straightPlusTwoSlab,"StraightPlusThreeSlab":straightPlusThreeSlab,"StraightPlusFourSlab":straightPlusFourSlab,"badStraightPlusSlab":badStraightPlusSlab,"noBadStraightPlusSlab":noBadStraightPlusSlab,"BentSameDigiPlusSlab":bentSameDigiPlusSlab,
            "badBentSameDigiPlusSlab":badBentSameDigiPlusSlab,"noBadBentSameDigiPlusSlab":noBadBentSameDigiPlusSlab,"BentDiffDigi":bentDiffDigiPlusSlab,"Slabs":[slabs],"ExtraPaths":extraPaths,
            "StraightPlusTwoOrMoreSlabs":straightPlusTwoOrMoreSlabs,"badStraightPlusTwoOrMoreSlabs":badStraightPlusTwoOrMoreSlabs,"noBadStraightPlusTwoOrMoreSlabs":noBadStraightPlusTwoOrMoreSlabs,"BentSameDigiPlusTwoOrMoreSlabs":bentSameDigiPlusTwoOrMoreSlabs,
            "badBentSameDigiPlusTwoOrMoreSlabs":badBentSameDigiPlusTwoOrMoreSlabs,"noBadBentSameDigiPlusTwoOrMoreSlabs":noBadBentSameDigiPlusTwoOrMoreSlabs,"BentDiffDigi":bentDiffDigiPlusTwoOrMoreSlabs}
    return allPaths,paths


allPaths, paths = preparePaths()
file = #file
c = r.TChain('t')
c.Add(file)

entries = c.GetEntries()

allowedPaths = []
nPaths46 = 0

for iE in range(entries):
	c.GetEntry(iE)

	if c.beam: continue

	nBarsActivePerLayer = [0,0,0]
	panel = False

	for pulse, layer, itype in zip(c.ipulse, c.layer, c.type):
		if pulse != 0: continue
		if itype == 2: panel = True
		if itype != 0: continue

		nBarsActivePerLayer[layer-1] += 1

	if nBarsActivePerLayer != [1,1,1]: continue 
	if panel: continue

	path = tuple(sorted(set(c.chan)))
	if path not in paths: continue
	if 4 in path or 6 in path:
		nPaths46 += 1 
	allowedPaths.append(path)

print(allowedPaths)