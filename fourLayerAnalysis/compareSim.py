import ROOT as r
import os
import numpy as np

def findHigherHistogram(histos): 
	maxBinHeight = histos[0].GetBinContent(histos[0].GetMaximumBin())
	maxHistoIndex = 0

	for i, h in enumerate(histos):
		if h.GetBinContent(h.GetMaximumBin()) > maxBinHeight:
			maxHistoIndex = i
		
	reorderedHistos = [] 
	reorderedHistos.append(histos[maxHistoIndex])

	for iH in range(len(histos)):
		if iH == maxHistoIndex: continue
		reorderedHistos.append(histos[iH])
	
	return reorderedHistos


def plot1DHistograms(histos, plotName="",
					plotOptions = "", 
					userLegend=None,
					fitType = "expo", 
					fitOptions = "", 
					fitRange=[0,0],
					colorPalette=55):


	drawLegend, logx, logy, logz, showMean = False, False, False, False, False

	drawLegend = 'l' in plotOptions
	logx = 'x' in plotOptions
	logy = 'y' in plotOptions
	logz = 'z' in plotOptions
	showMean = 'm' in plotOptions
	normalize = 'n' in plotOptions
	dotted = 'd' in plotOptions

	fitHisto = fitOptions != ""
	f1 = r.TF1("f1", fitType, fitRange[0], fitRange[1])
	if fitHisto: f1.SetLineWidth(4)

	styleByPalette = len(histos) >= 7
	lineColors = [4, 2, 1, 8, 38, 617]

	r.gStyle.SetOptStat(1)
	r.gStyle.SetPalette(colorPalette)
	drawOption = "e"
	rHistos = histos

	for i in range(len(rHistos)):
		if not styleByPalette: rHistos[i].SetLineColor(lineColors[i])

	if dotted: rHistos[1].SetLineStyle(2)

	if len(histos) > 1: 
		if styleByPalette: drawOption += " PLC PMC"

		if normalize:
			for h in histos:
				h.Scale(1/h.GetEntries())

		rHistos = findHigherHistogram(histos)
	

	cPlot = r.TCanvas("cPlot")
	cPlot.cd()
	if logx: cPlot.SetLogx()
	if logy: cPlot.SetLogy()
	if logz: cPlot.SetLogz()

	cPlot.SetCanvasSize(2100,1500)

	offsetx = 0.10
	offsety = 0.07
	legend = r.TLegend(0.35-offsetx, 0.75-offsety, 0.35+offsetx, 0.75+offsety)
	legend.SetBorderSize(0)

	if userLegend != None:
		legend = userLegend
	
	elif drawLegend:
		for h in rHistos:
			entryLabel = h.GetTitle()
			if showMean: entryLabel += str(h.GetMean())
			legend.AddEntry(h, entryLabel, "l")
		
	
	if fitHisto: rHistos[0].Fit("f1", fitOptions)

	for i in range(len(rHistos)):
		rHistos[i].SetLineWidth(4)
		if not logy: rHistos[i].SetMinimum(0)

		if i == 0: rHistos[i].Draw(drawOption)
		else: rHistos[i].Draw("same hist "+ drawOption)
	

	if drawLegend: legend.Draw()
	if fitHisto: cPlot.Update()

	cPlot.SaveAs(plotName)




def plot2DHistograms(histos, 
				plotName="",
				options = "", 
				colorPalette=51):

	
	if len(histos) > 1: 
		print("Error: trying to plot more than 1 2D plot...")
		exit() 

	logx, logy, logz, showMean = False, False, False, False, 

	logx = 'x' in options
	logy = 'y' in options
	logz = 'z' in options
	normalize = 'n' in options

	r.gStyle.SetPalette(colorPalette)
	r.gStyle.SetOptStat(0)
	drawOption = ""
	rHistos = histos

	drawOption += "colz"

	if normalize:
		pass #FIXME: how to normalize 2D plot?

	cPlot = r.TCanvas("cPlot")
	cPlot.cd()
	if logx: cPlot.SetLogx()
	if logy: cPlot.SetLogy()
	if logz: cPlot.SetLogz()

	cPlot.SetCanvasSize(2100,1500)
	rHistos[0].Draw(drawOption)

	cPlot.SaveAs(plotName)



if __name__ == '__main__':
	
	r.gROOT.SetBatch(True)
	inputDir = "signalSelection/"
	inputDir = ""
	f_data = inputDir + "data/data_4layer/o_plots/analyzePlots_3layers_oneHitPerLayer_data.root"
	f_mc = inputDir + "data/sim_4layer/o_plots/analyzePlots_3layers_oneHitPerLayer_sim.root"
	outputDir = "data/comparePlots/o/"

	data = r.TFile(f_data)
	mc = r.TFile(f_mc)

	optionsDict = {
		'hTime': 'ynl',
		'hTime_L1L2': 'ynl',
		'hTime_L2L3': 'ynl',
		'hTime_L2L3': 'ynl',
		'hTime_L3L4': 'ynl',
		'hTime_L3L4_20': 'ynl',
		'hTime_Slab21_L3': 'ynl',
		'hTime_Slab21_L4': 'ynl',
		'hNHits0': 'ynl',
		'hNHits1': 'ynl',
		'hNHits2': 'ynl',
		'hNHits3': 'ynl',
		'hNumSheetHits': 'nl',
		'hSheets': 'nl',
		'hAreaL0': 'ynl',
		'hAreaL1': 'ynl',
		'hAreaL2': 'ynl',
		'hAreaL3': 'ynl',
		'hAfter15ns': 'nl',
		'hAfter20ns': 'nl',
		'hAfter30ns': 'nl',
		'sig_nPE_2and1': 'nl',
		'sig_nPE_2or1': 'nl',
		'hMiddleGivenTop': 'nld',
		'hMiddleGivenBottom': 'nld'
	}

	histos_pairs = [[dKey.GetName(),mcKey.GetName()] for dKey, mcKey in zip(data.GetListOfKeys(), mc.GetListOfKeys())]

	print(histos_pairs)

	for histos in histos_pairs:
		dataHist = histos[0]
		mcHist = histos[1]

		hData = data.Get('{}'.format(dataHist))
		hMC = mc.Get('{}'.format(mcHist))

		if '2D' in str(type(hData)): continue # dont want to plot 2D on same axes
		if hData.GetEntries() == 0:
			print('data plot {} is empty...'.format(dataHist))
			continue
		elif hMC.GetEntries() == 0:
			print('mc plot {} is empty...'.format(mcHist))
			continue

		plotOptions = 'nl'
		if dataHist in optionsDict: 
			plotOptions = optionsDict[dataHist]

		legend = r.TLegend(0.7, 0.62, 1.0, 0.82)
		legend.SetBorderSize(0)

		legend.AddEntry(hData, 'Data', 'l')
		legend.AddEntry(hMC, 'Sim', 'l')

		# if dataHist != "14": continue

		plot1DHistograms([hData, hMC], plotName=outputDir+dataHist+".png", plotOptions=plotOptions, userLegend=legend)

