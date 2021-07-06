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
        if run in fname:
            if 'HS_' not in fname:
                continue
            else:
                runDirectory = fname
    if runDirectory == '':
        print 'Error: could not find run'
        exit()
    config = runDirectory[runDirectory.find('_')+1:len(runDirectory)]
    print runDirectory
    f = open('dtimeV2beam.sub', 'w')
    submitLines = """

    Universe = vanilla
    +IsLocalJob = true
    Rank = TARGET.IsLocalSlot
    request_disk = 2MB
    request_memory = 2048MB
    request_cpus = 1
    executable              = dtime_wrap.sh
    arguments               = {0} {1}
    log                     = /data/users/wintering/dTime/dtime_logs/Run{1}_logs/log{1}.log
    output                  = /data/users/wintering/dTime/dtime_logs/Run{1}_logs/out{1}.txt
    error                   =  /data/users/wintering/dTime/dtime_logs/Run{1}_logs/error{1}.txt
    should_transfer_files   = Yes
    when_to_transfer_output = ON_EXIT
    transfer_input_files = /home/wintering/treesV16Template.h, /home/wintering/hsScripts/dtimeV2beam.cpp, /home/wintering/utilities.h
    getenv = true
    queue

    """.format(runDirectory, run)
    #input files to transfer

    f.write(submitLines)
    f.close()

    os.system('condor_submit dtimeV2beam.sub')

if __name__=="__main__":
    for name in os.listdir('/store/user/wintering/'):
        if 'HS_Run' not in name:
            continue
        run = name[name.find('n')+1:name.find('n')+5]
        if Decimal(run) > 1630:
            continue
        print run

        os.system('mkdir /data/users/wintering/dTime/dtime_logs/Run{0}_logs'.format(run))
        submitOneJob(run)
