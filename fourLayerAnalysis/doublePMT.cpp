#include "treesV19Template.h"

void doublePMT(
	bool signal = 0,
	int NLAYERS = 3,
	double timingSel = 15
)
{

	int chan_28 = 0;
	int chan_15_28 = 0;
	int chan_not15_28 = 0;

	// four layer corrections
	std::vector<double> nPE_fourLayer = signal ? nPE_corrs_signal:nPE_corrs_data;
	nPE_fourLayer[15] = 0.020283152813273295;
	nPE_fourLayer[20] = 1.0033444816053512;
	nPE_fourLayer[28] = 1.9233939660383577;
	std::vector<double> nPE_corrs = nPE_fourLayer;

	string mode = signal ? "<< SIM MODE >>":"<< DATA MODE >> ";
	TString inputFile = signal ? "data/cosmics_output_4Layer_Jul11.root" : "data/allPhysicsAndTripleChannel4LayerV19_threeLayerHitPlusSlab_200506.root";

	cout << BOLDYELLOW << mode << RESET << endl;

	TChain * chain = new TChain("t");
	chain->Add(inputFile);
	int entries = SetAddresses(chain);
	std::cout << GREEN << "Added " << entries << " entries from " << inputFile << RESET << std::endl;
	if(entries == 0) exit(EXIT_FAILURE); 

	////////// output tree variables /////////////////////////////////////////////////////////
	int run = -1;
	int file = -1;
	int eventNum = -1;
	bool beam_on;
	bool triggerablePath = false;
	bool passPanelVeto = false;
	bool oneHitPerLayer = false;
	std::vector<int> nBarsActiveInLayer;
	int nSlabsActive = 0;
	double maxDeltaT = 0;
	double minDeltaT = 0;
	std::vector<int> bars;
	std::vector<int> otherBars;
	std::vector<int> firstBars;
	std::vector<double> firstBarNPE;
	std::vector<int> firstBarLayer;
	std::vector<double> firstBarArea;
	std::vector<double> firstBarT;
	std::vector<double> sheetNPEs;
	std::vector<double> sheetChans;
	std::vector<int> barLayer;
	std::vector<double> slabTime;
	std::vector<int> slabsHit;
	//////////////////////////////////////////////////////////////////////////////////////////

	int double_coin(0), triple_coin(0);
	std::vector<TString> selections  = {
		"atLeastOneHitPerLayer",
        "oneHitPerLayer",
        "quietRMS",
        "panelVeto",
        "firstPulseMax",
        "pulseTimeSelection",
        "maxminBarNpe",
        "timingSelection",
        "slabMuonveto",
        "triggerablePath"
	};
    std::vector<int> counts(selections.size(),0);

	TH2D * hNPE = new TH2D("hNPE", "Ch 15 v. 28 Npe; Ch 15 Npe; Ch 28 Npe", 20, 0, 100, 20, 0, 100);

	int layer1(0), layer2(0), layer3(0);

	for(int i = 0; i < entries; i++){

		if(i > entries) break;
		displayProgress(i, entries);
		chain->GetEntry(i);

		if(runNum <= 1860) continue;

		// boards are synched, beam is off (for data)
		if(!signal){
			if((*v_groupTDC_b0)[0] != (*v_groupTDC_b1)[0]) continue;
			if(beam) continue;
		}

		firstBarT.clear();
		firstBarLayer.clear();
		firstBarNPE.clear();
		firstBarArea.clear();
		firstBars.clear();
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

		std::vector<bool> chansAlreadyActive(33, false);
		std::vector<double> firstTriggerTimes;
		bool largeSlabHit = false;
		bool failPulseTimeSelection = false;
		bool failfirstPulseMax = false;
		int sheetsActive = 0;
		std::vector<std::vector<double>> pulseList;

		for(int ich=0; ich<32; ++ich){ 
			std::vector<double> tmp_vec(40, -1);
			pulseList.push_back(tmp_vec); 
		}
		
		bool chan15hit = false;
		bool chan28hit = false;
		double ch15nPE = -1;
		double ch28nPE = -1;

		for(int j = 0; j < (*v_ipulse).size(); j++){
			if((*v_chan)[j] == 15) {
				chan15hit = true;
				if((*v_ipulse)[j]==0) ch15nPE = (*v_nPE)[j] * nPE_corrs[(*v_chan)[j]];
				continue;
			}
			
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
			double correctedPulseTime = correctTime(pulseTime, pulseArea, tubeType, signal);

			// fourth layer adjust
			if(pulseChan == 20 || pulseChan == 28){
				pulseLayer = 4;
				pulseType = kBar;
				// correctedPulseTime = correctTimeFourthLayer(pulseTriggerTime, pulseChan);
				// correctedPulseTime = correctTime(correctedPulseTime, pulseArea, tubeType, signal);
			}
			
			if(nPEcorr < 0.5) continue;
			if(signal) if(nPEcorr < 5) continue;

			if(pulseChan == 28){
				chan28hit = true;
				if((*v_ipulse)[j]==0) ch28nPE = (*v_nPE)[j] * nPE_corrs[(*v_chan)[j]];
			}


			pulseList[pulseChan][pulseNum] = nPEcorr; 

			switch(pulseType){
				case kSheet :
					if(chansAlreadyActive[pulseChan]) break;
					sheetsActive++; 
					sheetNPEs.push_back(nPEcorr);
					sheetChans.push_back(pulseChan);
					break;
				
				case kSlab :
					if(!chansAlreadyActive[pulseChan]){
						slabsHit.push_back(pulseChan);
						slabTime.push_back(pulseTriggerTime);

						if((pulseChan == 18 || pulseChan == 21) && nPEcorr < 300) nSlabsActive++;
					}
					if((pulseChan == 18 || pulseChan == 21) && nPEcorr  > 250) largeSlabHit = true;
					break;

				case kBar :
					// save only the first bar to be hit in each layer
					bool notFirstInLayer = false;
					if(firstBars.size() > 0){
						for(int iBar = 0u; iBar < firstBars.size(); iBar++){
							if(firstBarLayer[iBar] == pulseLayer){
								notFirstInLayer = true;
								if(correctedPulseTime < firstBarT[iBar]){
									firstBars[iBar] = pulseChan;
									firstBarT[iBar] = correctedPulseTime;
									firstTriggerTimes[iBar] = pulseTriggerTime;
									firstBarLayer[iBar] = pulseLayer;
									firstBarArea[iBar] = pulseArea;
									firstBarNPE[iBar] = nPEcorr;
								}
							}
						}
					}
					if(firstBars.size() == 0 || !notFirstInLayer){
						firstBars.push_back(pulseChan);
						firstBarT.push_back(correctedPulseTime);
						firstTriggerTimes.push_back(pulseTriggerTime);
						firstBarLayer.push_back(pulseLayer);
						firstBarArea.push_back(pulseArea);
						firstBarNPE.push_back(nPEcorr);
					}

					if(chansAlreadyActive[pulseChan]) break;

					nBarsActiveInLayer[pulseLayer-1]++;
					bars.push_back(pulseChan);
					barLayer.push_back(pulseLayer);
		
					break;

			} // end pulse type switch
			
			chansAlreadyActive[pulseChan] = true;

		} // end ipulse loop

		if(!chan28hit) continue; // want to study same bar
		
		int numLayersActive = 0;
		int numLayersOneBarHit = 0;
		for(int iLayer = 0; iLayer < 4; iLayer++){
			if(nBarsActiveInLayer[iLayer] >= 1) numLayersActive++;
			if(nBarsActiveInLayer[iLayer] == 1) numLayersOneBarHit++;
		}

		// at least one hit per layer
		if(numLayersActive < NLAYERS) continue;

		// one hit per layer
		oneHitPerLayer = numLayersOneBarHit == NLAYERS;
        // if(!oneHitPerLayer) continue;

		// quiet sideband
		bool quietRMS = true;
		for(int iPulse = 0; iPulse < (*v_sideband_RMS).size(); iPulse++){
			if(iPulse == 15) continue;
			if((*v_sideband_RMS)[iPulse] > 1.3) quietRMS = false;
		}	
		if(!quietRMS) continue;

        // panel veto
        passPanelVeto = sheetsActive<=0;
		if(!passPanelVeto) continue;

        // first pulse is max
		for(int ichan=0; ichan<32; ++ichan){

            std::vector<int> info = getInfo(ichan, true);

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

        // no prepulses / slab activity
		failPulseTimeSelection = msl::minElement(firstTriggerTimes) < 200;
		if(slabTime.size() > 0) failPulseTimeSelection = msl::minElement(slabTime) < 200;
		if(failPulseTimeSelection) continue;
		
        // bar max/min npe cut
        double maxBarNPE = msl::maxElement(firstBarNPE);
		double minBarNPE = msl::minElement(firstBarNPE);
		if(maxBarNPE/minBarNPE > 10) continue;

        // find max and min times
		if(firstBarT.size() == NLAYERS){
			minDeltaT = findTimeExtrema(firstBarT, false, firstBarLayer)[0];
			maxDeltaT = findTimeExtrema(firstBarT, false, firstBarLayer)[1];
		}
		else continue;

        // timing selection	
		if(fabs(maxDeltaT) > timingSel && fabs(fabs(maxDeltaT/timingSel)-1) > 0.01) continue;
		else if(fabs(maxDeltaT/timingSel - 1) < 0.01) continue;
		

        // slab muon veto
        if(largeSlabHit) continue;

		
		// triggerable path == 3/2 hits in same digi
		// chans 12 and 13 don't trigger
		int digi0 = 0;
		int digi1 = 0;
		for(int iBar = 0; iBar < bars.size(); iBar++){
			if(iBar == 12  || iBar == 13) continue;
			if(bars[iBar] >= 16) digi1++;
			if(bars[iBar] <= 15) digi0++;
		}
		if(digi0 >= 3 || digi1 >= 3) triggerablePath = true;
		else triggerablePath = false;
		if(!triggerablePath) continue;

		if(minBarNPE < 3) continue;
	
		// fill output variables
		for(auto b: bars) if(!msl::is_in(firstBars, b)) otherBars.push_back(b);

		if(chan28hit) chan_28++;
		if(chan28hit && chan15hit){
			cout << ch15nPE <<   " pepepopo " << ch28nPE << endl;
			hNPE->Fill(ch15nPE, ch28nPE);
			
			chan_15_28++;

			bool layer2hit(0);
			for(int iBar = 0; iBar < firstBars.size(); iBar++){
				if(getInfo(firstBars[iBar])[1] == 1) layer1++;
				if(getInfo(firstBars[iBar])[1] == 2) layer2++;
				if(getInfo(firstBars[iBar])[1] == 3) layer3++;
			}
			
		}
		if(chan28hit && !chan15hit) chan_not15_28++;
		

		ClearVectors();
	} // end event loop

	plot2DHistograms({hNPE}, "data/data_4layer/reee.png");

	cout << "\nNumber of hits in\n";
	cout << BOLDYELLOW << "28\t\t" << chan_28 << endl;
	cout << "28 and 15\t" <<	chan_15_28	<<	endl;
	cout << "28 and not 15\t" << chan_not15_28 << RESET << endl;

	cout << "Where are the hits doe?" << endl;
	cout << "dey be in layer 1 doe “æ…™£´£¢: " << layer1 << endl;
	cout << "dam homie in layer 2 bruv ranch dressing: " << layer2 << endl;
	cout << "shiet here they at in layer version 3.001 ccok" << BOLDYELLOW << layer3 << RESET<<  endl;
	

} // end doublePMT