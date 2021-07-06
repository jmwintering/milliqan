import ROOT as r
import os
import numpy as np 

if __name__ == '__main__':
	
	efficiencyLabel = 'all_physics_beam.csv'
	signal = False
	if signal: print("SIG INJECT MODE")
	inputFile = #file

	debugEvent = False
	comp_run = 1378
	comp_file = 1047
	comp_event = 903

	c = r.TChain('t')
	c.Add(inputFile)
	entries = c.GetEntries()
	print('Added {0} entries'.format(entries))

	isStraightPath = False
	minNPE = 1e9
	maxNPE = -1
	minSlabNPE = 1e9
	maxSlabNPE = -1
	minBarNPE = 1e9
	maxBarNPE = -1
	maxDeltaT = -1
	minDeltaT = 1e9
	maxSlabDeltaT = -1
	minSlabDeltaT = 1e9

	nCuts = 17
	count = [0]*nCuts


	for iE in range(nEntries):
		c.GetEntry(iE)

		flag = debugEvent and c.run == comp_run and c.file == comp_file and c.event == comp_event

		# quiet RMS
		quiet = True
		for i, rms in enumerate(c.sideband_RMS):
			if i == 15: continue
			if rms > 1.3: quiet = False
		if not quiet: continue

		if not signal:
			if c.groupTDC_b0[0] != c.groupTDC_b1[0]: continue
			if c.beam: continue
			c.scale1fb = 1

		failNPEcut, failFirstPulseMax, failPulseTimeSelection, slabVeto = False, False, False, False
		slabsActive = 0
		nBarsActiveInLayer = [0,0,0]

		for ipulse, nPE, time, chan, layer, row, col, itype in zip(c.ipulse, c.nPE, c.time_module_calibrated,
																  c.chan, c.layer, c.row, c.column, c.type):
			if chan == 15: contunue

			if nPE < 0.5: 
				failNPEcut = true
				break

			





































