#include "treesV16TemplateTest.h"

struct hit {
	int type;
	int column;
	int row;
	int layer;
	double height;
	double area;
	double nPE;
	int iPulse;
	int channel;
	double time;
};


void fitLineEfficiency(){

	TChain * chain = new TChain("t");
	chain->Add("/data/hsEfficiency2/*.root");
	SetAddressesChainHS(chain);

	vector<double> muonBarCuts(32, 400);
    vector<double> cosmicBarCuts(32, 0);
    cosmicBarCuts[0] = 2900;
    cosmicBarCuts[1] = 2700;
    cosmicBarCuts[2] = 2500;
    cosmicBarCuts[3] = 2500;
    cosmicBarCuts[4] = 2800;
    cosmicBarCuts[5] = 900;
    cosmicBarCuts[6] = 3400;
    cosmicBarCuts[7] =  2800;
    cosmicBarCuts[8] = 2500;
    cosmicBarCuts[9] = 1400;
    cosmicBarCuts[12] = 2900;
    cosmicBarCuts[13] = 2000;
    cosmicBarCuts[16] = 5700;
    cosmicBarCuts[17] = 2000;
    cosmicBarCuts[22] = 650;
    cosmicBarCuts[23] = 3000;
    cosmicBarCuts[24] = 2000;
    cosmicBarCuts[25] = 3600;

	vector<double> slabCuts(4,0);
	slabCuts[0] = 300;
	slabCuts[1] = 350;
	slabCuts[2] = 150;
	slabCuts[3] = 350;

	vector<int> layer_1_bars = {0,1,24,25,8,9};
	vector<int> layer_2_bars = {6,7,16,17,12,13};
	vector<int> layer_3_bars = {2,3,22,23,4,5};
	vector<int> slabs = {18, 20, 28, 21};

	bool cutStudy = false;

	int entries = chain->GetEntries();
	cout << "added " << entries << " entries" << endl;

	TH1D::SetDefaultSumw2();
	// Cut investigation
	TH1D * barnPE = new TH1D("barnPE", "barnPE; nPE", 200, 0, 4000);
	TH1D * layer1nPE = new TH1D("layer1nPE", "layer1nPE; nPE", 200, 0, 4000);
	TH1D * layer2nPE = new TH1D("layer2nPE", "layer2nPE; nPE", 200, 0, 4000);
	TH1D * layer3nPE = new TH1D("layer3nPE", "layer3nPE; nPE", 200, 0, 4000);

	int nPass = 0;
	int nTotal = 0;
	TH1D * hPass = new TH1D("hPass", "hPass", 1,0,1);
	TH1D * hTotal = new TH1D("hTotal", "hTotal", 1,0,1);
	 
	for(int iE=0; iE<entries; ++iE){
		chain->GetEntry(iE);
		displayProgress(iE, entries);

		// testing 
		// if(event != 510) continue; 

		//check for straight path through bars
    	bool isStraightPath = false;
		bool isStraightHS = false;
		bool matched = false;
		bool weak_matched = false;
		vector<bool> straightPaths(6,false);
		vector<bool> straightPathsHS(6,false);
		double sheetNPE = 0;
		double slabNPE = 0;
		double otherBarsNPE = 0;
		vector<bool> layersHit = {false, false, false, false};
		vector<bool> slabsHit = {false, false, false, false};
		vector<hit> barHits= {};
		
		//if(!beam) continue;
		vector<double> barCuts(32,0);
        if(beam) barCuts = muonBarCuts;
        else barCuts = cosmicBarCuts; 

		//consider only events with HS data
		bool skipEvent = false;
		for(unsigned int j = 0; j < (*v_hs).size(); j++){
			if((*v_hs)[j] == -1) skipEvent = true;
		}
		if(skipEvent) continue;

		for(unsigned int ipulse=0; ipulse < (*v_ipulse).size(); ++ipulse){
			if((*v_ipulse)[ipulse]!=0) continue;
			if((*v_type)[ipulse] == 1) {
				slabNPE += (*v_nPE)[ipulse];
				for(unsigned int j = 0; j < slabs.size(); j++){
					if(slabs[j] == (*v_chan)[ipulse] && (*v_nPE)[ipulse] > slabCuts[j]) slabsHit[j] = true;
				}
			}
			if((*v_type)[ipulse] == 2) sheetNPE += (*v_nPE)[ipulse];

			if((*v_layer)[ipulse] == 1 && (*v_type)[ipulse]==0) layer1nPE->Fill((*v_nPE)[ipulse]);
			if((*v_layer)[ipulse] == 2 && (*v_type)[ipulse]==0) layer2nPE->Fill((*v_nPE)[ipulse]);
			if((*v_layer)[ipulse] == 3 && (*v_type)[ipulse]==0) layer3nPE->Fill((*v_nPE)[ipulse]);

			if((*v_type)[ipulse]== 0 && (*v_nPE)[ipulse]>barCuts[(*v_chan)[ipulse]]){

				hit thisHit;
				thisHit.type = (*v_type)[ipulse];
				thisHit.column = (*v_column)[ipulse];
				thisHit.row = (*v_row)[ipulse];
				thisHit.layer = (*v_layer)[ipulse];
				thisHit.height = (*v_height)[ipulse];
				thisHit.area = (*v_height)[ipulse];
				thisHit.nPE = (*v_nPE)[ipulse];
				thisHit.iPulse = (*v_ipulse)[ipulse];
				thisHit.channel = (*v_chan)[ipulse];
				thisHit.time = (*v_time_module_calibrated)[ipulse];
				barHits.push_back(thisHit);

				layersHit[(*v_layer)[ipulse]] = true;
			}

			if((*v_type)[ipulse]== 0 && (*v_nPE)[ipulse]< barCuts[(*v_chan)[ipulse]]){
				otherBarsNPE += (*v_nPE)[ipulse];
			}

		} // end ipulse loop


		// check for straight paths in bars
		for(const auto firstHit : barHits){
		  for(const auto secondHit : barHits){
			  for(const auto thirdHit : barHits){
					if(firstHit.layer != secondHit.layer && secondHit.layer != thirdHit.layer && firstHit.layer != thirdHit.layer){
						isStraightPath = firstHit.row - secondHit.row == 0 && secondHit.row - thirdHit.row == 0 && firstHit.column - secondHit.column == 0 && secondHit.column - thirdHit.column ==0;

						for(int path=0;path<6;++path){
							if(firstHit.channel == layer_1_bars[path] && secondHit.channel == layer_2_bars[path] && thirdHit.channel == layer_3_bars[path]) straightPaths[path] = true;
						} // paths
					} // if
			  } // 3rd hit
		  } //2nd hit
	  	} // 1st hit
        
		
        vector<float> hs_phys_coord_xTop = {};
		vector<float> hs_phys_coord_xBot = {};
		vector<float> hs_phys_coord_yTop = {};
		vector<float> hs_phys_coord_yBot = {};
		vector<vector<float>> hsCoord = {};
		for(unsigned int j = 0; j < (*v_hs).size(); j++) {
			vector<int> coord_index = convertRawToPhysCh((*v_hs)[j],true);
			if(coord_index.size() < 3) continue;

			hsCoord.push_back(convertPhysChToCoord(coord_index, true));
			if((*v_hs)[j] >= 48 && (*v_hs)[j] <= 63){
				 
				if(coord_index.at(0) != 0) hs_phys_coord_xTop.push_back(convertPhysChToCoord(coord_index, true)[0]);
			}
			if((*v_hs)[j] >= 0 && (*v_hs)[j] <= 15){
				 
			if(coord_index.at(0) != 0) hs_phys_coord_xBot.push_back(convertPhysChToCoord(coord_index, true)[0]);
			}
			if((*v_hs)[j] >= 32 && (*v_hs)[j] <= 47){
				 
				if(coord_index.at(1) != 0) hs_phys_coord_yTop.push_back(convertPhysChToCoord(coord_index, true)[1]);
			}
			if((*v_hs)[j] >= 16 && (*v_hs)[j] <= 31){
				 
			if(coord_index.at(1) != 0) hs_phys_coord_yBot.push_back(convertPhysChToCoord(coord_index, true)[1]);
			}
		}

		bool withinBarCenter2 = true;
		vector<double> barCoords = {};
		if(barHits.size() != 0){
			barCoords = convertBarChToPhysCoord(barHits[0].channel);
			for(int icoord=0;icoord<hsCoord.size(); ++icoord){
				if(hsCoord[icoord][0] != 0){
					if(abs(hsCoord[icoord][0] - barCoords[0])> 3.5) withinBarCenter2 = false;
				} 
				if(hsCoord[icoord][1] != 0) {
					if(abs(hsCoord[icoord][1] - barCoords[1])> 3.5) withinBarCenter2 = false;
				}
			}
		}

		vector<double> barCoordsL1_x = {};
		vector<double> barCoordsL1_y = {};
		vector<double> barCoordsL3_x = {};
		vector<double> barCoordsL3_y = {};

		//store layer 1 coords and layer 3 coords
		if(barHits.size()!=0){
			for(int j = 0; j < barHits.size(); j++){
				if(getLayer(barHits[j].channel) == 1){
					barCoordsL1_x.push_back(convertBarChToPhysCoord(barHits[j].channel)[0]);
					barCoordsL1_y.push_back(convertBarChToPhysCoord(barHits[j].channel)[1]);
				}
				if(getLayer(barHits[j].channel) == 3){
					barCoordsL3_x.push_back(convertBarChToPhysCoord(barHits[j].channel)[0]);
					barCoordsL3_y.push_back(convertBarChToPhysCoord(barHits[j].channel)[1]);
				}
			}
		}

		bool withinBarCenter = true;
		if(barHits.size() != 0){
			for(int j = 0; j < hs_phys_coord_xBot.size(); j++){
				for(int k = 0; k < barCoordsL1_x.size(); k++){
					if(abs(hs_phys_coord_xBot[j]-barCoordsL1_x[k]) > 3.5) {
						withinBarCenter = false;
					}
				}
			}
			for(int j = 0; j < hs_phys_coord_yBot.size(); j++){
				for(int k = 0; k < barCoordsL1_y.size(); k++){
					if(abs(hs_phys_coord_yBot[j]-barCoordsL1_y[k]) > 3.5) {
						withinBarCenter = false;
					}
				}
			}
			for(int j = 0; j < hs_phys_coord_xTop.size(); j++){
				for(int k = 0; k < barCoordsL3_x.size(); k++){
					if(abs(hs_phys_coord_xTop[j]-barCoordsL3_x[k]) > 3.5) {
						withinBarCenter = false;
					}
				}
			}
			for(int j = 0; j < hs_phys_coord_yTop.size(); j++){
				for(int k = 0; k < barCoordsL3_y.size(); k++){
					if(abs(hs_phys_coord_yTop[j]-barCoordsL3_y[k]) > 3.5) {
						withinBarCenter = false;
					}
				}
			}
		}


		/* CUTS  */

		// baseline:
		if(hs_phys_coord_xBot.size() <1 || hs_phys_coord_xTop.size() <1 || hs_phys_coord_yBot.size() <1 || hs_phys_coord_yTop.size() <1) continue;
		if(otherBarsNPE > 200) continue;
		if(!layersHit.at(3) || !layersHit.at(1) || !layersHit.at(2)) continue;
		if(barHits.size() != 3) continue;
		
		//select muons
		if(hs_phys_coord_xBot.size() !=2 || hs_phys_coord_xTop.size() !=2 || hs_phys_coord_yBot.size() !=2 || hs_phys_coord_yTop.size() !=2) continue;
		if(!withinBarCenter) continue;
		// if(!withinBarCenter2) continue;
		if(sheetNPE > 100) continue;
		if(!isStraightPath) continue;
		if(!slabsHit[0] || !slabsHit[1] || !slabsHit[2] || !slabsHit[3]) continue;
 		

        //fill chanHS with channels hit by HS by the line fit
        vector<double> parFit = {};
		vector<int> chanHS = {};
		vector<double> path_length= {};

        parFit = doCircleFitInverted(v_hs);
        if(parFit.size() == 0) continue;
        bool mathematicaRequired = false;
        getPathLengthInverted(parFit, chanHS, path_length);
        parFit.clear();

		// if(chanHS.size() == 0) continue;
		for(const auto h:barHits){
			bool predHit = false;
			for(const auto pred:chanHS){
				if(h.channel == pred) predHit = true;
			}
			if(predHit){
				++nPass;
				hPass->Fill(0.0);
			}
			++nTotal;
			hTotal->Fill(0.0);
		}
	}
	double a  = static_cast<double>(nPass);
	double b = static_cast<double>(nTotal);
	cout << "\nnPass " << nPass << endl;
	cout << "nTotal " << nTotal << endl;
	cout << "\tEff: " << a/b << " +- " << poissRateError(a,b) << endl;
	cout << "\tEff: " << a/b << " +- " << calcIntervalErrors(hPass,hTotal)[1] << "," <<  calcIntervalErrors(hPass,hTotal)[0] << endl;


	if(cutStudy){
		TFile * outputCutVer = new TFile("barnPECutVer.root", "recreate");
		barnPE->Add(layer1nPE);
		barnPE->Add(layer2nPE);
		barnPE->Add(layer3nPE);
		barnPE->Write();
		layer1nPE->Write();
		layer2nPE->Write();
		layer3nPE->Write();
		outputCutVer->Close();
	}
} // end function