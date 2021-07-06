/* usage:
	root -l cosmics.cpp
*/


#include "treesV12Template.h"

void cosmics(){

	vector<TString> directories = {
		"/data/users/MilliQan/trees_v12/Run817_tripleChannelMay01BarsPanelsSlabs10mV",
		"/data/users/MilliQan/trees_v12/Run816_doubleChannelCosmicApr24HV1450TD6", 	
		"/data/users/MilliQan/trees_v12/Run815_doubleChannelCosmicApr24HV1400TD6",
		"/data/users/MilliQan/trees_v12/Run1000_tripleChannelJun25BarsPanelsSlabs10mV",
		//"/data/users/MilliQan/trees_v12/Run1003_tripleChannelJun25BarsPanelsSlabs10mV",
		"/data/users/MilliQan/trees_v12/Run997_tripleChannelJun25BarsPanelsSlabs10mV",
		"/data/users/MilliQan/trees_v12/Run998_tripleChannelJun25BarsPanelsSlabs10mV",
		"/data/users/MilliQan/trees_v12/Run843_tripleChannelMay01BarsPanelsSlabs10mV",
		"/data/users/MilliQan/trees_v12/Run834_tripleChannelMay01BarsPanelsSlabs10mV",
		"/data/users/MilliQan/trees_v12/Run833_tripleChannelMay01BarsPanelsSlabs10mV"
	};


	for(int iDirectory = 0; iDirectory < directories.size(); ++iDirectory){

		//testing
		if(iDirectory > 0) break;


		TChain * chain = new TChain("t");
		chain->Add(directories[iDirectory] + "/*.root");

		SetAddresses(chain);

		int cosmics = 0;
		int cosmics0 = 0;
		int cosmicsMap[3][2] = {
		  { 0, 0 },
		  { 0, 0 },
		  { 0, 0 }
		};

		// keep track of total live time of the run
	    double maxTime = -1;
	    double minTime = 1.e100;

	    TH1D * h1 = new TH1D("areas", "areas", 100, 0, 30000);
	    TH1D * h2 = new TH1D("rates", "Rates Above Threshold", 100, 0, 30000);

		for (int i=0; i<chain->GetEntries(); ++i)
		{

			chain->GetEntry(i);

			if(i%100000 == 0) cout << "Working on event " << i << endl;

			//testing
			//if(i > 1000) break;

			vector<bool> channels(32, false);
			vector<double> areas(32, -1);
			//setCosmicCuts();
			int cosmicCut = 2000;

			//check if beam is as wanted
			//if(beam_on == !beam) continue;

			// keep track of total run live time
		    if(event_time_fromTDC > maxTime) maxTime = event_time_fromTDC;
		    if(event_time_fromTDC < minTime) minTime = event_time_fromTDC;

			for(int j=0; j<(*v_ipulse).size(); ++j){

				//if((*v_ipulse)[j] == 0 && (*v_area)[j] >= cosmicCuts[(*v_chan)[j]]) channels[(*v_chan)[j]] = true;
				if((*v_ipulse)[j] == 0 && (*v_area)[j] >= 0){
					channels[(*v_chan)[j]] = true;
					areas[(*v_chan)[j]] = (*v_area)[j];
				}

			} // end ipulse loop

			int hitsMap[3][2] = {
			  { 0, 0 },
			  { 0, 0 },
			  { 0, 0 }
			};

			/*
			for(int LAYER = 0; LAYER < 3; LAYER++) {
				for(int COLUMN = 0; COLUMN < 2; COLUMN++) {

					int hits = 0;
					int hits0 = 0;

					for(int ROW = 0; ROW < 3; ROW++) {
						if(COLUMN == 0) {
							if(channels[channelMap[LAYER][COLUMN][ROW]] && !channels[channelMap[LAYER][1][ROW]]){
								hits++;
								if(LAYER == 0 && COLUMN == 0 && !channels[channelMap[0][1][ROW]]) hits0++;
						}
					}

					if(hits >= 3 && channels[sheetMap[LAYER][1]]) cosmics++;
					if(hits0 >= 3 && channels[sheetMap[LAYER][1]]) cosmics0++;

				}

			}
			*/

			for(int LAYER = 0; LAYER < 3; LAYER++) {
				for(int COLUMN = 0; COLUMN < 2; COLUMN++) {

					int hits = 0;
			
					for(int ROW = 0; ROW < 3; ROW++) {
						if(COLUMN == 0) {
							if(channels[channelMap[LAYER][COLUMN][ROW]] && !channels[channelMap[LAYER][1][ROW]]){
								hits++;
								hitsMap[LAYER][COLUMN]++;
							}
						}
						if(COLUMN == 1) {
							if(channels[channelMap[LAYER][COLUMN][ROW]] && !channels[channelMap[LAYER][0][ROW]]){
								hits++;
								hitsMap[LAYER][COLUMN]++;
							}
						}
					}

					if(hits >= 3 && channels[sheetMap[LAYER][1]]) cosmics++;
					if(hitsMap[LAYER][COLUMN] >= 3 && channels[sheetMap[LAYER][1]]){
						cosmicsMap[LAYER][COLUMN]++;
					if(LAYER == 0 && COLUMN == 1) h1->Fill(areas[channelMap[LAYER][COLUMN][0]]);
					}
				}


			}


		} // end event loop
		
		cout << "Directory: " << directories[iDirectory] << endl;
		cout << "Cosmics: " << cosmics << endl;
		cout << "Cosmics rate: " << cosmics / (maxTime - minTime) << endl;
		cout << "Cosmics rate per hour: " << (cosmics / (maxTime - minTime)) * 3600 << endl;
		//cout << "Cosmics0 rate: " << cosmics0 / (maxTime - minTime) << endl;
		//cout << "Cosmics0 rate per hour: " << (cosmics0 / (maxTime - minTime)) * 3600 << endl;

		cout << endl;

		
		for(int i = 0; i < 3; i++) {
			for(int j = 0; j < 2; j++) {
				cout << "Cosmics rate per hour path layer " << i << " column " << j << ": " << (cosmicsMap[i][j] / (maxTime - minTime)) * 3600<< endl;
			}
		}

		cout << endl;

	h1->Draw();

	for(int iBin=0; iBin < h1->GetNbinsX(); ++iBin){
		h2->SetBinContent(iBin+1, (h1->Integral(iBin+1, -1)/(maxTime-minTime))*3600);
	} 

	h2->Draw();
	h2->SetLineColor(kBlack);
	h2->GetXaxis()->SetTitle("Charge Area Threshold [pV*s]");
	h2->GetYaxis()->SetTitle("Rate Above Threshold [Hr^-1]");

	}// end directory loop

cout << "done" << endl;

} // end script