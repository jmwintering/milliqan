#!/bin/bash

for i in {1030..1630}
do
	echo "$i"
	python submitEffB.py $i
	sleep 2
done
