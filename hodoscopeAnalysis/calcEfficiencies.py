import os,re
import math

reRun = True

tag = 'noCut'
runSelection = 'All'



if reRun:
    if os.path.exists('ellipFiles/'+tag+runSelection+'.txt'):
        os.remove('ellipFiles/'+tag+runSelection+'.txt')
    if os.path.exists('ellipFiles/'+tag+runSelection+'_output.txt'):
        os.remove('ellipFiles/'+tag+runSelection+'_output.txt')
    if os.path.exists('ellipFiles/'+tag+runSelection+'noMath.txt'):
        os.remove('ellipFiles/'+tag+runSelection+'noMath.txt')
    # if os.path.exists('barEffResults_Math.txt'):
    #     os.remove('barEffResults_Math.txt')
    # if os.path.exists('barEffResults_NoMath.txt'):
    #     os.remove('barEffResults_NoMath.txt')
    
    os.system("root -l -b -q barEfficiencyAnalyze.cpp")
    os.system("./checkOverlap.wls ellipFiles/"+tag+runSelection)
    os.system("python barEfficiencyAnalyzeV2.py")

# observed = 0
# notObserved = 0
# file1 = open("barEffResults_NoMath.txt", "r")
# for line in file1:
#     if 'Observed' in line:
#         line = line.replace('Observed:', '')
#         line = line.replace(' ','')
#         observed += float(line)
#     if 'Not observed' in line:
#         line = line.replace('Not observed:', '')
#         line = line.replace(' ', '')
#         notObserved += float(line)

# file2 = open("barEffResults_Math.txt", "r")
# for line in file2:
#     if 'Observed' in line and 'Not' not in line:
#         line = line.replace('Observed:', '')
#         line = line.replace(' ','')
#         observed += float(line)
#     if 'Not Observed' in line:
#         line = line.replace('Not Observed:', '')
#         line = line.replace(' ', '')
#         notObserved += float(line)

# print 'Observed', observed
# print 'Not observed', notObserved
# a = observed 
# b = observed + notObserved
# print ' Eff: ', (observed / (observed + notObserved))*100, '+-', (math.sqrt( (a/ math.pow(b,2)) + (math.pow(a,2)/math.pow(b,3) ) ) )*100  

