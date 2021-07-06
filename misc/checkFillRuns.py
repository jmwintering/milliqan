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


def checkOneRun(runDir, numFiles, run, config):
    beamVars = []
    c = r.TChain('t')
    for i in range(numFiles):
        if i == 0:
            continue
        if not os.path.exists("/data/trees_v14/{0}/UX5MilliQan_Run{1}.{2}_{3}_v14.root".format(runDir, run, i, config)):
            continue
        c.Add("/data/trees_v14/{0}/UX5MilliQan_Run{1}.{2}_{3}_v14.root".format(runDir, run, i, config))
    runGood = True

    entries = c.GetEntries()

    print 'Added', entries
    nBeamChange = 0

    startWithBeam = False

    thisBeam = False
    previousBeam = False

    for iE in range(entries):
        c.GetEntry(iE)
        if c.event == 1 and c.file == 1:
            startWithBeam = c.beam
            thisBeam = c.beam
            previousBeam = c.beam
        thisBeam = c.beam
        if thisBeam != previousBeam:
            nBeamChange += 1
            previousBeam = thisBeam

    beamVars.append(nBeamChange)
    beamVars.append(startWithBeam)
    return beamVars


if __name__ == '__main__':
    badRuns = []
    goodRuns = []

    runDirectory = ''
    directory = os.listdir('/data/trees_v14/')
    for fname in directory:
        if 'Run' in fname:
            runDirectory = fname
        if runDirectory == '':
            continue
        run = fname[fname.find('n')+1:fname.find('n')+5]

        fileNums = []
        for file in os.listdir('/data/trees_v14/'+ runDirectory):
            if '.1' in file:
                file_config = file[file.find('.1')+3:file.find('v14')-1]
            fileNum = file[file.find('.')+1:file.find('_', 13)]
            if len(fileNum) > 5 :
                continue
            #print "num: ",fileNum
            fileNums.append(Decimal(fileNum))
        numFiles = max(fileNums) + 1

        vars = []
        vars = checkOneRun(runDirectory,numFiles, run, file_config)
        if vars == []:
            print 'error vars empty'
            continue
        if vars[0]%2 == 0 and vars[1] == 0:
            goodRuns.append(run)
        else:
            badRuns.append(run)
    print goodRuns, badRuns
