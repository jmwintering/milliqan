#include "treesV19Template.h"
// #include "corrections.h"
#include "readFitFunctions.h"
// #define line  __LINE__

// TRandom3 * smear  = new TRandom3();

// int tubeSpecies(int const ch){

// 	vector<string> tubeTypes = {
// 		"878","878","878","878",            
// 	    "878","7725","878","878",       
// 	    "878","ET","878","878", 
// 	    "878","878","878","",   
// 	    "878","ET","878","878",
// 	    "878","878","7725","878",   
// 	    "ET","ET","878","878",  
// 	    "878","878","878","878"
// 	};

// 	string tube = tubeTypes[ch];

// 	if(tube == "878"){ return 0; }
// 	else if(tube == "ET"){ return 1; }
// 	else if(tube == "7725"){ return 2; }
// 	else{ return -1; }
// }


// vector<int> checkType(int const ch){
	
// 	int type = -1;
// 	int layer = -1;
// 	int column = -1;
// 	int row = -1;

// 	for(int i = 0; i < 3; i++){
// 		for(int j = 0; j < 2; j++){
// 			for(int k = 0; k < 3; k++){
// 				if(ch == channelMap[i][j][k]){
// 					type = 0;
// 					layer = i;
// 					column = j;
// 					row = k;
// 				}
// 			}
// 		}
// 	}

// 	for(int i = 0; i < 3; i++){
// 		for(int j = 0; j < 3; j++){
// 			if(ch == sheetMap[i][j]){
// 				type = 2;
// 				layer = i;
// 			}
// 		}
// 	}

// 	for(int i = 0; i < 4; i++){
// 		if(ch == slabs[i]){
// 			type = 1;
// 			layer = i;
// 		}
// 	}

// 	vector<int> info(4,0);
// 	info[0] = type;
// 	info[1] = layer;
// 	info[2] = column;
// 	info[3] = row;

// 	return info;
// }


// vector<double> findTimeExtrema(vector<double> const time, bool slabs, vector<int> layer={}){

// 	double maxDeltaT = -1;
// 	double minDeltaT = 1e9;

// 	vector<double> deltaTimes = {};
// 	if(!slabs){
// 		for(int iTime=0; iTime < time.size(); ++iTime){
// 			for(int jTime=0; jTime < time.size(); ++jTime){
// 				if(iTime != jTime && (layer[iTime] > layer[jTime])){
// 					deltaTimes.push_back(time[iTime] - time[jTime]) ;
// 				}
// 			}
// 		}
// 	}
// 	else{
// 		for(int iTime=0; iTime < time.size(); ++iTime){
// 			for(int jTime=0; jTime < time.size(); ++jTime){
// 				if(iTime != jTime) deltaTimes.push_back(time[iTime] - time[jTime]) ;
// 			}
// 		}
// 	}

// 	vector<double> deltaTimeAbs = {};
// 	for(int j = 0; j < deltaTimes.size(); j++){
// 		deltaTimeAbs.push_back(fabs(deltaTimes[j]));
// 	}

// 	maxDeltaT = maxElement(deltaTimeAbs);
// 	minDeltaT = minElement(deltaTimeAbs);
	
// 	int maxIndex = -1;
// 	int minIndex =-1;
// 	for(int j = 0; j < deltaTimeAbs.size(); j++){
// 		if(deltaTimeAbs[j] == maxDeltaT) maxIndex = j;
// 		if(deltaTimeAbs[j] == minDeltaT) minIndex = j;
// 	}

// 	maxDeltaT = deltaTimes[maxIndex];
// 	minDeltaT = deltaTimes[minIndex];

// 	return  {minDeltaT, maxDeltaT};
// }


// double correctTime(double const oldTime, double const area, int const tubeSpecies, bool signal){
// 	if(tubeSpecies < 0){ return oldTime; }

// 	vector<float> t_corrections = data_t_corrs[tubeSpecies];
// 	if(signal) vector<float> t_corrections = mc_t_corrs[tubeSpecies];
	
// 	double time = oldTime;

// 	for(int iBin = 0; iBin < area_bins.size(); iBin++){

// 		if(iBin == area_bins.size()-1){
// 			if(area >= area_bins[iBin]){
// 				time += t_corrections[iBin];
// 				if(signal) time += smear->Gaus(0, smear_corrs[tubeSpecies][iBin]);
// 			}
// 		}
// 		else{
// 			if(area >= area_bins[iBin] && area < area_bins[iBin+1]){
// 				time += t_corrections[iBin];
// 				if(signal) time += smear->Gaus(0, smear_corrs[tubeSpecies][iBin]);
// 			}
// 		}

// 		if(iBin == 0){
// 			if(area < area_bins[iBin]){
// 				time += t_corrections[iBin];
// 				if(signal) time += smear->Gaus(0, smear_corrs[tubeSpecies][iBin]);
// 			}
// 		}
// 	} 

// 	return time;
// }


void cutEfficiency(
	TString inputFile="data/signalInjection/m0p05q0p007.root", 
	string efficiencyLabel="all_physics_beam_signal.txt",
	bool signal = 1
){ 

	vector<double> nPE_corrs = nPE_corrs_data;
	if(signal){
		cout << "SIG INJECT MODE" << endl;
		nPE_corrs = nPE_corrs_signal;
	}

	readFitFuncs();

	TChain * chain = new TChain("t");
	int entries = 0;
	chain->Add(inputFile);
	entries = chain->GetEntries();
	SetAddresses_nohs(chain);
	cout << "Added " << entries << " entries to abcdMeasure" << endl << endl;
	if(entries == 0){ exit(EXIT_FAILURE); }


	// debugging stuff //////////////////////////
	bool debugEvent = false;
	auto eventCompEntry = -1;
	int eventComp_run = 1018;
	int eventComp_file = 478;
	int eventComp_event = 428;
	if(debugEvent) eventCompEntry  = GetTreeIndex(chain, eventComp_run, eventComp_file, eventComp_event, entries);
	bool printedEvent = false;
	bool flag = false;
	/////////////////////////////////////////////

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

	int nCuts = 17;
	vector<double> cuts(nCuts, 0);
	double nTot = 0; // total of board-synched beam off events


	for(int i = 0; i < entries; i++) {

		displayProgress(i, entries);
		if(!debugEvent) chain->GetEntry(i);
		else{
			chain->GetEntry(eventCompEntry);
			if(flag) break;
		}

		if(runNum == eventComp_run && fileNum == eventComp_file && event == eventComp_event && debugEvent) flag = true;

		if(!signal) scale1fb = 1;
		else scale1fb *= 37.5;

		// make sure boards are synched and beam is off
		if(!signal){
			if((*v_groupTDC_b0)[0] != (*v_groupTDC_b1)[0]){ continue; }
			if(beam){ continue; } 
		}

		nTot += scale1fb;

		// quiet sideband
		bool quietRMS = true;
		for(int iPulse = 0; iPulse < (*v_sideband_RMS).size(); iPulse++) {
			if(iPulse == 15) continue;
			if((*v_sideband_RMS)[iPulse] > 1.3){ quietRMS = false; }
		}
		
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

		// cut booleans/counters
		bool slabVeto = false;
		bool failPulseTimeSelection = false;
		bool failfirstPulseMax = false;
		int slabsActive = 0;
		int anySlabActive = 0;
		int sheetsActive = 0;
		vector<int> nBarsActiveInLayer(3, 0);

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
			double correctedPulseTime = correctTime(pulseTime, pulseArea, tubeType, signal);

			if(nPEcorr < 0.5) continue;
			
			// store event info in simpler vectors
			channel.push_back(pulseChan);
			nPE.push_back(nPEcorr);
			type.push_back(pulseType);
			row.push_back(pulseRow);
			col.push_back(pulseCol);
			layer.push_back(pulseLayer);

			pulseList[pulseChan][pulseNum] = nPEcorr; 
			
			if(pulseType == kSheet){ sheetsActive++; }
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


		if(bars.size() < 3){ continue; }
		if((nBarsActiveInLayer[0] < 1 || nBarsActiveInLayer[1] < 1 || nBarsActiveInLayer[2] < 1)) continue;
		cuts[0] += scale1fb;

		if(!quietRMS){ continue;}
		if((nBarsActiveInLayer[0] != 1 || nBarsActiveInLayer[1] != 1 || nBarsActiveInLayer[2] != 1)) continue;
		cuts[1] += scale1fb;		

		// signal efficiencies from Francesco Setti @ UCSB
		if(signal){
			double triggerEffPulse(1);

			for(int iBar=0; iBar < barNPEs.size(); ++iBar){
				double eff = fitFuncs[bars[iBar]]->Eval(barNPEs[iBar]);

				if(barNPEs[iBar]  < 1){
					// cout << bars[iBar] << " " << barNPEs[iBar] << " " << eff << endl;
				}
				triggerEffPulse *= fitFuncs[bars[iBar]]->Eval(barNPEs[iBar]);
			}
        	scale1fb *= triggerEffPulse;
		}

		if(sheetsActive>0) continue;
		cuts[2] += scale1fb; 


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
		else{ continue; }
		
		if(failfirstPulseMax) continue;
		cuts[3] += scale1fb;


		int row_var = abs(barRow[0] - barRow[1]) + abs(barRow[1] - barRow[2]) + abs(barRow[0] - barRow[2]);
		int col_var = abs(barCol[0] - barCol[1]) + abs(barCol[1] - barCol[2]) + abs(barCol[0] - barCol[2]);
	
		if(row_var + col_var == 0) isStraightPath = true;
		else isStraightPath = false;


		// CUTS
		if(failPulseTimeSelection) continue;
		cuts[4] += scale1fb;


		// trigger efficiency FIXME: add trigger eff for signal
		cuts[5] += scale1fb;


		if(maxBarNPE/minBarNPE > 10){ continue; }
		cuts[6] += scale1fb;

		
		if(fabs(maxDeltaT) > 15 && fabs(fabs(maxDeltaT/15)-1) > 0.01){ continue; }
		if(fabs(maxDeltaT/15 - 1) < 0.01){ continue; }
		cuts[7] += scale1fb;

		if(slabVeto){ continue; }
		cuts[8] += scale1fb;
		
		if(!isStraightPath){ continue; }
		cuts[9] += scale1fb;


		if(slabsActive == 0){

			cuts[10] += scale1fb;

			if(minBarNPE > 2 && minBarNPE < 20){ cuts[12] += scale1fb; } // region 1
			if(minBarNPE > 20){ cuts[13] += scale1fb; } // region 2

		}
		else if(slabsActive == 1){

			cuts[11] += scale1fb;

			if(minBarNPE > 5 && minBarNPE < 30){  cuts[14] += scale1fb; } // region 3 
			if(minBarNPE > 30){  cuts[15] += scale1fb; } // region 4
	
		}
		else if(slabsActive >= 2){ 

			cuts[11] += scale1fb; 
			cuts[16] += scale1fb; 
			} // region 5

		ClearVectors();
	} // end event loop

	// Write efficiency numbers out
	ofstream fEff;
	string effFileLabel = efficiencyLabel;
	fEff.open(effFileLabel);

	fEff << nTot << " : ";
	for(int icut = 0; icut < cuts.size(); ++icut){

		if(icut != cuts.size() - 1){ fEff << cuts[icut] << " : "; }
		else{ fEff << cuts[icut]; }

	}
	fEff.close();

	cout << endl;
} // end cutEff