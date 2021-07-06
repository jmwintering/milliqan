import ROOT as r
import progressbar


input_file = #FILE

c = r.TChain('t')
c.Add(input_file)
entries = c.GetEntries()

t_first, t_last = 1e90, -1

for i in progressbar.progressbar(range(entries), suffix='{seconds_elapsed:.1}'):
    c.GetEntry(i)

    if c.event_time_fromTDC > t_last: t_last = c.event_time_fromTDC
    if c.event_time_fromTDC < t_first: t_first = c.event_time_fromTDC

total = t_last - t_first 
total_hr = total/3600
print('total time (s): ', total)
print('total time (h): ', total_hr)