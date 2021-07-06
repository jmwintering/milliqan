import requests
import sys 
import utilities as u 

def findTiming(runHtml, run):
    totalTimeForRun = 0
    runInfo = htmlString[htmlString.find('Livetime')+9:htmlString.find('sec', htmlString.find('Livetime')+8)]
    try: totalTimeForRun = float(runInfo)
    except: 
        print 'Error finding timing for run', run
        return 0
    return totalTimeForRun

def findLumi(runHtml, run):
    totalLumiForRun = 0
    lumiInfo = htmlString[htmlString.find('Lumi')+5:htmlString.find('/pb')]
    try: totalLumiForRun = float(lumiInfo)
    except: 
        print 'Error finding lumi for run', run
        return 0
    return totalLumiForRun

if __name__ == "__main__":
    # settings
    useRunList = False
    useAllRuns = True 

    calcTiming = True
    calcLumi = True

    # user defined run
    manualRun = 1000

    runList = [1000, 1012]
    if not useRunList:
        runList = [manualRun]
    
    # run from command line argument
    if len(sys.argv) > 1:
        manualRun = sys.argv[1]
        try: manualRun = int(manualRun)
        except: 
            print 'Error: please enter a valid run (integer)'
            exit()
        runList = [str(manualRun)]
    
    # all runs availible
    if useAllRuns:
        for i in range(1000,2114):
            runList.append(str(i))

    if len(runList) == 0: 
        print 'Error: No runs added to run list'
        exit()
    
    runTimes = {}
    runLumis = {}

    for i,run in enumerate(runList):
        u.displayProgress(i, len(runList))

        run = str(run)
        url = 'http://cms2.physics.ucsb.edu/milliqanValidationPlots/Run'+run+'/index.html'
        notFound = 'The requested URL /milliqanValidationPlots/Run'+run+'/index.html was not found on this server.'
        r = requests.get(url)
        htmlString = r.text
        if notFound in htmlString:
            print 'Validation timing for run', run, 'not found'
            continue
        
        thisRunTime = 0
        thisRunLumi = 0
        if calcTiming:
            thisRunTime = findTiming(htmlString,run)
            runTimes[run] = thisRunTime
        if calcLumi:
            thisRunLumi = findLumi(htmlString,run)
            runLumis[run] = thisRunLumi
            
    print '\nNote: Units for time are sec. and units for lumi are /pb unless otherwise noted'
    print runTimes, runLumis

    if True:
        tempFile = open('validTimeLumiDicts.txt', 'w')
        tempFile.write(str(runTimes))
        tempFile.write('\n')
        tempFile.write(str(runLumis))