import os

masscharges = ["m0p05q0p007","m1p0q0p02","m3p0q0p1"]
for sim in [1,0]:
	if sim == 0:
		for beam in [0,1]:
			os.system("root -l -b -q 'slab_study_2.cpp({0},{1},{2},\"{3}\")'".format(sim, beam, 15, "blah"))
	if sim == 1:
		for masscharge in masscharges:
			os.system("root -l -b -q 'slab_study_2.cpp({0},{1},{2},\"{3}\")'".format(sim, 0, 15, masscharge))