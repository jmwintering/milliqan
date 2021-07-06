import pickle
import os,re
import sys
import time
from decimal import Decimal
import glob
import subprocess

def submitOneJob(run):
    runDirectory = ''
    directory = os.listdir('/store/user/wintering/')
    for fname in directory:
        if 'HS_Run' not in fname:
            continue
        if run in fname:
            runDirectory = '/store/user/wintering/' + fname + '/*.root'
    if runDirectory == '':
        print 'Error: could not find run'
        exit()
    config = runDirectory[runDirectory.find('_')+1:len(runDirectory)]

    print runDirectory

    f = open('hsEfficiency.sub', 'w')
    submitLines = """

    Universe = vanilla
    +IsLocalJob = true
    Rank = TARGET.IsLocalSlot
    request_disk = 2MB
    request_memory = 2048MB
    request_cpus = 1
    executable              = hsEfficiency
    arguments               = {1}
    log                     = effLogs/Run{1}_logs/log{1}_$(PROCESS).log
    output                  = effLogs/Run{1}_logs/out{1}_$(PROCESS).txt
    error                   = effLogs/Run{1}_logs/error{1}_$(PROCESS).txt
    should_transfer_files   = Yes
    when_to_transfer_output = ON_EXIT
    transfer_input_files =  /home/wintering/treesV16Template.h, /home/wintering/utilities.h
    getenv = true
    queue

    """.format(runDirectory, run)
    #input files to transfer

    f.write(submitLines)
    f.close()

    os.system('condor_submit hsEfficiency.sub')

if __name__=="__main__":
    for name in os.listdir('/store/user/wintering/'):
        if 'HS' not in name:
            continue
        run = name[name.find('n')+1:name.find('n')+5]
        if 'Run' not in name:
            continue
        if Decimal(run) < 1630:
            continue
        print run

        os.system('mkdir /data/users/wintering/OSUAnalysis/wintering/hsScripts/effLogs/Run{0}_logs'.format(run))
        submitOneJob(run)
