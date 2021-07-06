#!/bin/bash

root -l -b -q make_HS_treeV2.C++\(\"\/store/user/llavezzo/$1/$5/UX5MilliQan_Run$2.$3_$4_v16.root\"\)
mkdir /store/user/wintering/hsTrees_v17/HS_Run$2/$5 
mv UX5MilliQan_Run$2.$3_$4_v16.root /store/user/wintering/hsTrees_v17/HS_Run$2/$5/UX5MilliQan_Run$2.$3_$4_v16.root
