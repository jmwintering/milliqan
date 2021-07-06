import pickle
import os,re
import sys
import time
from decimal import Decimal
import glob
import subprocess

def submitOneJob(run, file_config, runPath):
    # grab the run information for condor
    runDirectory = ''
    directory = os.listdir(runPath)
    for fname in directory:
        if 'UX5' in fname:
            continue
        if run in fname:
            runDirectory = fname
    if runDirectory == '':
        print 'Error: could not find run'
        exit()
    config = runDirectory[runDirectory.find('_')+1:len(runDirectory)]
    for subfolder in os.listdir(runPath+runDirectory+'/'):
        if not os.path.exists('/data/users/wintering/NewLogs_v17/Run{0}_logs/{1}'.format(run, subfolder)):
            os.system('mkdir /data/users/wintering/NewLogs_v17/Run{0}_logs/{1}'.format(run, subfolder))

        numFiles = len(os.listdir(runPath+runDirectory+'/'+subfolder))

        if int(subfolder) != 1:
            continue

        #numFiles = 10 #testing
        print runDirectory, run, file_config, numFiles, subfolder
    # submit run (on a subfolder by subfolder basis to condor)

        #numFiles = 3 #testing
        f = open('run'+run+'_'+subfolder+'.sub', 'w')
        submitLines = """

        Universe = vanilla
        +IsLocalJob = true
        Rank = TARGET.IsLocalSlot
        request_disk = 2MB
        request_memory = 2048MB
        request_cpus = 1
        executable              = makeTree_wrap.py
        arguments               = {0} {1} {2} {3} {4}
        log                     = /data/users/wintering/NewLogs_v17/Run{1}_logs/{4}/log{1}.log
        output                  = /data/users/wintering/NewLogs_v17/Run{1}_logs/{4}/out{1}.txt
        error                   = /data/users/wintering/NewLogs_v17/Run{1}_logs/{4}/error{1}.txt
        should_transfer_files   = Yes
        when_to_transfer_output = ON_EXIT
        transfer_input_files = /home/wintering/makeTrees/makeTree_wrap.py, /home/wintering/makeTrees/make_HS_treeV2.C, /home/wintering/treesV16TemplateTest.h, /home/wintering/utilitiesV2.h
        getenv = true
        queue

        """.format(runDirectory, run, file_config, numFiles, subfolder)
        #input files to transfer

        f.write(submitLines)
        f.close()

        os.system('condor_submit run'+run+'_'+subfolder+'.sub')

if __name__=="__main__":
    # 2019 runs start at Run 1631

    runStoragePath = '/store/user/llavezzo/trees_v17/'
 
    fileName = ''
    for name in os.listdir(runStoragePath):
        if 'HS' in name:
            continue
        if 'Run' not in name:
            continue

        run = name[name.find('n')+1:name.find('n')+5]
        if run == '': continue

        if run != '2506': continue 

        if Decimal(run) < 1630:
            print 'Caution: HS data for this run is located in 2018 files. make_HS_treeV2 must be changed'
            continue

        if '1' not in os.listdir(runStoragePath+name): continue
        for file in os.listdir(runStoragePath+name+'/1/'):
            if '.1' in file:
                fileName = file
                file_config = fileName[fileName.find('.1')+3:fileName.find('v17')-1]
        
        print(run, file_config, runStoragePath)

        
        if not os.path.exists('/store/user/wintering/hsTrees_v17/HS_Run'+run):
            os.system('mkdir /store/user/wintering/hsTrees_v17/HS_Run'+run)
        if not os.path.exists('/data/users/wintering/NewLogs_v17/Run{0}_logs'.format(run)):
            os.system('mkdir /data/users/wintering/NewLogs_v17/Run{0}_logs'.format(run))

        

        submitOneJob(run, file_config, runStoragePath)
