import os,re
import math

reRun = True
tag = 'slabs'
runSelection = 'All'

if reRun:
    if os.path.exists('ellipFiles/'+tag+runSelection+'.txt'):
        os.remove('ellipFiles/'+tag+runSelection+'.txt')
    if os.path.exists('ellipFiles/'+tag+runSelection+'_output.txt'):
        os.remove('ellipFiles/'+tag+runSelection+'_output.txt')
    if os.path.exists('ellipFiles/'+tag+runSelection+'noMath.txt'):
        os.remove('ellipFiles/'+tag+runSelection+'noMath.txt')
    
    os.system("root -l -b -q slabEfficiencyMethod.cpp")
    os.system("./checkOverlap.wls ellipFiles/"+tag+runSelection)
    os.system("python slabEfficiencyAnalyze.py")

