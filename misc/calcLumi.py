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
import rootpy
from rootpy.tree import Tree, TreeModel, FloatCol, IntCol
from decimal import Decimal

def calcLumiOneRun(runDir):
    c = r.TChain('t')
    c.Add('/data/trees_v16/'+ runDir + '/*.root')
    totalRunLumi = 0

    previousFill = -1
    thisFill = -1
    for iE in range(c.GetEntries()):
        c.GetEntry(iE)
        thisFill = c.fill
        if thisFill != previousFill:
            if c.fill > 0:
                totalRunLumi += c.fillTotalLumi
            previousFill = thisFill

    print totalRunLumi

    return totalRunLumi

if __name__ == '__main__':
    badRuns = []

    totalLumi = 0

    for fname in os.listdir('/data/trees_v16/'):
        if 'Run' not in fname:
            continue
        run = fname[fname.find('n')+1:fname.find('n')+5]

        #testing
        if Decimal(run) != 1077:
            continue
        print run

        runLumi = calcLumiOneRun(fname)
        if runLumi > 0:
            totalLumi += runLumi

    print totalLumi
