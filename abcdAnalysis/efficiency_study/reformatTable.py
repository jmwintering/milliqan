import os

file = "combined_eff.csv"

table = []

for line in open(file):
    row = ''

    for i, elt in enumerate(line.split(",")):
        elt = elt.replace(",", " ")

        if i != 0:
            try:
                elt = float(elt)
                elt *= 37
                elt = round(elt, 3)
            except:
                continue
        else: 
            try: 
                elt = float(elt)
                elt = int(elt)
            except:
                continue

        if i != len(line.split(","))-1:
            row += str(elt) + " & "
        else:
            row += str(elt) 
    table.append(row + '\\\\')   

for row in table:
    print(row)    