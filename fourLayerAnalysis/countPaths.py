from itertools import product 

allBars = [0,1,24,25,8,9,6,7,16,17,12,13,2,3,22,23,4,5]
slabs=[18,22]
layersMap = {1:[0,1,24,25,8,9],2:[6,7,16,17, 12,13],3:[2,3,22,23,4,5], 4:[20,28]}

allPossiblePaths = list(product(layersMap[1], layersMap[2], layersMap[3], layersMap[4]))
nonRepeatingPaths =  [path for path in allPossiblePaths if len(set(path)) > 2]

triggerablePaths = []
for path in nonRepeatingPaths:
    digiPath = [int(elt/16) for elt in path]
    nFirstDigi = len([elt for elt in digiPath if elt == 0])
    nSecondDigi = len([elt for elt in digiPath if elt == 1])
    if nFirstDigi == nSecondDigi: continue
    triggerablePaths.append(path)

for j, path in enumerate(triggerablePaths):
    print("{", end ="")
    for i, elt in enumerate(path): 
        if i != len(path)-1:
            print(elt, ",", end ="")
        else: 
            print(elt, end ="")
    if j != len(triggerablePaths)-1:
        print("},", end ="")
    else: print("}", end ="")


print(len(triggerablePaths))