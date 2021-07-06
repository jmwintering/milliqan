import os,re 
import sys 

def getEventFile(run, file,event):
    try:
        run = int(run)
        file = int(file)
        event = int(event)
    except:
        print 'Please enter a valid event'

    runPath = '/store/user/llavezzo/hsTrees_v16/'

    run = str(run)
    file = str(file)
    event = str(event)

    subdir = file[0:1]

    config = ''
    for fname in os.listdir(runPath + "HS_Run" + str(run) +"/1/"):
        if '.1_' not in fname: continue
        config = fname[fname.find('_',17)+1:fname.find('v16',25)-1]
    if config == '': 
        print 'Error cannot find a config'
        exit()


    filePath = runPath + "HS_Run" + str(run) +"/" + str(subdir) + "/UX5MilliQan_Run"+ str(run) + "." + file +"_"+config+"_v16.root"

    return filePath