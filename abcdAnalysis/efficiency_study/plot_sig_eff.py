import ROOT as r
import os
import numpy as np

masses = ["0p02", "0p007", "0p7", "0p14", "3p0", "0p1"]
allowedCharges = ["0p005","0p007", "0p01", "0p02","0p05","0p1","0p2", "0p03"]
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


def plotEfficiencies(mass, slab):

    cutValues = []
    chargeHistos = []
    chargeNames = []
    charges = []

    # first sort charges so every legend is same
    fileDir = "cut_files/signal/noslab/"
    if slab: fileDir = "cut_files/signal/slab/"

    for file in os.listdir(fileDir):
        if mass not in file: continue

        charge = file[file.find("q")+1:file.find("cut", file.find("q"))]
        if charge not in allowedCharges: continue
        charge = charge.replace("p", ".")

        if charge != None:
            charges.append(float(charge))

    if charges == []: return
    charges = sorted(charges, key=float)
    charges = [str(i).replace(".", "p") for i in charges]
    
    # now actually load cut values
    for ich in charges:
        file = fileDir+mass+"_q"+ich+"cut_values.txt"

        denominator = 1
        chargeCutArray = np.loadtxt(file, delimiter=":")
        denominator = chargeCutArray[0]
        chargeCutValues = [val/denominator for val in chargeCutArray]
        cutValues.append(chargeCutValues)

        # plot cut values in hist
        chargeHist = r.TH1D(ich, ich, nCuts, 1, nCuts+1)
        for i, value in enumerate(chargeCutValues):
            chargeHist.Fill(i+1, value)
    
        chargeHistos.append(chargeHist)
        chargeNames.append(ich)

    # plot histos on canvas
    c = r.TCanvas("c")
    c.cd()
    # c.SetLogy()
    c.SetGrid()

    # set bin labels to cut names / format histos
    for ilab, label in enumerate(cutNames):
        for ihist, hist in enumerate(chargeHistos):
            hist.GetXaxis().SetBinLabel(ilab+1, label)
        
    leg = r.TLegend(0.62,0.67,0.85,0.90)

    r.gStyle.SetOptStat(0)
    r.gStyle.SetPalette(55)

    if cutValues == []: return
    maxYvalue =  1.5* max(map(max, *cutValues))
    minYvalue = (0.25) * min(map(min, *cutValues))
    if minYvalue < 1e-20: minYvalue = -0.05
    maxYvalue = 1.3
    # minYvalue = 1e-6
    # maxYvalue = 1.1

    # print(maxYvalue, minYvalue)
    # print(cutValues)
   
    for ihist, hist in enumerate(chargeHistos):
        hist.GetYaxis().SetTitle("Efficiency w.r.t. initial events")
        hist.SetTitle('Cut efficiencies for mass '+mass)
        hist.SetLineWidth(5)
        hist.SetMarkerStyle(8)
        hist.SetMarkerSize(1.3)
        hist.SetMarkerColor(ihist+1)
        hist.SetLineColor(ihist+1)
        hist.GetYaxis().SetRangeUser(minYvalue, maxYvalue)
        hist.GetXaxis().SetRangeUser(1, nCuts+1)

        leg.AddEntry(hist, chargeNames[ihist], "l")

        if(ihist == 0): hist.Draw('PLC PMC p l hist')
        else: hist.Draw('same PLC PMC p l hist')

    # input()
    # return 
    
    printString = "eff_plots/signal/noslab/"+mass+"_eff.pdf"
    if slab: printString = "eff_plots/signal/slab/"+mass+"_eff.pdf"
    leg.SetNColumns(2)
    leg.Draw()

    # input()
    c.SaveAs(printString)
   
    cLeg = r.TCanvas("cLeg")
    cLeg.cd()
    leg.Draw()
    legendString = "eff_plots/signal/noslab/"+mass+"_leg.pdf"
    if slab: legendString = "eff_plots/signal/slab/"+mass+"_leg.pdf"
    # cLeg.SaveAs(legendString)


if __name__ == "__main__":
    
    for mass in masses:
        # if "3p0" not in mass: continue # testing

        massString = "m"+mass
        print("mass: ",massString)

        for slab in [True, False]:
            plotEfficiencies(massString, slab)