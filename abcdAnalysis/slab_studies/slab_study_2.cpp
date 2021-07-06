#include "treesV19Template.h"
#include "readFitFunctions.h"

void slab_study_2(
	bool simulation = 0,
	bool isBeam = 0,
	double timingSel = 30,
	TString masscharge = "m3p0q0p1"
)
{

	readFitFuncs(); 
	
	vector<double> nPE_corrs = simulation ? nPE_corrs_signal:nPE_corrs_data;

	// masscharge = "m0p05q0p007";
	// masscharge = "m1p0q0p02";
	// masscharge = "m3p0q0p1";

	string mode = simulation ? "<< SIM MODE >>":"<< DATA MODE >>";
	TString inputFile = simulation ?  "/data/simulationData/"+masscharge+".root" : "data/allPhysicsAndTripleChannelSinceTS1_threeLayerHit_200303.root";
	
	cout << BOLDYELLOW << mode << RESET << endl;
	if(mode == "<< DATA MODE >>") {
		string beamString = isBeam ? "Beam on":"Beam off";
		cout << BOLDYELLOW << beamString << RESET << endl;
	}

	string oFileLabel = "slab_study_2";
	if(!simulation){
		if(isBeam) oFileLabel = oFileLabel + "_beamon";
		else oFileLabel = oFileLabel + "_beamoff";
	}
	string tag = simulation ? "sim":"data";
	if(simulation) tag += "_"+masscharge;
	oFileLabel += "_"+tag+".txt";

	cout << BOLDRED << oFileLabel << RESET << endl;

	TChain * chain = new TChain("t");
	chain->Add(inputFile);
	int entries = SetAddresses(chain);
	cout << GREEN << "Added " << entries << " entries from " << inputFile << RESET << endl;
	if(entries == 0) exit(EXIT_FAILURE); 

	////////// output tree variables /////////////////////////////////////////////////////////
	int run = -1;
	int file = -1;
	int eventNum = -1;
	bool beam_on;
	bool triggerablePath = false;
	bool passPanelVeto = false;
	bool oneHitPerLayer = false;
	vector<int> nBarsActiveInLayer;
	int nSlabsActive = 0;
	double maxDeltaT = 0;
	double minDeltaT = 0;
	vector<int> bars;
	vector<int> otherBars;
	vector<int> firstBars;
	vector<double> firstBarNPE;
	vector<int> firstBarLayer;
	vector<double> firstBarArea;
	vector<double> firstBarT;
	vector<double> firstBarRow;
	vector<double> firstBarCol;
	vector<double> sheetNPEs;
	vector<double> sheetChans;
	vector<int> barLayer;
	vector<double> slabTime;
	vector<int> slabsHit;
	//////////////////////////////////////////////////////////////////////////////////////////

	vector<TString> selections  = {
		"atLeastOneHitPerLayer",
        "oneHitPerLayer",
        "panelVeto",
        "firstPulseMax",
        "pulseTimeSelection",
        "timingSelection",
        "slabMuonveto",
	};
    vector<double> counts(selections.size(),0);

    TH1D * hPanelNPE = new TH1D("hPanelNPE", "Panel NPE for Events Passing All Selections", 10, 0, 50);
    

	for(int i = 0; i < entries; i++){

		displayProgress(i, entries);
		chain->GetEntry(i);

		// boards are synched, beam is off (for data)
		if(!simulation){
			if((*v_groupTDC_b0)[0] != (*v_groupTDC_b1)[0]) continue;
			if(isBeam && !beam) continue;
			if(!isBeam && beam) continue;
			scale1fb = 1;
		}
		else scale1fb *= 37.5;

		firstBarT.clear();
		firstBarLayer.clear();
		firstBarNPE.clear();
		firstBarArea.clear();
		firstBars.clear();
		firstBarRow.clear();
		firstBarCol.clear();
		barLayer.clear();
		sheetNPEs.clear();
		sheetChans.clear();
		slabTime.clear();
		slabsHit.clear();
		bars.clear();
		otherBars.clear();
		nBarsActiveInLayer.clear();

		// re initialize desired variables
		run = runNum;
		file = fileNum;
		eventNum = event;
		beam_on = beam;
		triggerablePath = false;
		passPanelVeto = false;
		oneHitPerLayer = false;
		nBarsActiveInLayer = {0,0,0,0};
		nSlabsActive = 0;
		maxDeltaT = -1e99;
		minDeltaT = 1e99;

		vector<bool> chansAlreadyActive(33, false);
		vector<double> firstTriggerTimes;
		bool largeSlabHit = false;
		bool failPulseTimeSelection = false;
		bool failfirstPulseMax = false;
		int sheetsActive = 0;
		vector<vector<double>> pulseList;
		for(int ich=0; ich<32; ++ich){ 
			vector<double> tmp_vec(40, -1);
			pulseList.push_back(tmp_vec);
		}
		std::vector<int> nSlabsActiveInLayer = {0,0,0,0};
		std::vector<double> slabT;
		std::vector<int> slabLayer;
		
		for(int j = 0; j < (*v_ipulse).size(); j++){
			if((*v_chan)[j] == 15) continue;
			
			double nPEcorr = (*v_nPE)[j] * nPE_corrs[(*v_chan)[j]];
			double pulseArea = (*v_area)[j];
			double pulseTime = (*v_time_module_calibrated)[j];
			double pulseTriggerTime = (*v_time)[j];
			int pulseChan = (*v_chan)[j];
			int pulseNum = (*v_ipulse)[j];
			int pulseRow = (*v_row)[j];
			int pulseCol = (*v_column)[j];
			int pulseType = (*v_type)[j];
			int pulseLayer = (*v_layer)[j];
			int tubeType = tubeSpecies(pulseChan);
			double correctedPulseTime = correctTime(pulseTime, pulseArea, tubeType, simulation);

			if(pulseType == kBar) continue;
			if(pulseType == kSlab){
				std::vector<int> info = getInfo(pulseChan, false);
				pulseLayer = info[1];
			}

			if(nPEcorr < 0.5) continue;

			pulseList[pulseChan][pulseNum] = nPEcorr; 

			switch(pulseType){

				case kSheet :
					if(!chansAlreadyActive[pulseChan]){
						sheetsActive++; 
						sheetNPEs.push_back(nPEcorr);
						sheetChans.push_back(pulseChan);
					}
					break;
				
				case kSlab :
					if(!chansAlreadyActive[pulseChan]) {
						slabsHit.push_back(pulseChan);
						slabTime.push_back(pulseTriggerTime);
						slabT.push_back(correctedPulseTime);
						slabLayer.push_back(pulseLayer);
						if(nPEcorr < 250) nSlabsActive++;	
						nSlabsActiveInLayer[pulseLayer]+=1;
					}
					if(nPEcorr >= 250) largeSlabHit = true;
					break;

				case kBar :

					if(chansAlreadyActive[pulseChan]) break;

					// if there is another active bar in the layer,
					// save info of the earliest pulse
					if(msl::is_in(firstBarLayer, pulseLayer)){
						for(int iBar = 0; iBar < firstBars.size(); iBar++){
							if(firstBarLayer[iBar] == pulseLayer){
								if(correctedPulseTime < firstBarT[iBar]){
									firstBars[iBar] = pulseChan;
									firstBarT[iBar] = correctedPulseTime;
									firstTriggerTimes[iBar] = pulseTriggerTime;
									firstBarLayer[iBar] = pulseLayer;
									firstBarArea[iBar] = pulseArea;
									firstBarNPE[iBar] = nPEcorr;
									firstBarRow[iBar] = pulseRow;
									firstBarCol[iBar] = pulseCol;
								}
							}
						}
					}
					else{
						firstBars.push_back(pulseChan);
						firstBarT.push_back(correctedPulseTime);
						firstTriggerTimes.push_back(pulseTriggerTime);
						firstBarLayer.push_back(pulseLayer);
						firstBarArea.push_back(pulseArea);
						firstBarNPE.push_back(nPEcorr);
						firstBarRow.push_back(pulseRow);
						firstBarCol.push_back(pulseCol);
					}

					nBarsActiveInLayer[pulseLayer-1]++;
					bars.push_back(pulseChan);
					barLayer.push_back(pulseLayer);
		
					break;

			} // end pulse type switch

			chansAlreadyActive[pulseChan] = true;
			
		} // end ipulse loop

		
		int numLayersActive = 0;
		for(int iLayer = 0; iLayer < 4; iLayer++){
			if(nSlabsActiveInLayer[iLayer] > 0) numLayersActive++;
		}

		// at least one hit per layer
		if(numLayersActive < 4) continue;
		counts[0] += scale1fb;

		// one hit per layer
		//oneHitPerLayer = numLayersOneBarHit == 3;
        //if(!oneHitPerLayer) continue;

		// quiet sideband
		bool quietRMS = true;
		for(int iPulse = 0; iPulse < (*v_sideband_RMS).size(); iPulse++){
			if(iPulse == 15) continue;
			if((*v_sideband_RMS)[iPulse] > 1.3) quietRMS = false;
		}	
		if(!quietRMS) continue;
		
		counts[1] += scale1fb;

		////// signal efficiencies from Francesco Setti @ UCSB //////////////////////// 
		if(simulation){
			double triggerEffPulse(1);

			for(int iBar=0; iBar < firstBarNPE.size(); ++iBar){
				double eff = fitFuncs[firstBars[iBar]]->Eval(firstBarNPE[iBar]);

				triggerEffPulse *= fitFuncs[firstBars[iBar]]->Eval(firstBarNPE[iBar]);
			}
			scale1fb *= triggerEffPulse;
		}
		///// signal efficiencies from Francesco Setti @ UCSB /////////////////////////

        // panel veto
        passPanelVeto = sheetsActive<=0;
		// if(!passPanelVeto) continue;

		counts[2] += scale1fb;

        // first pulse is max
		for(int ichan=0; ichan<32; ++ichan){

            vector<int> info = getInfo(ichan, false);
            if(info[0] == kBar) continue;

            // consider only first bars in each layer
            if(info[0] == kBar){
                if(!msl::is_in(firstBars, ichan)) continue;
            }
            int firstNonzeroPulse = 0;

            for(int pulseIndex = 0u; pulseIndex < pulseList[ichan].size(); ++pulseIndex){
                if(pulseList[ichan][pulseIndex] > 0){
                    firstNonzeroPulse = pulseIndex; 
                    break;
                }
            }

            if(info[0] == 2) continue;
			if(pulseList[ichan][firstNonzeroPulse] < msl::maxElement<double>(pulseList[ichan])) failfirstPulseMax = true;
		}
		if(failfirstPulseMax) continue;
		counts[3] += scale1fb;

        // no prepulses / slab activity
		//failPulseTimeSelection = msl::minElement(firstTriggerTimes) < 200;
		if(slabTime.size() > 0) failPulseTimeSelection = msl::minElement(slabTime) < 200;
		if(failPulseTimeSelection) continue;
		counts[4] += scale1fb;
		
        // find max and min times
		if(slabT.size() > 1){
			minDeltaT = findTimeExtrema(slabT, true, slabLayer)[0];
			maxDeltaT = findTimeExtrema(slabT, true, slabLayer)[1];
		}
		else {
			cout << "error" << endl;
			continue;
		}

        // timing selection	
		if(fabs(maxDeltaT) > timingSel && fabs(fabs(maxDeltaT/timingSel)-1) > 0.01) continue;
		else if(fabs(maxDeltaT/timingSel - 1) < 0.01) continue;
		counts[5] += scale1fb;
		
        // slab muon veto
        if(largeSlabHit) continue;
        counts[6] += scale1fb;

        for(int k=0; k < (*v_ipulse).size(); ++k){
        	if((*v_ipulse)[k] != 0) continue;
        	if((*v_type)[k] != 2) continue;

        	hPanelNPE->Fill((*v_nPE)[k] * nPE_corrs[(*v_chan)[k]]);
        }

        cout << runNum << " " << fileNum << " " << event << endl;

		ClearVectors();
	} // end event loop
	cout << endl;

	// Write cutflow to file
	ofstream oFile;

	oFile.open(oFileLabel);
	for(int icut = 0; icut < selections.size(); ++icut){
		oFile << selections[icut] << ", " << counts[icut] << endl;
		cout << selections[icut] << ", " << counts[icut] << endl;
	}
	oFile.close();


	plot1DHistograms({hPanelNPE}, "panelNPE_allSlabs_beamOff_30ns.png");

	
} // end slab study