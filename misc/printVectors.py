
file = open("7725corrs.txt", "r")

dataCorr = []
mcCorr = []
smearSigma = []

for line in file:
    # print(line.split())
    if len(line.split()) < 4: continue
    dataCorr.append(line.split()[2])
    mcCorr.append(line.split()[3])
    smearSigma.append(line.split()[4])


for cor in smearSigma:
    print(cor+",", end = '')

print("")

for cor in dataCorr:
    print(cor+",", end = '')

print("")

for cor in mcCorr:
    print(cor+",", end = '')




