#!/usr/bin/env python

import os,re
import sys

if __name__ == "__main__":
    if len(sys.argv) < 6:
        print "Error: too few arguments..."
        print "args needed:  runDirectory   run   file_config   numFiles   subfolder"
        exit()
    print sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5]
    runDirectory = sys.argv[1]
    run = sys.argv[2]
    file_config = sys.argv[3]
    numFiles = sys.argv[4]
    subfolder = sys.argv[5]

    if not os.path.exists('/store/user/llavezzo/trees_v17/'+runDirectory+'/'+subfolder+'/'):
        print 'Error: could not find directory'
        exit()
    if not os.path.exists('/store/user/wintering/hsTrees_v17/HS_Run/'+run+'/'+subfolder+'/'):
        os.system('mkdir /store/user/wintering/hsTrees_v17/HS_Run'+run+'/'+subfolder+'/')
    for file in os.listdir('/store/user/llavezzo/trees_v17/'+runDirectory+'/'+subfolder+'/'):
        os.system('root -l -b -q \'make_HS_treeV2.C("/store/user/llavezzo/trees_v17/'+runDirectory+'/'+subfolder+'/'+file+'")\'')
        os.system(' mv {0} /store/user/wintering/hsTrees_v17/HS_Run{1}/{2}/{0}'.format(file, run, subfolder))

    
    