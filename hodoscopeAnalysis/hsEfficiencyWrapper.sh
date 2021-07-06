#!/bin/bash

echo "$1"
root -l -b -q hsEfficiencyMethod.cpp++\(\"$1\"\)
mv hsEff$1.root /data/hsEfficiency2/hsEff$1.root
