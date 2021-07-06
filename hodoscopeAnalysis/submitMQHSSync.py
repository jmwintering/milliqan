import pickle
import os,re
import sys
import time
from decimal import Decimal
import glob
import subprocess

def submitOneJob(run):
    f = open('mqSync.sub', 'w')
    submitLines = """

    Universe = vanilla
    +IsLocalJob = true
    Rank = TARGET.IsLocalSlot
    request_disk = 2MB
    request_memory = 2048MB
    request_cpus = 1
    executable              = mqHSSync
    arguments               = {0}
    log                     = Logs/mqSync/Run{0}_logs/log{0}_$(PROCESS).log
    output                  = Logs/mqSync/Run{0}_logs/out{0}_$(PROCESS).txt
    error                   = Logs/mqSync/Run{0}_logs/error{0}_$(PROCESS).txt
    should_transfer_files   = Yes
    when_to_transfer_output = ON_EXIT
    getenv = true
    queue

    """.format(run)
    #input files to transfer

    f.write(submitLines)
    f.close()

    os.system('condor_submit mqSync.sub')

if __name__=="__main__":
    # 2019 runs start at Run 1631
    fileName = ''
    for name in os.listdir('/store/user/wintering/'):
        if 'HS' in name:
            continue

        run = name[name.find('n')+1:name.find('n')+5]
        if 'Run' not in name:
            continue

        if Decimal(run) != 1369:
            continue
        print(run)

        if Decimal(run) >= 1630:
            print 'Caution: HS data for this run is located in 2019 files. make_HS_treeV2 must be changed'
            continue

        print name

        os.system('mkdir /home/Logs/mqSync/Run{0}_logs'.format(run))
        submitOneJob(run)
