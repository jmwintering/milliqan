import os

sigDir = #File

chargeMassCombinations = [["3p0","0p1"], ["1p0","0p02"], ["0p05","0p007"]]

for path in os.listdir(sigDir):
	if "mixv4" not in path: continue

	for file in os.listdir(sigDir+path):
		charge = path[path.find("q")+1:path.find("_",11)]
		mass = path[5:path.find("_", 4)]

		if charge == "" or mass == "": continue
		if [mass, charge] not in chargeMassCombinations: continue

		inputFile = sigDir+path+"/*.root"
		effLabel = "m"+mass+"_q"+charge+"_cut_values.txt"

		if os.path.exists(effLabel):
			os.remove(effLabel)
			
		success = os.system("root -l -b -q 'cutEfficiency.cpp(\"{0}\", \"{1}\",1)'".format(inputFile, effLabel))
		if success != 0: exit()