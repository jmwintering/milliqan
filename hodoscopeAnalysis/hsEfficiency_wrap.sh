#!/bin/bash

echo "$1" > /home/bmanley/hsRunName.txt
root -l -b -q '/home/bmanley/hsEfficiencyB.cpp++("/home/bmanley/hsRunName.txt")'
mv hsEff$1.root /data/users/bmanley/hsEff$1.root
