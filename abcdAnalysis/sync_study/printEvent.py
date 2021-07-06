import sys
import ROOT as r 
import numpy as np

if len(sys.argv) < 4: 
    print('run file event')
    exit()

smear = r.TRandom1()

def GetTreeIndex(tree, runVar, fileVar, eventVar, entries):
	nentries = entries
	hist = r.TH1D("hist","", nentries,0,entries)
	draw_selection = "run=={0} && file=={1} && event=={2}".format(runVar,fileVar, eventVar)
	draw_command = "Entry$ >> hist".format(nentries,nentries)
	tree.Draw(draw_command,draw_selection,"goff")
	iEntry = hist.GetBinLowEdge(hist.FindFirstBinAbove(0))
	return int(iEntry)


def tubeSpecies(ch):
  
	tubeSpecies = ["878","878","878","878",             # 0 1 2 3 
	     "878","7725","878","878",       # 4 5 6 7   
	     "878","ET","878","878", # 8 9 10 11
	     "878","878","878","",    # 12 13 14 15
	     "878","ET","878","878", # 16 17 18 19 
	     "878","878","7725","878",   # 20 21 22 23
	     "ET","ET","878","878",  # 24 25 26 27
	     "878","878","878","878"]    # 28 29 30 31

	tube = tubeSpecies[ch]
	if tube == "878": return 0
	elif tube == "7725": return 2
	elif tube == "ET": return 1
	else: return -1


def correctTime(area, tube, time):
	area_bins = [10, 20, 40, 70, 150, 250, 400, 750, 1400, 2500, 4500, 9000, 15000, 30000, 50000, 80000, 150000, 300000]

	data_t_878 = [-34.15377870074264,-34.15377870074264,-33.19892473118279,-32.543103448275865,-24.27463503649635,-22.660398230088497,-19.454957356076758,-17.85125968992248,-15.731502242152466,-14.28806954436451,-12.805232558139535,-10.966954022988507,-10.071428571428571,-7.831807780320366,-5.412649219467402,-2.5579132690882136,1.510488704472107,3.99234693877551]
	data_t_ET = [-14.015949746665287,-13.907894736842104,-14.761029411764707,-14.841772151898734,-12.463235294117647,-11.885245901639344,-10.847826086956522,-9.676724137931034,-9.185855263157896,-8.754310344827585,-7.692039106145251,-7.1875,-6.567796610169491,-6.075,-5.071480938416422,4.056298533218292,4.056298533218292,4.056298533218292]
	data_t_7725 = [-17.378660436137075,-17.378660436137075,-17.378660436137075,-15.675,-16.166666666666668,-14.0625,-13.1625,-11.61111111111111,-10.428571428571429,-9.659090909090908,-8.75,-7.757352941176471,-6.357758620689655,-5.791083916083916,-2.431318681318681,2.2483031674208145,2.2483031674208145,2.2483031674208145]

	mc_t_7725  = [-11.25,-11.25,-11.25,-9.546339563862928,-9.174180327868852,-9.125,-8.369200244200243,-7.484080841638981,-6.681924101198402,-6.033905704307334,-5.200774754346183,-4.346945137157107,-3.0809859154929575,-1.3670602605863191,0.7314542046063203,3.1493492684586593,1.6796875,1.6796875]
	mc_t_ET = [-6.875,-6.7669449901768175,-6.617595204513399,-6.374109528049867,-5.9316458519179305,-5.396926389976508,-4.597265422498704,-3.881748994830557,-3.186450839328537,-2.5162253374870196,-1.629910119840213,-0.6707857424182749,0.1616877585787296,0.9882438520743383,2.096875,3.8074709564024523,3.8074709564024523,3.8074709564024523]
	mc_t_878 = [-24.08653846153846,-24.08653846153846,-23.13168449197861,-18.2379617414248,-16.16713771839671,-14.251368440921102,-12.478297719334293,-10.4748006833713,-8.714937666963491,-7.051212539432177,-5.298327570155606,-3.8523175542406314,-2.5033256880733945,-1.335558707053409,-0.5196643370741214,1.0823906052601355,3.0854433323411223,3.0854433323411223]

	smear_878 = [ 32.68470887875327,32.68470887875327,32.68470887875327,13.706835101583811,8.33349934522336,5.642388901746749,5.109635021899302,4.123179805146569,3.4388251496794267,3.240391437474683,2.8771786686068896,2.797089556343491,2.8832976661969254,2.747314172564764,2.6206972121661836,2.8041144859693716,2.681256486727365,2.1193456879146453]
	smear_ET = [11.867659706549142,11.867659706549142,6.395054617831197,6.8131100819612,3.998393342402683,3.2778329425910884,3.3188810430700393,2.9059334314856584,1.9285285756979043,2.3595632550377363,2.3063215836805298,2.3923942765372193,2.56622676999796,2.276034905975209,2.70843217674636,2.0665625263459826,2.0665625263459826,2.0665625263459826 ]
	smear_7725 = [6.006014002681707,6.006014002681707,6.006014002681707,6.006014002681707,5.484275575419314,4.362016690643299,3.2570068232272926,4.129467447034762,3.318410727134852,3.075862737617618,3.5194446271222253,2.2133211947697995,2.8631559390852606,2.789813628072842,3.251305313835265,1.7812300677982842,1.7812300677982842,1.7812300677982842]

	corrected_time = time
	if tube < 0: return time

	corrections = data_t_878
	smears = smear_878

	if tube == 1:
		smears = smear_ET
		corrections = data_t_ET
	elif tube == 2:
		smears = smear_7725
		corrections = data_t_7725

	for i, ibin in enumerate(area_bins):
		corr = corrections[i] 
		sigma = smears[i]

		if i == len(area_bins):
			if area >= ibin:
				# print("corrlast", corr)
				corrected_time += corr
				# corrected_time += np.random.normal(0,sigma)
				# corrected_time += smear.Gaus(0, sigma)
				return corrected_time
		else:
			if area >= ibin and area < area_bins[i+1]:
				corrected_time += corr
				print("corrmiddle", area, tube, time, corr, corrected_time)
				# corrected_time += np.random.normal(0, sigma)
				# corrected_time += smear.Gaus(0, sigma)
				return corrected_time
		if i == 0:
			if area < ibin:
				# print("corrfirst", corr)
				corrected_time += corr
				# corrected_time += np.random.normal(0, sigma)
				# corrected_time += smear.Gaus(0, sigma)
				return corrected_time
    

c = r.TChain('t')
c.Add(#FILE)

run = int(sys.argv[1])
file = int(sys.argv[2])
event = int(sys.argv[3])

entryNum = GetTreeIndex(c, run, file, event, c.GetEntries())
c.GetEntry(entryNum)

if c.run == run and c.file == file and c.event == event:

    deltaTimes = []
    straightPath = True
    for iChan, ipulse, iTime, iType, iRow, iCol, iArea in zip(c.chan, c.ipulse, c.time_module_calibrated, c.type, c.row, c.column, c.area):
        for jChan, jpulse, jTime, jType, jRow, jCol, jArea in zip(c.chan, c.ipulse, c.time_module_calibrated, c.type, c.row, c.column, c.area):
            if ipulse != 0 or jpulse != 0: continue
            if iChan == jChan: continue 
            if iType != 0 or jType != 0: continue
            if iChan == 15 or jChan == 15: continue
            if iRow != jRow or iCol != jCol: straightPath = False
            timei = correctTime(iArea, tubeSpecies(iChan), iTime)
            timej = correctTime(jArea, tubeSpecies(jChan), jTime)
            print(timei, timej)
            deltaTimes.append(abs(timei - timej))
    print('\n')

    correctedTimes = []
    for area, chan, time in zip(c.area, c.chan, c.time_module_calibrated):
    	cortime = correctTime(area, tubeSpecies(chan), time)
    	print("c", cortime, area, tubeSpecies(chan), time)
    	correctedTimes.append(cortime)


    print("chan      || ",c.chan)
    print("layer     || ",c.layer)
    print("type      || ",c.type)
    print("ipulse    || ",c.ipulse)
    print("nPE       || ",c.nPE)
    print("area      || ",c.area)
    print("time(corr)|| ",correctedTimes)
    print("time(cali)|| ",c.time_module_calibrated)
    print("timediff  || ",[(corr - uncorr) for corr,uncorr in zip(correctedTimes, c.time_module_calibrated)])
    print("time      || ",c.time, "\n")

    print("sideband || ",c.sideband_RMS,"\n")
    print("max delta t: ",max(deltaTimes))
    print("straight path: ",straightPath)
    print('beam: ', c.beam)


