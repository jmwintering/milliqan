import ROOT as r
import os

r.gROOT.SetBatch(1)

def getEntriesAbove(hist, aboveNum):

	firstBin = hist.FindFixBin(aboveNum)
	nEntriesAbove = 0
	nPercentAbove = 0
	for i in range(firstBin, hist.GetNbinsX()):
		nEntriesAbove += hist.GetBinContent(i)

	minusFirstBin = hist.FindFixBin(-aboveNum)
	for i in range(1, minusFirstBin):
		nEntriesAbove += hist.GetBinContent(i)

	nPercentAbove = ((nEntriesAbove*1.0)/hist.GetEntries())*100
	return [int(nEntriesAbove), nPercentAbove]

valuesAbove = [15,18,20, 30]

valDir = 'methodFiles/'
for path in os.listdir(valDir):
	if 'valplots' not in path: continue
	if 'uncorr' in path: continue

	chmass = path[path.find("_")+1:path.find(".")]
	if "m1p0" not in chmass: continue

	fIn = r.TFile(valDir+path, "read")
	h878 = fIn.Get("h878")
	h878_ET = fIn.Get("h878_ET")
	h878_7725  = fIn.Get("h878_7725")
	hET = fIn.Get("hET")
	hET_7725 = fIn.Get("hET_7725")
	hAllTubes = fIn.Get("hAllTubes")

	r.gStyle.SetOptStat(0)
	# r.gStyle.SetImageScaling(3.)

	h878.SetTitle(chmass)
	h878.GetXaxis().SetTitle("t(hit 1) - t(hit 2) [ns]")
	h878.GetYaxis().SetTitle("Count")

	hAllTubes.SetTitle(chmass+" (all tubes)")
	hAllTubes.GetXaxis().SetTitle("t(hit 1) - t(hit 2) [ns]")
	hAllTubes.GetYaxis().SetTitle("Count")

	h878.SetLineColor(1)
	h878_7725.SetLineColor(4)
	h878_ET.SetLineColor(2)
	hET.SetLineColor(8)
	hET_7725.SetLineColor(6)

	h878.SetLineWidth(5)
	h878_7725.SetLineWidth(5)
	h878_ET.SetLineWidth(5)
	hET.SetLineWidth(5)
	hET_7725.SetLineWidth(5)

	hAllTubes.SetLineWidth(5)


	c1 = r.TCanvas("c1", "c1", 0, 1000, 1200, 1000)
	c1.cd()

	legend = r.TLegend(0.77,0.7,0.9,0.9)
	legend.AddEntry(h878, "", "l")
	legend.AddEntry(h878_7725, "878 & 7725", "l")
	legend.AddEntry(h878_ET, "878 & ET", "l")
	legend.AddEntry(hET, "ET", "l")
	legend.AddEntry(hET_7725, "ET & 7725", "l")

	h878.Draw()
	h878_7725.Draw('same')
	h878_ET.Draw('same')
	hET.Draw('same')
	hET_7725.Draw('same')

	legend.Draw()
	
	c1.SaveAs("closurePlots/sigInject/"+chmass+".png")

	c2 = r.TCanvas("c2", "c2", 0, 1000, 1200, 1000)
	c2.cd()
	hAllTubes.Draw()
	c2.SaveAs("closurePlots/sigInject/"+chmass+"_all.png")

	valuesAboveFile = open('valuesAbove_all.txt', 'a')
	valuesAboveFile.write('\n'+chmass)
	valuesAboveFile.write('\nAll tube species:\n')
	for value in valuesAbove:
		valuesAboveFile.write('{0}: {1} ({2:.2f}%)\n'.format(value, getEntriesAbove(h878, value)[0], getEntriesAbove(h878, value)[1]))
	valuesAboveFile.close()












