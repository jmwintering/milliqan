import ROOT as r
import numpy as np
import os, re
import collections
import time
import statistics
import operator


# globals
NMODROWS = 9
NMODCOLS = 6
NLAYERS = 4
NROWS = NMODROWS*2
NCOLS = NMODCOLS*2


def bar_coords(bar):

    # (invariant under layer change)
    l_bar = bar
    if bar % 216 == 0: l_bar = 216
    else: l_bar = bar % 216
        
    # module coordinates
    m_col = np.ceil(l_bar*1.0/36)-1
    if l_bar % 36 == 0: l_bar = 36
    else: l_bar = l_bar % 36
    m_row = np.ceil(l_bar*1.0/4)-1
    
    # bar coordinates within module
    if l_bar % 4 == 0: l_bar = 4
    else: l_bar = (l_bar % 4)
    
    m_coords = [-1,-1]
    if l_bar == 1: m_coords = [1,1]
    elif l_bar == 2: m_coords = [1,0]
    elif l_bar == 3: m_coords = [0,1]
    elif l_bar == 4: m_coords = [0,0]

    # global bar coordinates in (18, 12) space (invariant under layer change)
    coords = [int((m_row)*2 + m_coords[0]), int((m_col)*2 + m_coords[1]), int(np.ceil(bar*1.0/216))]
    return coords


# takes bar coordinates of "first" bars in each layer and finds a straight path
# (allows for channels b away to be hit but no closer)
# returns the path(s) if one is found or an empty list if one isn't
def find_candidate_path(bar_coords):
    bar_local_coords = [tuple(x[:2]) for x in bar_coords]
    pair_freqs = collections.Counter(bar_local_coords)
    can_pairs = [list(key) for key, x in pair_freqs.items() if x == NLAYERS]
    if len(can_pairs) == 0: return []
    can_coords = []
    for pair in can_pairs:
        for l_pair in bar_coords:
            if pair == l_pair[:2]: can_coords.append(l_pair)

    target_coords = []
    for coord in can_coords:
        spans_all_layers = len(set([x[2] for x in can_coords if x[:2] == coord[:2]])) == NLAYERS
        if spans_all_layers: 
            target_coords.append(tuple(coord[:2]))
    target_coords = [list(t) for t in set(target_coords)]
    if len(target_coords) == 0: return []

    b = [1, 1] # "distance of closest approach" allowed to path(s) (in # of channels)
    good_paths = []
    for i, target in enumerate(target_coords): 
        fails = [False, False]
        for coord in bar_coords:
            if coord[:2] == target: continue
            for x in [0, 1]:
                if abs(coord[x]-target[x]) <= b[x]: 
                    fails[x] = True
        if not all(fails): good_paths.append(target)

    target_paths = []
    for t in good_paths:
        target_paths.append([x for x in can_coords if x[:2] == t])

    return target_paths
		


inputFile = 'data/DRStest_v21.root'
c = r.TChain('t')
c.Add(inputFile)
nEvents = c.GetEntries()


counts = {
	"atLeastOneHitPerLayer" : 0,
	"quietSideband" : 0,
	"failsPanelVeto"  : 0,
	"failsFirstPulseMax"  : 0,
	"failPrepulse"  : 0,
	"failPrepulseSlab"  : 0,
	"failMaxMin"  : 0,
	"failTimingSel"  : 0,
	"failPath" : 0
}

for iE in range(nEvents):
	c.GetEntry(iE)

	cuts = dict.fromkeys(counts, False)
	
	nPanelsActive, nSlabsActive, firstInLayer, largeSlabHit = 0, 0, False, False
	first_pulses = []
	slab_pulses = []

	for ipulse, itype, npe, chan, time, layer in zip(c.ipulse, c.type, c.npe, c.chan, c.time, c.layer):
		if npe < 0.5: continue

		firstInLayer = time == min([t for t, l, n in zip(c.time, c.layer, c.npe) if l == layer and n > 0.5])
		firstInChannel = time == min([t for t, c, n in zip(c.time, c.chan, c.npe) if c == chan and n > 0.5])

		if itype == kBar:
			if firstInLayer:
				first_pulses.append([time, chan, npe, layer])

		elif itype == kPanel:
			if firstInChannel:
				nPanelsActive += 1
		elif itype == kSlab: 
			largeSlabHit = npe > 250 
			if firstInChannel:
				if npe < 250: nSlabsActive += 1
				slab_pulses.append([time, chan, npe])
	
	cuts['atLeastOneHitPerLayer'] = len(set([x for x in c.layer if x > -1])) >= NLAYERS
	cuts['quietSideband'] = not any(s > 1.3 for s in c.sideband_RMS)
	cuts['falsePanelVeto'] = nPanelsActive <= 0

	# first pulse by time must also be largest by NPE
	for layer in range(NLAYERS):
		layer_pulses = first_pulses[np.where(first_pulses[:,3]==layer)]
		if (np.where(layer_pulses[:,0] == np.min(layer_pulses,0)) != np.where(layer_pulses[2,:] == np.max(layer_pulses,2))):
			cuts['failsFirstPulseMax'] = True

	# no pre pulses
	cuts['failPrepulse'] = any(t < 200 for t in first_pulses[:, 0])
	cuts['failPrepulseSlab'] = any(t < 200 for t in slab_pulses[:, 0])

	maxBarNPE, minBarNPE = max(first_pulses[:, 2]), min(first_pulses[:,2])
	cuts['failMaxMin'] = maxBarNPE/minBarNPE <= 10

	# timing selection
	time_diffs = []
	for i, t1 in enumerate(first_pulses[:,0]):
		for j, t2 in enumerate(first_pulses[:,0]):
			if i == j: continue
			time_diffs.append(abs(t1-t2))
	maxDeltaT = max(time_diffs)
	cuts['failTimingSel'] = abs(maxDeltaT) > timingSel and abs(abs(maxDeltaT/timingSel)-1) > 0.01 and abs(maxDeltaT/timingSel - 1) < 0.01
	
	bar_global_coords = [bar_coords(x) for x in first_pulses[:,1]]
	can_path = find_canidate_path(bar_global_coords)
	cuts['failPath'] = len(can_path) == 0


	# make cuts
	for cutName, cutBool in cuts.items():
		if not cutBool: continue
		else: counts[cutName] += 1

		if cutName == "failPulse":
		
	'''
	- plot at any cut
	- reference by name
	- change the order of cuts easily
	- omit 1 or more cuts
	- for each cut can take subset of events passing cut
	'''