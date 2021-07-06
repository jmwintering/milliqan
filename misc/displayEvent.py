import os,re
import sys 

chansVector = ''
hsChansVector = ''
tpChansVector = ''
nPEsVector = ''
timesVector = ''
layer4hit = 0

run = ''
file = ''
event = ''

# if len(sys.argv) != 4:
#     print 'Usage: python displayEvent.py run file event'
#     exit()
        
# run = sys.argv[1]
# file  = sys.argv[2]
# event = sys.argv[3]

os.system('scp wintering@cms-t3.mps.ohio-state.edu:/home/wintering/misc/displayEventInfo.txt .')

displayFile = open('displayEventInfo.txt')
for line in displayFile:
    if 'Chan' in line.split():
        for item in line.split():
            if item == 'Chan': continue
            chansVector += item
    if 'HSChan' in line.split():
         for item in line.split():
            if item == 'HSChan': continue
            hsChansVector += item
    if 'TPChan' in line.split():
         for item in line.split():
            if item == 'TPChan': continue
            tpChansVector += item
    if 'nPE' in line.split():
         for item in line.split():
            if item == 'nPE': continue
            nPEsVector += item
    if 'time' in line.split():
         for item in line.split():
            if item == 'time': continue
            timesVector += item
    if 'is4layer' in line.split():
         for item in line.split():
            if item == 'is4layer': continue
            layer4hit = int(item)


os.system('root -l \'displayEvent.C({0},{1},{2},{3},{4},{5})\''.format(chansVector, hsChansVector, tpChansVector, nPEsVector, timesVector, layer4hit))
