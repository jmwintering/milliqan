import os,re
import random

file = open('test.csv', 'w')

# FIXME: Change to desired data format: [chans] [nPE] [time_module_calibrated] [npulses]

for j in range(1,100):
    for i in range(1,10):
        frand = random.uniform(0, 10)
        frandInt = random.randint(0,1)
        if i != 9:
            file.write(str(frand)+',')
        else:
            file.write(str(frandInt))
    file.write('\n')


