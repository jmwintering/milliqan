import os,re
from decimal import Decimal 

filenames1 = ['ellipFiles/effChunk1376_1437_output.txt', 'ellipFiles/effChunk1439_1451_output.txt', 'ellipFiles/effChunk1_output.txt']

with open('ellipFiles/effAll_1.txt', 'w') as outfile:
    for fname in filenames1:
        with open(fname) as infile:
            for line in infile:
                outfile.write(line)
