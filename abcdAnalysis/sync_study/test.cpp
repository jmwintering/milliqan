#include "treesV19Template.h"

vector<double> findTimeExtrema(vector<double> time, bool slabs, vector<int> layer={}){

	double maxDeltaT = -1;
	double minDeltaT = 1e9;

	vector<double> deltaTimes = {};
	if(!slabs){
		for(int iTime=0; iTime < time.size(); ++iTime){
			for(int jTime=0; jTime < time.size(); ++jTime){
				if(iTime != jTime && (layer[iTime] > layer[jTime])){
					deltaTimes.push_back(time[iTime] - time[jTime]) ;
				}
			}
		}
	}
	else{
		for(int iTime=0; iTime < time.size(); ++iTime){
			for(int jTime=0; jTime < time.size(); ++jTime){
				if(iTime != jTime) deltaTimes.push_back(time[iTime] - time[jTime]) ;
			}
		}
	}

	vector<double> deltaTimeAbs = {};
	for(int j = 0; j < deltaTimes.size(); j++){
		deltaTimeAbs.push_back(fabs(deltaTimes[j]));
	}

	maxDeltaT = maxElement(deltaTimeAbs);
	minDeltaT = minElement(deltaTimeAbs);
	
	int maxIndex = -1;
	int minIndex =-1;
	for(int j = 0; j < deltaTimeAbs.size(); j++){
		if(deltaTimeAbs[j] == maxDeltaT) maxIndex = j;
		if(deltaTimeAbs[j] == minDeltaT) minIndex = j;
	}

	maxDeltaT = deltaTimes[maxIndex];
	minDeltaT = deltaTimes[minIndex];

	return  {minDeltaT, maxDeltaT};
}



void test(){


	vector<double> times = {394.653,392.77,378.236};
	vector<int> layers = {1,3,2};

	cout << printVector(findTimeExtrema(times, 0, layers)) << endl;
}