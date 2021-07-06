#include "treesV19Template.h"
#include "corrections.h"
#define line  __LINE__

TRandom3 * smear  = new TRandom3();

int tubeSpecies(int const ch){
	//tubeSpecies = ["878","878","878","878",             # 0 1 2 3 
	 //    "878","7725","878","878",       # 4 5 6 7   
	 //    "878","ET","878","878", # 8 9 10 11
	 //    "878","878","878","",    # 12 13 14 15
	 //    "878","ET","878","878", # 16 17 18 19 
	 //    "878","878","7725","878",   # 20 21 22 23
	 //    "ET","ET","878","878",  # 24 25 26 27
	 //    "878","878","878","878"]    # 28 29 30 31

	vector<string> tubeTypes = {
		"878","878","878","878",            
	    "878","7725","878","878",       
	    "878","ET","878","878", 
	    "878","878","878","",   
	    "878","ET","878","878",
	    "878","878","7725","878",   
	    "ET","ET","878","878",  
	    "878","878","878","878"
	};

	string tube = tubeTypes[ch];

	if(tube == "878"){ return 0; }
	else if(tube == "ET"){ return 1; }
	else if(tube == "7725"){ return 2; }
	else{ return -1; }
}


vector<int> checkType(int const ch){
	
	int type = -1;
	int layer = -1;
	int column = -1;
	int row = -1;

	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 2; j++){
			for(int k = 0; k < 3; k++){
				if(ch == channelMap[i][j][k]){
					type = 0;
					layer = i;
					column = j;
					row = k;
				}
			}
		}
	}

	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			if(ch == sheetMap[i][j]){
				type = 2;
				layer = i;
			}
		}
	}

	for(int i = 0; i < 4; i++){
		if(ch == slabs[i]){
			type = 1;
			layer = i;
		}
	}

	vector<int> info(4,0);
	info[0] = type;
	info[1] = layer;
	info[2] = column;
	info[3] = row;

	return info;
}


vector<double> findTimeExtrema(vector<double> const time, bool slabs, vector<int> layer={}){

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


double correctTime(double const oldTime, double const area, int const tubeSpecies, bool signal){
	if(tubeSpecies < 0){ return oldTime; }

	vector<float> t_corrections = data_t_corrs[tubeSpecies];
	if(signal) vector<float> t_corrections = mc_t_corrs[tubeSpecies];
	
	double time = oldTime;

	for(int iBin = 0; iBin < area_bins.size(); iBin++){

		if(iBin == area_bins.size()-1){
			if(area >= area_bins[iBin]){
				time += t_corrections[iBin];
				if(signal) time += smear->Gaus(0, smear_corrs[tubeSpecies][iBin]);
			}
		}
		else{
			if(area >= area_bins[iBin] && area < area_bins[iBin+1]){
				time += t_corrections[iBin];
				if(signal) time += smear->Gaus(0, smear_corrs[tubeSpecies][iBin]);
			}
		}

		if(iBin == 0){
			if(area < area_bins[iBin]){
				time += t_corrections[iBin];
				if(signal) time += smear->Gaus(0, smear_corrs[tubeSpecies][iBin]);
			}
		}
	} 

	return time;
}


void cutEfficiency_debug(
	bool singlePulseSel=0,
	TString workDir="methodFiles/", 
	TString inputDir= //file, 
	TString plotsDir="closurePlots/",
	string efficiencyLabel="all_physics_beam.csv",
	bool signal = 0
){ 

	bool signalInjection = signal;
	
	vector<double> nPE_corrs = nPE_corrs_data;
	if(signalInjection){ 
		cout << "SIG INJECT MODE" << endl; 
		nPE_corrs = nPE_corrs_signal;
	}

	bool debugEvent = false;
	auto eventCompEntry = -1;
	int eventComp_run = 1018;
	int eventComp_file = 478;
	int eventComp_event = 428;

	TChain * chain = new TChain("t");
	int entries = 0;
	chain->Add(inputDir+"oneHitPerLayer_allPhysics.root");
	entries = chain->GetEntries();
	SetAddresses_nohs(chain);

	eventCompEntry  = GetTreeIndex(chain, eventComp_run, eventComp_file, eventComp_event, entries);

	cout << "Added " << entries << " entries to abcdMeasure" << endl << endl;

	bool isStraightPath = false;
	bool beam_on;
	double minNPE = 1e9;
	double maxNPE = -1;
	double minSlabNPE = 1e9;
	double maxSlabNPE = -1;
	double minBarNPE = 1e9;
	double maxBarNPE = -1;
	double maxDeltaT = -1;
	double minDeltaT = 1e9;
	double maxSlabDeltaT = -1;
	double minSlabDeltaT = 1e9;
	bool sameDigi  = false;

	bool printedEvent = false;

	int nCuts = 17;
	vector<double> count(nCuts,0);

	int NcompareCuts = 15;
	vector<int> compareCuts(NcompareCuts, 0);

	int nTot = 0; // total of board-synched beam off events
	bool flag = false;

	for(int i = 0; i < entries; i++) {

		displayProgress(i, entries);
		if(debugEvent){
			chain->GetEntry(eventCompEntry);
		}
		else{
			chain->GetEntry(i);
		}

		if(debugEvent){
			if(flag){ break; }
		}
		
		if(runNum == eventComp_run && fileNum == eventComp_file && event == eventComp_event && debugEvent){ 
			flag = true; 
		}

		if(!signalInjection){
			if((*v_groupTDC_b0)[0] != (*v_groupTDC_b1)[0]){ continue;}
			if(beam){ continue; } 
		}

		nTot++;

		// if(flag) cout << printVector((*v_time_module_calibrated)) << endl;

		// quiet sideband/boards synched
		bool quietRMS = true;
		for(int iPulse = 0; iPulse < (*v_sideband_RMS).size(); iPulse++) {
			if(iPulse == 15) continue;
			if((*v_sideband_RMS)[iPulse] > 1.3){ quietRMS = false; }
		}
		
		if(!signalInjection){ scale1fb = 1; }
	
		vector<int> channel = {};
		vector<int> type = {};
		vector<double> time = {};
		vector<double> triggerTimes = {};
		vector<double> slabTime = {};
		vector<int> slabChan = {};
		vector<int> row = {};
		vector<int> col = {};
		vector<int> barRow = {};
		vector<int> barCol = {};
		vector<int> barLayer = {};
		vector<double> barNPEs = {};
		vector<double> nPE = {};
		vector<int> bars = {};
		vector<int> layer = {};
		vector<bool> chansAlreadyActive(33, false);

		// by channel nPE 
		vector<vector<double>> pulseList = {};
		for(int ich=0;ich<32;++ich){ 
			vector<double> tmp_vec(40, -1);
			pulseList.push_back(tmp_vec); 
		}

		// re initialize desired variables
		minNPE = 1e9;
		maxNPE = -1;
		minSlabNPE = 1e9;
		maxSlabNPE = -1;
		maxBarNPE = -1;
		minBarNPE = 1e9;
		minSlabNPE = 1e9;
		isStraightPath = false;
		minDeltaT = 1e9;
		maxDeltaT = -1;
		minSlabDeltaT = 1e9;
		maxSlabDeltaT = -1;
		sameDigi = false;

		bool singlePulse = true;
		bool slabVeto = false;
		bool failPulseTimeSelection = false;
		bool failfirstPulseMax = false;
		int slabsActive = 0;
		int anySlabActive = 0;
		int sheetsActive = 0;
		vector<int> nBarsActiveInLayer(3, 0);
		bool failNPEcut = false;

		debug(true, line);

		for(int j = 0; j < (*v_ipulse).size(); j++){
			if((*v_chan)[j] == 15){ continue; } // timing card
			
			double nPEcorr = (*v_nPE)[j] * nPE_corrs[(*v_chan)[j]];
			double pulseArea = (*v_area)[j];
			double pulseTime = (*v_time_module_calibrated)[j];
			double pulseTriggerTime = (*v_time)[j];
			int pulseChan = (*v_chan)[j];
			int pulseNum = (*v_ipulse)[j];
			int pulseType = (*v_type)[j];
			int pulseLayer = (*v_layer)[j];
			int pulseRow = (*v_row)[j];
			int pulseCol = (*v_column)[j];

			int tubeType = tubeSpecies(pulseChan);
			double correctedPulseTime = correctTime(pulseTime, pulseArea, tubeType, signalInjection);

			if(flag){
				cout << "\nchan: " << (*v_chan)[j] << endl;
				cout << "tube: " << tubeType << endl;
				cout << "area: " << pulseArea << endl;
				cout << "\to: " << pulseTime << endl;
				cout << "\tc: " << correctedPulseTime << endl;
				cout << "\tcorr: " << correctedPulseTime - pulseTime << endl;
			}
			
			if(nPEcorr < 0.5) continue;
			
			// store event info in simpler vectors
			channel.push_back(pulseChan);
			nPE.push_back(nPEcorr);
			type.push_back(pulseType);
			row.push_back(pulseRow);
			col.push_back(pulseCol);
			layer.push_back(pulseLayer);

			pulseList[pulseChan][pulseNum] = nPEcorr; 
			
			if(pulseType == kSheet){
				sheetsActive++;
			}
			else if(pulseType == kSlab){

				anySlabActive++;
				if(!chansAlreadyActive[pulseChan]){
					slabChan.push_back(pulseChan);
					slabTime.push_back(pulseTriggerTime);
					
					if(nPEcorr < minSlabNPE) minSlabNPE = nPEcorr;
					if(nPEcorr > maxSlabNPE) maxSlabNPE = nPEcorr;

					if((pulseChan == 18 || pulseChan == 20 || pulseChan == 21) && nPEcorr < 300) slabsActive++; 
					else if(pulseChan == 28 && nPEcorr  < 200) slabsActive++;
				}
				
				if((pulseChan == 18 || pulseChan == 20 || pulseChan == 21) && nPEcorr  > 250) slabVeto = true; 
				else if(pulseChan == 28 && nPEcorr  > 250) slabVeto = true;
			}
			else if(pulseType == kBar){

				if(!chansAlreadyActive[pulseChan]){
					
					nBarsActiveInLayer[pulseLayer-1]++;
					barRow.push_back(pulseRow);
					barCol.push_back(pulseCol);
					bars.push_back(pulseChan);

					time.push_back(correctedPulseTime);
					triggerTimes.push_back(pulseTriggerTime);

					barNPEs.push_back(nPEcorr);
					barLayer.push_back(pulseLayer);

					if(nPEcorr < minBarNPE) minBarNPE = nPEcorr;
					if(nPEcorr > maxBarNPE) maxBarNPE = nPEcorr;
				}
				
				// if(pulseNum == 0){
				// 	time.push_back(correctedTime);
				// 	barNPEs.push_back(nPEcorr);
				// 	barLayer.push_back(pulseLayer);
				// 	chansAlreadyActive[pulseChan] = true;
				// }
				// else if(pulseNum > 0  && !chansAlreadyActive[pulseChan]){
				// 	time.push_back(correctedTime);
				// 	barNPEs.push_back(nPEcorr);
				// 	barLayer.push_back(pulseLayer);
				// 	chansAlreadyActive[pulseChan] = true;
				// }
				
			}

			if(!chansAlreadyActive[pulseChan]){
				if(nPEcorr < minNPE) minNPE = nPEcorr;
				if(nPEcorr > maxNPE) maxNPE = nPEcorr;
			}
			
			chansAlreadyActive[pulseChan] = true;

		} // end ipulse loop


		// no prepulses
		if(minElement(triggerTimes) < 200) failPulseTimeSelection = true;
		if(slabTime.size() > 0){
			if(minElement(slabTime) < 200){ failPulseTimeSelection = true;}
		}

		debug(true, line);

		// first pulse is max
		for(int ichan=0;ichan<32;++ichan){
			
			int firstNonzeroPulse = 0;

			for(int pulseIndex = 0; pulseIndex < pulseList[ichan].size(); ++pulseIndex){
				if(pulseList[ichan][pulseIndex] > 0){
					firstNonzeroPulse = pulseIndex; 
					break;
				}
			}

			vector<int> info = checkType(ichan);
			if(info[0] == 2) continue;
			if(pulseList[ichan][firstNonzeroPulse] < maxElement(pulseList[ichan])) failfirstPulseMax = true;
		}

		debug(true, line);

		// cout << "huh" << endl;
		// if(flag) cout << printVector(bars) << endl;
		if(flag) cout << "he2" << endl;

		if(bars.size() < 3){ continue; }
		if((nBarsActiveInLayer[0] < 1 || nBarsActiveInLayer[1] < 1 || nBarsActiveInLayer[2] < 1)) continue;
		compareCuts[0] += scale1fb;

		if((nBarsActiveInLayer[0] != 1 || nBarsActiveInLayer[1] != 1 || nBarsActiveInLayer[2] != 1)) continue;
		compareCuts[1] += scale1fb;

		if(!quietRMS){ continue;}
		if(slabVeto){ continue; }
		compareCuts[2] += scale1fb;

		// find max and min times
		maxDeltaT = -1e99;
		minDeltaT = 1e99;
		if(time.size() > 2){
			minDeltaT = findTimeExtrema(time, false, barLayer)[0];
			maxDeltaT = findTimeExtrema(time, false, barLayer)[1];
		}
		else if(time.size() == 1){
			minDeltaT = time[0];
			maxDeltaT = time[0];
		}
		else{
			continue; 
		}
		debug(flag, line);

	
		if(failfirstPulseMax) continue;
		compareCuts[3] += scale1fb;

		debug(flag, line);

		int row_var = abs(barRow[0] - barRow[1]) + abs(barRow[1] - barRow[2]) + abs(barRow[0] - barRow[2]);
		int col_var = abs(barCol[0] - barCol[1]) + abs(barCol[1] - barCol[2]) + abs(barCol[0] - barCol[2]);
	
		if(row_var + col_var == 0) isStraightPath = true;
		else isStraightPath = false;


		// CUTS
		if(failPulseTimeSelection) continue;
		compareCuts[4] += scale1fb;


		// trigger efficiency FIXME: add trigger eff for signal
		compareCuts[5] += scale1fb;


		if(maxBarNPE/minBarNPE > 10){ continue; }
		compareCuts[6] += scale1fb;

		debug(flag, line);

		if(sheetsActive>0) continue;

		if(flag){
			cout << "max: " << maxDeltaT << endl;
		}

		debug(flag, line);
		// if(fabs(maxDeltaT) > 15 || fabs(fabs(maxDeltaT/15)-1) < 0.01){ continue; }
		if(fabs(maxDeltaT) > 15 && fabs(fabs(maxDeltaT/15)-1) > 0.01){ continue; }

		debug(flag, line);

		if(flag) cout << "hm: " << fabs(maxDeltaT/15 - 1)<< endl;
		if(fabs(maxDeltaT/15 - 1) < 0.01){ continue; }
		compareCuts[7] += scale1fb;

		debug(flag, line);

		if(flag){ cout << "streaight: " << isStraightPath << endl;}
		
		if(!isStraightPath){ continue; }
		compareCuts[8] += scale1fb;

		if(flag){ cout << "garbage" << endl;}
		ofstream fPulseMax;
		fPulseMax.open("comp_straight_b.txt", ios_base::app);
		fPulseMax << runNum << " " << fileNum << " " << event << " " << maxDeltaT << endl;
		fPulseMax.close();


		if(slabsActive == 0){

			compareCuts[15] += scale1fb;
			if(minBarNPE > 2 && minBarNPE < 20){ 
				ofstream fPulse;
				fPulse.open("comp_straight_b_region1.txt", ios_base::app);
				fPulse << runNum << " " << fileNum << " " << event << " " << maxDeltaT << endl;
				fPulse.close();

				compareCuts[11] += scale1fb; // region 1
			} 
			if(minBarNPE > 20){ 

				ofstream fPulse;
				fPulse.open("comp_straight_b_region2.txt", ios_base::app);
				fPulse << runNum << " " << fileNum << " " << event << " " << maxDeltaT << endl;
				fPulse.close();

				compareCuts[12] += scale1fb; // region 2
			} 
		}
		else if(slabsActive == 1){
			compareCuts[9] += scale1fb;


			// Cuts[13] += scale1fb;
			if(minBarNPE > 5 && minBarNPE < 30){  // region 3

				ofstream fPulse;
				fPulse.open("comp_straight_b_region3.txt", ios_base::app);
				fPulse << runNum << " " << fileNum << " " << event << " " << maxDeltaT << endl;
				fPulse.close();

				compareCuts[13] += scale1fb; 
			} 

			if(minBarNPE > 30){  // region 4

				ofstream fPulse;
				fPulse.open("comp_straight_b_region4.txt", ios_base::app);
				fPulse << runNum << " " << fileNum << " " << event << " " << maxDeltaT << endl;
				fPulse.close();
				compareCuts[14] += scale1fb; 
			} 
		}
		else if(slabsActive >= 2){
			ofstream fPulse;
			fPulse.open("comp_straight_b_region5.txt", ios_base::app);
			fPulse << runNum << " " << fileNum << " " << event << " " << maxDeltaT << endl;
			fPulse.close();
			compareCuts[10] += scale1fb; // region5
		}

		ClearVectors();
	} // end event loop

	
	vector<string> compareCutNames = {
		"At least one hit per layer",
		"Quiet", 
		"Cosmic",
		"OP",
		"PulseTime",
		"TriggerEff",
		"Max/min",
		"Timing",
		"straight",
		"straight+slab",
		"straight+2slab (region5)",
		"region1",
		"region2",
		"region3",
		"region4",
		"noslabs"
	};

	cout <<"\nnTot " << nTot << endl;
	for(int iCut = 0; iCut < compareCuts.size(); ++iCut){
		cout << compareCutNames[iCut] << " " << compareCuts[iCut] << endl;
	}

	// // Write efficiency numbers
	// ofstream fEff;
	// string effFileLabel = efficiencyLabel;
	// fEff.open(effFileLabel);
	// for(int icut = 0; icut < count.size(); ++icut){
	// 	if(icut != count.size() - 1){ fEff << count[icut] << endl;}
	// 	else{ fEff << count[icut]; }
	// }
	// fEff.close();

	cout << endl;
} // end cutEff