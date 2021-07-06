import ROOT as r 
import utilities as u

c = r.TChain('t')
c.Add('/data/slabskims/*.root')
n = c.GetEntries()

if n == 0:
    raise ValueError("Error in <TChain>: 0 Entries found")

# make plots for all channels?
slabNPE = r.TH1D('slabNPE', 'slabNPE', 100, 0, 5000)
hSheetNPE = r.TH1D('sheetNPE', 'sheetNPE', 100, 0, 5000)

muonBarCuts = [400]*32
cosmicBarCuts = [0]*32
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

nMuons = 0
nPmts = 0

for iE in range(n):
    u.displayProgress(iE,n)
    c.GetEntry(iE)

    npeCuts = [0]*32
    if c.beam:
        npeCuts = muonBarCuts
    else:
        npeCuts = cosmicBarCuts

    sheetNPE = 0
    slabsHit = 0

    barRows = []
    barCols = []
    barLayers = []
    nBars = 0

    for ipulse, itype, nPE, layer, col, row, time, chan in zip(c.ipulse, c.type, c.nPE, c.layer, c.column, c.row, c.time_module_calibrated, c.chan):
        if ipulse != 0: continue

        if itype == 1 and nPE > 300:
            slabsHit += 1
        if itype == 2:
            sheetNPE += nPE

        if itype == 0 and nPE > npeCuts[chan]:
            nBars += 1
            barRows.append(row)
            barCols.append(col)
            barLayers.append(layer)


    #  select muon like events
    if sheetNPE > 200: continue
    if slabsHit != 4: continue

    # straight paths
    straightPath = False
    if len(set(barCols)) == 1 and len(set(barRows)) == 1 and len(set(barLayers)) == 3: straightPath = True
    if nBars ==3: print set(barLayers), set(barCols), set(barRows)
    
    nMuons += 1
    if straightPath: nPmts +=1

print 'nMuons', nMuons
print 'nPmts', nPmts


    

    

