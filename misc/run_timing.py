import ROOT as r
import os,re
from decimal import Decimal
import utilities as u 

def calculateOneRunTime(runDir, numFiles, run, config, ver):
    c = r.TChain('t')

    runPath = '/data/'
    runPath = '/store/user/llavezzo/hsTrees_'+ver+'/'
    
    nFiles = 0

    for ifile in range(numFiles+1):
        fileNum = str(ifile)
        subdir = fileNum[0:1]
        if subdir == '0':
            continue
        file = runPath+"{0}/{1}/UX5MilliQan_Run{2}.{3}_{4}_{5}.root".format(runDir, subdir, run, ifile, config, ver)
        if not os.path.exists(file):
            continue
        nFiles += c.Add(file)
    
    # entries = c.GetEntries()
    entries = nFiles*1000
    print 'added', entries, 'entries'
    runTimes = [-1,-1]
    if entries == 0:
      print 'error loaded 0 entries for run', run
      return runTimes

    previousBeam = True
    beamOnTime = 0
    beamOffTime = 0
    minTempTime = 0
    maxTempTime = 0
    previousTime = 0
    totalRunTimeBeamOn = 0
    totalRunTimeBeamOff = 0

    for iE in range(entries):
        # u.displayProgress(iE,entries)
        c.GetEntry(iE)

        if c.beam != previousBeam or iE == 0 or iE == entries-1 :
            maxTempTime = previousTime

            if c.event_time_fromTDC == -1:
                continue
            # print 'tdc', c.event_time_fromTDC
            if previousBeam:
                # print 'temp', minTempTime, maxTempTime
                beamOnTime = maxTempTime - minTempTime
                totalRunTimeBeamOn += beamOnTime
                # print 'times', previousBeam, beamOnTime

            else:
                # print 'temp', minTempTime, maxTempTime
                beamOffTime = maxTempTime - minTempTime
                totalRunTimeBeamOff += beamOffTime
                # print 'times', previousBeam, beamOffTime



            minTempTime = c.event_time_fromTDC

        previousTime = c.event_time_fromTDC;
        previousBeam = c.beam;

    runTimes[0] = totalRunTimeBeamOn
    runTimes[1] = totalRunTimeBeamOff

    return runTimes


if __name__ == '__main__':
    ver = 'v14'
    fileName = "/home/wintering/misc/runTiming"+ver+".txt"

    runPath = '/data/'
    runPath = '/store/user/llavezzo/hsTrees_'+ver+'/'

    if os.path.exists(fileName):
        os.remove(fileName)
        os.system('touch {0}'.format(fileName))
    else:
        os.system('touch {0}'.format(fileName))
    
    # runs = [1745, 1748, 1751, 1738, 1735, 1742, 1769, 1786, 1803, 1821] 

    runDirectory = ''
    directory = os.listdir(runPath)
    for fname in directory:
        if 'Run' not in fname: continue
        if 'Run' in fname:
            runDirectory = fname
        if runDirectory == '':
            continue

        run = fname[fname.find('n')+1:fname.find('_')]
        run = fname[fname.find('n')+1:len(fname)]
        if run == '': continue
        try:
            run = int(run)
        except: 
            continue
        # if int(run) <= 1378 or int(run) > 1399: continue
        # print run
        # if int(run) not in runs: continue

        # print run
        run = str(run)
        file_config = ''
        fileNums = []
        
        foundSubDirs = False
        for subdir in os.listdir(runPath+fname):
            if len(subdir) != 1: continue
            foundSubDirs = True
            for file in os.listdir(runPath+fname+'/'+subdir):
                fileNum = file[file.find('.')+1:file.find('_',13)]
                file_config = file[file.find('_',13)+1:file.find('_v')]
                ver = file[file.find('_v')+1:file.find('.', file.find('_v'))]
                fileNums.append(int(fileNum))
        if not foundSubDirs: continue

        numFiles = max(fileNums)

        if run == '' or file_config == '' or runDirectory == '': continue

        print runDirectory, numFiles, run, file_config
        run_times = [-1, -1]

        if ver != 'v14': continue

        run_times = calculateOneRunTime(runDirectory, numFiles, run, file_config, ver)
        # print run, run_times[0], run_times[1], run_times[0] + run_times[1]
        if(run_times != [-1,-1]):
            outputFile = open(fileName, 'a+')
            outputFile.write(run + ': ')
            outputFile.write(str(run_times[0]))
            outputFile.write(', ')
            outputFile.write(str(run_times[1]))
            outputFile.write(' \n')
            outputFile.close()
        
