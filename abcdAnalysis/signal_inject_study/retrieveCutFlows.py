import os,re 

selections = ["noSlab", "oneSlab"]
 

# if "sheet:" in line.split(): sheetList.append(line.split()[2])
# if "slab:" in line.split(): slabList.append(line.split()[2])

file = open("/data/abcd_data/cutEfficiencies_method.txt")

initialList = []
rmsList = []
singlePulseList = []
nBarsList = []
oneHitList = []

for line in file:
    if "initial" in line.split(): initialList.append(line.split()[2])
    if "RMS" in line.split(): rmsList.append(line.split()[2])
    if "Single" in line.split(): singlePulseList.append(line.split()[2])
    if "nBars" in line.split(): nBarsList.append(line.split()[5])
    if "hit" in line.split(): oneHitList.append(line.split()[4])
   
for i in selections: 
    file1 = open("/data/abcd_data/cutEfficiencies_"+i+".txt")
    file2 = open("/data/abcd_data/cutEfficienciesAnalyze_"+i+".txt")

    slabList = []
    sheetList = []
    straightList = []
    deltaTList = []

    for line in file1:
        if "slab:" in line.split(): slabList.append(line.split()[2])
        if "sheet:" in line.split(): sheetList.append(line.split()[2])
    for line in file2:
        if "straight:" in line.split(): straightList.append(line.split()[1])
        if "signal:" in line.split(): deltaTList.append(line.split()[1])

    print "noSlabData: {", "\n"
    for i in range(0, 10):
        print "{", initialList[i], ",", rmsList[i],",", singlePulseList[i],",", nBarsList[i], ",",oneHitList[i], ",",slabList[i], ",",sheetList[i], ",",straightList[i],",", deltaTList[i], "},", "\n"
    print "}"


        