import ROOT as r
import os
import numpy as np


nCuts = 10
cutNames = [
    "At least one hit per layer",
    "One hit per layer",
    "No pre pulse",
    "First pulse is max",
    "No sheets",
    "Max/min nPE < 10", 
    "Slab cut",
    "Min NPE",
    "Straight path",
    "Max delta t < 15"
]

def plotEfficiencies(slab):

    cutValues = []
    denominator = 1
    
    # load cut values
    file = ''
    if slab: file = 'cut_files/data/slab/allPhysics_slab_cut_values.txt'
    else: file = 'cut_files/data/noslab/allPhysics_noslab_cut_values.txt'

    cutArray = np.loadtxt(file, delimiter=":")
    denominator = cutArray[0]

    cutValues = [val/denominator for val in cutArray]

    # plot cut values in hist
    hist = r.TH1D('all', 'all', nCuts, 1, nCuts+1)
    for i, value in enumerate(cutValues):
        hist.Fill(i+1, value)
    
    # plot histos on canvas
    c = r.TCanvas("c")
    c.cd()
    c.SetLogy()
    c.SetGrid()

    # set bin labels to cut names / format histos
    for ilab, label in enumerate(cutNames):
        hist.GetXaxis().SetBinLabel(ilab+1, label)
        
    r.gStyle.SetOptStat(0)
    r.gStyle.SetPalette(55)

    if cutValues == []: return
    minYvalue = 1.5*max(cutValues)
    maxYvalue =0.5* min(cutValues)
    
    hist.GetYaxis().SetTitle("Efficiency w.r.t. one hit per layer cut")
    hist.SetTitle('Cut efficiencies for no beam (no slab)')
    if slab: hist.SetTitle('Cut efficiencies for no beam (at least 1 slab)')
    hist.SetLineWidth(5)
    hist.SetMarkerStyle(8)
    hist.SetMarkerSize(1.3)
    hist.SetMarkerColor(1)
    hist.SetLineColor(1)
    # ist.GetYaxis().SetRangeUser(minYvalue, maxYvalue)
    hist.GetXaxis().SetRangeUser(1, nCuts+1)
    hist.Draw('PLC PMC p l hist')

    printString = "eff_plots/data/noslab/allPhysics_noBeam_noSlab.pdf"
    if slab: printString = "eff_plots/data/slab/allPhysics_noBeam_atLeastOneSlab.pdf"
    c.SaveAs(printString)
   

if __name__ == "__main__":
    plotEfficiencies(True)
    plotEfficiencies(False)