import ROOT as r
import pickle
import os,re
import numpy as np
from uncertainties import ufloat
from collections import defaultdict
from collections import OrderedDict as odict
from array import array
import pandas as pd
from progressbar import ProgressBar
from ROOT import *
import sys
import rootpy
from rootpy.tree import *
import time
from decimal import Decimal

file = open('run_times.txt', 'r')
totalBeamOn = 0
totalBeamOff = 0
runs = []
beamOns = []
beamOffs = []
for line in file:
    if 'Run' in line:
        values = line.split()
        runName = values[1][0+3:values[1].find('_')]
        run = Decimal(runName)
        runs.append(run)
    if 'on' in line:
        values = line.split()
        beamOn = values[2]
        beamOns.append(beamOn)
    if 'off' in line:
        values = line.split()
        beamOff = values[2]
        beamOffs.append(beamOff)

#print totalBeamOn, totalBeamOff
f = r.TFile('runTimes.root', 'RECREATE')
tree = Tree('runTimes', 'runTimes')
tree.create_branches(
    {
    'run':'I',
    'beamOnTime':'D',
    'beamOffTime':'D'
    }
)
for i,j,k in zip(runs, beamOns, beamOffs):
    tree.run = 0
    tree.beamOnTime = 0
    tree.beamOffTime = 0

    tree.run = i
    tree.beamOnTime = j
    tree.beamOffTime = k

    tree.Fill()
tree.Write()
f.Close()
