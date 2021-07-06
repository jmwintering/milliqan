#include "treesV19Template.h"
//#include "readFitFunctions.h"
#define line  __LINE__

using namespace std; 


void signalSelection_4layer(
	bool signal = 0,
	int NLAYERS = 3,
	double timingSel = 15
)
{
	TString outputDir = "data/";
	TString outputFile = "outTree";
	TString outfile = outputDir+outputFile;
	outfile += "_" + msl::tstring(to_string(NLAYERS))+"layers";

	TString inputFile= "/data/cosmics_output_4Layer_Aug6.root";

	TString mode = signal ? "<< SIM MODE >>":"<< DATA MODE >>";
	if(signal){
		outputDir += "sim_4layer/";
		outfile += "_sim";
	}
	else{
		outputDir += "data_4layer/";
		inputFile = "/data/skims/allPhysicsAndTripleChannel4LayerV19_threeLayerHitPlusSlab_200506.root";
	}
	outfile += ".root";
	cout << BOLDYELLOW << mode << RESET << endl;
	// cout << BOLDGREEN << "Outputting to " << outfile << RESET << endl;
	
	TChain * chain = new TChain("t");
	chain->Add(inputFile);
	int entries = SetAddresses(chain);
	std::cout << GREEN << "Added " << entries << " entries from " << inputFile << RESET << std::endl;
	if(entries == 0) exit(EXIT_FAILURE); 

	TTree * outputTree = new TTree("outputTree", "outputTree");

		// four layer corrections
	std::vector<double> nPE_fourLayer = signal ? nPE_corrs_signal:nPE_corrs_data;
	nPE_fourLayer[15] = 0.020283152813273295;
	nPE_fourLayer[20] = 1.0033444816053512;	
	nPE_fourLayer[28] = 1.9233939660383577;
	std::vector<double> nPE_corrs = nPE_fourLayer;


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
	
	outputTree->Branch("run", &run);
	outputTree->Branch("file", &file);
	outputTree->Branch("eventNum", &eventNum);
	outputTree->Branch("beam_on", &beam_on);
	outputTree->Branch("triggerablePath", &triggerablePath);
	outputTree->Branch("passPanelVeto", &passPanelVeto);
	outputTree->Branch("nSlabsActive", &nSlabsActive);
	outputTree->Branch("oneHitPerLayer",&oneHitPerLayer);
    outputTree->Branch("nBarsActiveInLayer",&nBarsActiveInLayer);
    outputTree->Branch("maxDeltaT",&maxDeltaT);
    outputTree->Branch("minDeltaT",&minDeltaT);
    outputTree->Branch("firstBars", &firstBars);
	outputTree->Branch("firstBarNPE", &firstBarNPE);
	outputTree->Branch("firstBarArea",&firstBarArea);
	outputTree->Branch("firstBarLayer",&firstBarLayer);
	outputTree->Branch("barLayer",&barLayer);
	outputTree->Branch("firstBarT",&firstBarT);
	outputTree->Branch("sheetNPEs", &sheetNPEs);
	outputTree->Branch("sheetChans",&sheetChans);
	outputTree->Branch("slabTime",&slabTime);
	outputTree->Branch("slabsHit",&slabsHit);
	outputTree->Branch("bars", &bars);
	outputTree->Branch("otherBars", &otherBars);
	//////////////////////////////////////////////////////////////////////////////////////////


	vector<TString> selections  = {
		"atLeastOneHitPerLayer",
        "oneHitPerLayer",
		"triggerablePath",
        "panelVeto",
        "firstPulseMax",
        "pulseTimeSelection",
        "maxminBarNpe",
        "timingSelection",
        "slabMuonveto",
		"NSlabs0",
		"NSlabs1",
        "Region1",
        "Region2",
        "Region3",
        "Region4",
        "Region5"
	};
    vector<double> counts(selections.size(),0);

	for(int i = 0; i < entries; i++){

		displayProgress(i, entries);
		chain->GetEntry(i);

		// boards are synched, beam is off (for data)
		if(!signal){
			if((*v_groupTDC_b0)[0] != (*v_groupTDC_b1)[0]) continue;
			if(beam) continue;
			scale1fb = 1;
		}
		else scale1fb *= 37.5;

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
		bool slabVeto = false;
		bool failPulseTimeSelection = false;
		bool failfirstPulseMax = false;
		int sheetsActive = 0;
		std::vector<std::vector<double>> pulseList;

		for(int ich=0; ich<32; ++ich){ 
			std::vector<double> tmp_vec(40, -1);
			pulseList.push_back(tmp_vec); 
		}
		
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
			double correctedPulseTime = correctTime(pulseTime, pulseArea, tubeType, signal);

			// fourth layer adjust
			if(pulseChan == 20 || pulseChan == 28){
				pulseLayer = 4;
				pulseType = kBar;
				// correctedPulseTime = correctTimeFourthLayer(pulseTriggerTime, pulseChan);
				// correctedPulseTime = correctTime(correctedPulseTime, pulseArea, tubeType, signal);
			}
			
			if(nPEcorr < 0.5) continue;
			// if(signal) if(nPEcorr < 5) continue;

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
					if((pulseChan == 18 || pulseChan == 21) && nPEcorr  > 250) slabVeto = true; 
					break;

				case kBar :

					if(chansAlreadyActive[pulseChan]) break;

					if(pulseLayer == 0) cout << pulseType << " " << pulseChan << endl;
					if(pulseLayer != 0) nBarsActiveInLayer[pulseLayer-1]++;
					bars.push_back(pulseChan);
					barLayer.push_back(pulseLayer);
			
					// save only the first bar to be hit in each layer
					bool notFirstInLayer = false;
					if(firstBars.size() > 0){
						for(int iBar = 0; iBar < firstBars.size(); iBar++){
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
					break;
			} // end pulse type switch
			
			chansAlreadyActive[pulseChan] = true;

		} // end ipulse loop

		
		int numLayersActive = 0;
		int numLayersOneBarHit = 0;
		for(int iLayer = 0; iLayer < 4; iLayer++){
			if(nBarsActiveInLayer[iLayer] >= 1) numLayersActive++;
			if(nBarsActiveInLayer[iLayer] == 1) numLayersOneBarHit++;
		}

		// at least one hit per layer
		if(numLayersActive < NLAYERS) continue;
		counts[0]++;

        // one hit per layer
        oneHitPerLayer = numLayersOneBarHit == NLAYERS;
		if(!oneHitPerLayer) continue;

		// quiet sideband
		bool quietRMS = true;
		for(int iPulse = 0; iPulse < (*v_sideband_RMS).size(); iPulse++){
			if(iPulse == 15) continue; 
			if((*v_sideband_RMS)[iPulse] > 1.3) quietRMS = false;
		}	
		if(!quietRMS) continue;
		counts[1]++;

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
		counts[2]++;


        // panel veto
        passPanelVeto = sheetsActive<=0;
		if(!passPanelVeto) continue;
		counts[3]++;

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
			if(pulseList[ichan][firstNonzeroPulse] < msl::maxElement(pulseList[ichan])) failfirstPulseMax = true;
		}
		if(failfirstPulseMax) continue;
		counts[4]++;

        // no prepulses / slab activity
		failPulseTimeSelection = msl::minElement(firstTriggerTimes) < 200;
		if(slabTime.size() > 0) failPulseTimeSelection = msl::minElement(slabTime) < 200;
		if(failPulseTimeSelection) continue;
		counts[5]++;

        // bar max/min npe cut
        double maxBarNPE = msl::maxElement(firstBarNPE);
		double minBarNPE = msl::minElement(firstBarNPE);

		if(maxBarNPE/minBarNPE > 10) continue;
		counts[6]++;

        // find max and min times
		if(firstBarT.size() == NLAYERS){
			minDeltaT = findTimeExtrema(firstBarT, false, firstBarLayer)[0];
			maxDeltaT = findTimeExtrema(firstBarT, false, firstBarLayer)[1];
		}
		else continue;

		if(fabs(maxDeltaT) > timingSel && fabs(fabs(maxDeltaT/timingSel)-1) > 0.01) continue;
    	if(fabs(maxDeltaT/timingSel - 1) < 0.01) continue;
		counts[7]++;

        // slab muon veto
        if(slabVeto) continue;
		counts[8]++;


		if(nSlabsActive == 0) counts[9]++;
		else if(nSlabsActive >= 1) counts[10]++;

		if(nSlabsActive == 0 && minBarNPE >= 2 && minBarNPE < 20) counts[11]++;
		if(nSlabsActive == 0 && minBarNPE >= 20) counts[12]++;
		if(nSlabsActive == 1 && minBarNPE >= 5 && minBarNPE <= 30) counts[13]++;
		if(nSlabsActive == 1 && minBarNPE >= 30) counts[14]++;
		if(nSlabsActive >= 2) counts[15]++;



		outputTree->Fill();
		ClearVectors();

	} // end event loop

	string tag = "_within100ns";
	string oFileLabel = "cutflow_4layer";
	oFileLabel += signal ? "_sim" : "_data";
	oFileLabel += tag;
	oFileLabel += ".txt";
	ofstream oFile;
	oFile.open(oFileLabel);
	for(int icut = 0; icut < selections.size(); ++icut){
		oFile << selections[icut] << ", " << counts[icut] << endl;
	}
	oFile.close();



	return;
	// save events that passed selection to output tree
	if(outputTree->GetEntries() == 0){
		cerr << BOLDRED << "No output entries, skipping writing..." << RESET << std::endl;
	}
	else{
		std::cout << BOLDGREEN << "Writing to file... (" << outputTree->GetEntries() << " entries)" << RESET << std::endl;
		TFile * outputTreeFile = new TFile(outfile, "recreate");
		outputTreeFile->cd();
		outputTree->Write();
		outputTreeFile->Close();
	} 
	
} // end prepareSignal