# import os,re
# import requests
# import urllib.request
# import time
# from bs4 import BeautifulSoup
from urllib import request
import urllib.request
from bs4 import BeautifulSoup
import pandas as pd
# Beautiful soup documentation: https://www.crummy.com/software/BeautifulSoup/bs4/doc/

# bad_runs = []
# badRunFile = open('bad_runs.txt', 'r')
# for line in badRunFile:
#     string = line.split()
#     if string == []:
#         continue
#     if 'fill' in string or 'runs' in string or 'in' in string:
#         continue
#     run = string[0]
#     bad_runs.append(run)

url = "http://cms2.physics.ucsb.edu/milliqanValidationPlots/index.html"
# response = requests.get(url)
#
# #Reponse<200> indiciates succesfull connection
# print(response)
# # if(response != '<Response [200]>'):
# #     print('Error: unsuccesfull connection')
# #     exit()
#
# #print(response.text)
# soup = BeautifulSoup(response.text, "html.parser")
# print(soup.prettify())
# #soup.findAll('A')

page = urllib.request.urlopen(url)
soup = BeautifulSoup(page, "html.parser")
print(soup.title)
#print(soup.findAll('a'))
table = soup.find_all('table')[0] # Grab the first table

n_columns = 0
n_rows=0
column_names = []

# Find number of rows and columns
# we also find the column titles if we can

for row in table.find_all('tr'):

    # Determine the number of rows in the table
    td_tags = row.find_all('td')
    if len(td_tags) > 0:
        n_rows+=1

    # Handle column names if we find them
    th_tags = row.find_all('th')

    if(len(th_tags) != 0):
        print("Number of Columns")
        print(len(th_tags))
        n_columns = len(th_tags)
        for th in th_tags:
            column_names.append(th.get_text())

for name in column_names:
    print(name)
print(n_rows, n_columns)
# for name in column_names:
#     print(name)

# Safeguard on Column Titles
if len(column_names) > 0 and len(column_names) != n_columns:
    raise Exception("Column titles do not match the number of columns")

columns = column_names if len(column_names) > 0 else range(0,n_columns)
df = pd.DataFrame(columns = columns,
                  index= range(0,n_rows))
row_marker = 0
for row in table.find_all('tr'):
    column_marker = 0
    columns = row.find_all('td')
    for column in columns:
        df.iat[row_marker,column_marker] = column.get_text()
        column_marker += 1
    if len(columns) > 0:
        row_marker += 1
