import os,re

file = open('ellipFiles/ellipAllTest.txt')
num_lines = sum(1 for line in file)
file.seek(0)

# file1 = open('ellipFiles/ellip1.txt', 'w')
# file2 = open('ellipFiles/ellip2.txt', 'w')
# file3 = open('ellipFiles/ellip3.txt', 'w')
# file4 = open('ellipFiles/ellip4.txt', 'w')
#print num_lines, num_lines/12





fileNum = 1
for iline, line in enumerate(file):
    for i in range(1,12):
        if iline in range(i* num_lines/12, i* num_lines/12 + 1)

    
exit()
    

for i, file_lines in enumerate(lines):
    outFile = open('ellipFiles/ellipCut'+ str(i+1) +'.txt', 'w')
    for line in file_lines:
        outFile.write(line)
    outFile.close()


