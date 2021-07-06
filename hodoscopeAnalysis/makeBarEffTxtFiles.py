import os, re
from decimal import Decimal

for file in os.listdir('/data/users/llavezzo/methodFiles/barEfficiency/'):
   run = file[file.find('ff')+2:file.find('.')]
#    if Decimal(run) != 1090:
#        continue
   os.system("./barEff_wrap.sh {0} {1}".format(file,run))   