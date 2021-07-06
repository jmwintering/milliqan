import os,re



runs = []
runFile = open('/data/refData/physicsRuns2018Matt.txt','r')
for line in runFile:
    runs.append(line.split()[0])

beamOn = 0
beamOff = 0

timesFile = open('/data/refData/runTimes_all_v16.txt', 'r')
for line in timesFile:
    run = line.split()[0][0:4]
    onTime = line.split()[1].replace(',','')
    if run in runs:
        beamOn += float(onTime)
        beamOff += float(line.split()[2])

print 'beam on:', beamOn, 's', beamOn/3600, 'hr'
print 'beam off:', beamOff, 's', beamOff/3600, 'hr'