#! /bin/bash 

root -l -b -q "abcd4layer.cpp++($1,$2)"
mv fourLayer_outputTree_atLeastOneHitPerLayer_$1_$2.root /store/user/abcd4layer_outputTrees/fourLayer_outputTree_atLeastOneHitPerLayer_$1_$2.root