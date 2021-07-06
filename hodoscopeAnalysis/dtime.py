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

c = TChain('t')
for x in os.walk('/store/user/wintering/'):
    if 'HS_Run' in x[0]:
        #c.Add(x[0])
        print x[0]

totalEvents = c.GetEntries()

for iE in range(totalEvents):
    c.GetEntry(iE)
    time = gmtime(c.event_time_fromTDC)
