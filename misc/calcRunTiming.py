import os,re
import utilities as u 


def calcUsingValidationPlots(runWanted):
    beamOffTime = -1
    validFile = open('validationHtml.txt', 'r')
    run = ''
    time = ''
    for line in validFile:
        if 'Run' in line:
            run = line[line.find('Run'):line.find('Run')+9]
            run = re.sub("[^0-9]", "", run)
            
            if run == '': continue
            run = int(run)
        
        if run != int(runWanted): continue

        if 'hr' in line and 'evts' in line:
            time = line[line.find('hr')-9:line.find('hr')] 
            time = time.replace(' ', '')    
            time = re.sub("[^0-9 .]", "", time)  
            
            if time == '': continue
            time = float(time)
        
            # print run, time
            beamOffTime = time
        
    
        if 'hour' in line and 'evts' in line:
            time = line[line.find('hour')-10:line.find('hour')]   
            time = time.replace(' ', '')  
            time = re.sub("[^0-9]", "", time)

            if time == '': continue
            time = float(time)
            # print run, time
            beamOffTime = time


    validFile.close()
    return  beamOffTime


def createRunTimeDicts(runFiles):
    beamOnTimeDict = {}
    beamOffTimeDict = {}

    print runFiles
    for ifile in runFiles:
        openFile = open(ifile, 'r')
        for line in openFile:
            beamOnTime = 0
            beamOffTime = 0

            run = line.split()[0]
            beamOnTime = line.split()[1]
            beamOffTime = line.split()[2]
            run = run.replace(':','')
            beamOnTime = beamOnTime.replace(',','')
            run = int(run)
            beamOnTime = float(beamOnTime)
            beamOffTime = float(beamOffTime)

            beamOnTimeDict[run] = beamOnTime
            beamOffTimeDict[run] = beamOffTime
        openFile.close()

    return beamOnTimeDict, beamOffTimeDict



if __name__ == "__main__":

    runs = [2049, 2052, 2056, 2059, 2063, 2064, 2066, 2067, 2068, 2069, 2070, 2071, 2072, 2075, 2079, 2082, 2086, 2087, 2089, 2090, 2091, 2092, 2093, 2094, 2095, 2098, 2102, 2105, 2110, 2112, 2113, 2114, 2115, 2116, 1619,
 1620, 1621, 1626, 1627, 1628, 1629, 1630, 1631, 1661, 1730, 1732, 1733, 1734, 1736, 1737, 1739, 1740, 1741, 1743, 1744, 1746, 1747, 1749, 1750, 1752, 1753, 1754, 1755, 1756, 1757, 1759, 1761, 1763, 1764, 1768, 1770, 
 1771, 1772, 1773, 1774, 1777, 1778, 1781, 1785, 1787, 1788, 1789, 1790, 1791, 1793, 1794, 1795, 1798, 1799, 1802, 1803, 1804, 1805, 1806, 1808, 1809, 1813, 1816, 1820, 1822, 1823, 1833, 1837, 1839, 1840, 1841, 1843, 1847, 1850, 1856, 1857, 1858, 1859, 1860, 1861, 1862, 1863, 1866, 1867, 1870, 1874, 1876, 1877, 1878, 1879, 1880, 1884, 1887, 1891, 1893, 1894, 1895, 1896, 1897, 1901, 1904, 1905, 1907, 1908, 1910, 1911, 1912, 1914, 1918, 1921, 1925, 1927, 1928, 1929, 1933, 1937, 1940, 1944, 1946, 1947, 1948, 1950, 1952, 1955, 1956, 1959, 1963, 1965, 1966, 1967, 1968, 1970, 1971, 1975, 1978, 1982, 1984, 1986, 1987, 1988, 1992, 1996, 1999, 2003, 2005, 2006, 2011, 2013, 2015, 2017, 2018, 2022, 2024, 2025, 2026, 2027, 2030, 2034, 2037, 2041, 2043, 2044, 2045, 2047]

   
    corruptRuns = [1066,
    1080,
    1100,
    1101,
    1286,
    1378,
    1379,
    1477,
    1478,
    1479,
    1482,
    1485,
    1488,
    1491,
    1494,
    1511,
    1518,
    1526,
    1533,
    1540,
    1547,
    1550,
    1551,
    1552,
    1553,
    1557,
    1560,
    1563,
    1566,
    1569,
    1572,
    1579,
    1586,
    1593,
    1600,
    1602,
    1604,
    1625,
    1630,
    1695,
    1753,
    1984,
    1985]

    # runs = [1326]
    runs = []
    for i in range(1000,2118):
        runs.append(i)

    refDir = '/data/refData/'
    files = [refDir+'runTiming1400_1529v16.txt', refDir+'runTiming1529_2117v16.txt', refDir+'runTiming1379_1399v16.txt', refDir+'runTiming1000_1400v16.txt', refDir+'runTimingv14.txt']
    timeDicts = createRunTimeDicts(files)
    beamOnTimeDict = timeDicts[0]
    beamOffTimeDict = timeDicts[1]

    # find max timing from validation plots
    maxValidationRunTime = max(list(u.validationRunTimes.values()))

    totalOffTime = 0
    totalOnTime = 0

    overallAccuracy = 0
    numAccMeasures = 0

    for run in runs:
        if int(run) in corruptRuns: continue

        thisRunOffTime = -1
        thisRunOnTime = -1

        # see if run is in file at all
        try:
            thisRunOffTime = beamOffTimeDict[run]
            thisRunOnTime = beamOnTimeDict[run] 

            # case 1: run timing is negative
            if thisRunOffTime < 0 or thisRunOnTime < 0:
                try:
                    print 'Run timing for run', run, 'is negative, trying validation plots'
                    thisRunOffTime = u.validationRunTimes[str(run)]
                    if float(u.validationRunLumis[str(run)]) > 0:
                        print 'Error: Attempted to use validation plots for run', run, 'which has beam on data'
                        continue
                    totalOffTime += thisRunOffTime
                    continue
                except:
                    print 'Error could not find timing for run', run, 'in validation plots'
                    continue
            
            # case 2: run timing is > max timing in validation plots
            if (thisRunOffTime + thisRunOnTime) > maxValidationRunTime:
                try:
                    print 'Run timing for run', run, 'is too large, trying validation plots'
                    thisRunOffTime = u.validationRunTimes[str(run)]
                    if float(u.validationRunLumis[str(run)]) > 0:
                        print 'Error: Attempted to use validation plots for run', run, 'which has beam on data'
                        continue
                    totalOffTime += thisRunOffTime
                    continue
                except:
                    print 'Error could not find timing for run', run, 'in validation plots'
                    continue

            totalOffTime += thisRunOffTime 
            totalOnTime += thisRunOnTime
            # keep track of accuracy with respect to validation plots
            try:
                overallAccuracy += ((thisRunOffTime+thisRunOnTime) - float(u.validationRunTimes[str(run)]))/float(u.validationRunTimes[str(run)])
                numAccMeasures += 1
            except: 
                'Error in calculating accuracy for run', run 
               
        # if run not in file, use validation plots (only for beam off runs)
        except:
            try:
                print 'Could not find run timing for run', run, 'in file, trying validation plots'
                thisRunOffTime = u.validationRunTimes[str(run)]
                if float(u.validationRunLumis[str(run)]) > 0:
                    print 'Error: Attempted to use validation plots for run', run, 'which has beam on data'
                    continue
                totalOffTime += thisRunOffTime
                continue 
            except:
                print 'Error could not find timing for run', run, 'in validation plots'
                continue

    print 'Overall accuracy (compared to validation)', 100 - (overallAccuracy/numAccMeasures)*100, '%'
    print '(in s) On:', totalOnTime, 'Off:', totalOffTime
    print '(in hr) On:', totalOnTime/3600, 'Off:', totalOffTime/3600



    #  calc total lumi
    totalLumi = 0
    for lumi in u.validationRunLumis.values():
        totalLumi += float(lumi)

    print 'total lumi (pb^-1):', totalLumi
    print 'total lumi (fb^-1):', totalLumi/ pow(10,3)