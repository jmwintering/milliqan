import os
import numpy as np 

chargeMassCombinations = [["0p05","0p007"], ["1p0","0p02"], ["3p0","0p1"]]

cutFiles = ["slab_study_beamon_data.txt", "slab_study_beamoff_data.txt"]
for cm in chargeMassCombinations:
    cutFiles.append("slab_study_sim_m{0}q{1}.txt".format(cm[0], cm[1]))

nRows = 16
cutArray = np.zeros((nRows, len(cutFiles)))

# grab all the cut values and store in cutArray
for row in range(nRows):
    for col, file in enumerate(cutFiles):
        inputArray = np.loadtxt(file, delimiter=",", usecols=1)
        inputArray = np.array([x for x in inputArray])
        cutArray[row][col] = inputArray[row]

# now make latex table
identifierColumn = [
    "Common",
    "Selections",
    "", "", "", "", "", "", "", "", "",
    "Region 1", "Region 2", "Region 3", "Region 4", "Region 5"
]

selectionNames = [
    r"$\geq 1$ hit per layer",
    r"Exactly 1 hit per layer",
    r"Panel veto",
    r"First pulse is max",
    r"Veto early pulses",
    r"Max $\rm{N}_{\rm{PE}}$ / Min $\rm{N}_{\rm{PE}} < 10$",
    r"$\Delta t_{\rm{max}} \leq 15$",
    r"Slab muon veto",
    r"Straight path",
    r"$\rm{N}_{\rm{slab}} = 0$",
    r"$\rm{N}_{\rm{slab}} \geq 1$",
    r"$\rm{N}_{\rm{slab}} = 0$ \& Min $\rm{N}_{\rm{PE}} \in [2,20]$",
    r"$\rm{N}_{\rm{slab}} = 0$ \& Min $\rm{N}_{\rm{PE}} \geq 20$",
    r"$\rm{N}_{\rm{slab}} = 1$ \& Min $\rm{N}_{\rm{PE}} \in [5,30]$",
    r"$\rm{N}_{\rm{slab}} = 1$ \& Min $\rm{N}_{\rm{PE}} \geq 30$",
    r"$\rm{N}_{\rm{slab}} \geq 2$"
]

print('\n\n')
for row in range(nRows):
    rowTex = ''

    rowTex += identifierColumn[row] + r' & '
    rowTex += selectionNames[row] + " & "
    
    for i, cut in enumerate(cutArray[row]):
        if i not in [0,1]:
            cut = round(cut,1)
        else:
            cut = int(cut)


        if i != len(cutArray[row])-1:
            rowTex += str(cut) + " & "
        else:
            rowTex += str(cut)
    rowTex += r'\\'

    if "Region 1" in identifierColumn[row]:
        print(r"\hline")    
        
    print(rowTex)
print('\n\n')




