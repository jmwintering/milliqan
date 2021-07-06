import ROOT as r

c = r.TChain('t')
c.Add('/store/user/llavezzo/HS_Run1369/*.root')

entries = c.GetEntries()
print 'Added', entries

for iE in range(entries):
    c.GetEntry(iE)

    for iH in c.v_hs:
        print iH
