/*
	
	Selecting through-going particles, determine the distance of the
	hodoscope hits from the bar hits in the x and y planes for each
	straight path.


*/


#include "treesV16Template.h"

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

vector<int> barCuts(32, 1000);

void hsEfficiencyAnalyze2(){

	TChain * chain = new TChain("t");
	chain->Add("/data/users/wintering/methodFiles/hsEfficiency2/*.root");
	SetAddressesChainHS(chain);

	vector<int> layer1_bars = {0,1,24,25,8,9};
	vector<int> layer2_bars = {6,7,16,17,12,13};
	vector<int> layer3_bars = {2,3,22,23,4,5};

	TH1D *hDistX[7];
	for(int i=0;i<7;i++) {
		TString name = "Path X " + toTstring(tostr(i+1)) + ": " + toTstring(tostr(layer1_bars[i])) + toTstring(tostr(layer2_bars[i])) + toTstring(tostr(layer3_bars[i])) + "; (HS x-coord) - (Bar x-coord) [cm]";
		TString title = name;
		hDistX[i] = new TH1D(name,title,20,-10,10);
	}

	TH1D *hDistY[7];
	for(int i=0;i<7;i++) {
		TString name = "Path Y " + toTstring(tostr(i+1)) + ": " + toTstring(tostr(layer1_bars[i])) + toTstring(tostr(layer2_bars[i])) + toTstring(tostr(layer3_bars[i])) + "; (HS y-coord) - (Bar y-coord) [cm]";
		TString title = name;
		hDistY[i] = new TH1D(name,title,30,-15,15);
	}

	TH1D * xDist_HS = new TH1D("xDist_HS", "xDist_HS; delta X (hs i - hs j)", 100, -10,10);
	TH1D * yDist_HS = new TH1D("yDist_HS", "yDist_HS; delta y (hs i - hs j)", 100, -10,10);

	//cosmics
	/*
	barCuts[0] = 2900;
	barCuts[1] = 2700;
	barCuts[2] = 2500;
	barCuts[3] = 2500;
	barCuts[4] = 2800;
	barCuts[5] = 900;
	barCuts[6] = 3400;
	barCuts[7] =  2800;
	barCuts[8] = 2500;
	barCuts[9] = 1400;
	barCuts[12] = 2900;
	barCuts[13] = 2000;
	barCuts[16] = 5700;
	barCuts[17] = 2000;
	barCuts[22] = 650;
	barCuts[23] = 3000;
	barCuts[24] = 2000;
	barCuts[25] = 3600;
	*/

	vector<int> layer_1_bars = {0,1,24,25,8,9};
	vector<int> layer_2_bars = {6,7,16,17,12,13};
	vector<int> layer_3_bars = {2,3,22,23,4,5};
	vector<int> slabs = {18, 20, 28, 21};
	int straightBarEvents = 0;
	int straightHSevents = 0;
	int straightBarAndHSevents = 0;
	int weak_straightBarAndHSevents = 0;
	double minCut = 0;

	int entries = chain->GetEntries();
	cout << "added " << entries << " entries" << endl;

	for(int iE=0; iE<entries; ++iE){
		chain->GetEntry(iE);

		//check for straight path through bars
    	bool isStraightPath = false;
		vector<bool> straightPaths(6,false);
		double sheetNPE = 0;
		double slabNPE = 0;
		double otherBarsNPE = 0;
		vector<bool> layersHit = {false, false, false, false};
		vector<bool> slabsHit = {false, false, false, false};
		vector<hit> barHits;

		//consider only events with HS data
		bool skipEvent = false;
		for(unsigned int j = 0; j < (*v_hs).size(); j++){
			if((*v_hs)[j] == -1) skipEvent = true;
		}
		if(skipEvent) continue;

		if(!beam) continue;

		for(unsigned int ipulse=0; ipulse < (*v_ipulse).size(); ++ipulse){
			if((*v_ipulse)[ipulse]!=0) continue;
			if((*v_type)[ipulse] == 1) {
				slabNPE += (*v_nPE)[ipulse];
				for(unsigned int j = 0; j < slabs.size(); j++){
					if(slabs[j] == (*v_chan)[ipulse]) slabsHit[j] = true;
				}
			}
			if((*v_type)[ipulse] == 2) sheetNPE += (*v_nPE)[ipulse];

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

			if((*v_type)[ipulse]== 0 && (*v_nPE)[ipulse]< barCuts[(*v_chan)[ipulse]] && (*v_nPE)[ipulse] > 50){
				otherBarsNPE += (*v_nPE)[ipulse];
			}

		}

		//selections
		//if(sheetNPE > 50) continue;
		//if(slabNPE < 1000) continue;
		if(otherBarsNPE > 0) continue;
		if(!slabsHit[0] || !slabsHit[1] || !slabsHit[2] || !slabsHit[3]) continue;
		if(!layersHit.at(3) || !layersHit.at(1) || !layersHit.at(2)) continue;
 		if(barHits.size() != 3) continue;

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
	
		if(!isStraightPath) continue;
	

		//for straight events that were not able to matched due to
		//not enough hits in the HS, look to see where the HS was acutally hit,
		//and look if it was in the same row, col as the bars that were hit
		vector<float> hs_coord_x;
		vector<float> hs_coor
		for(unsigned int j = 0; j < (*v_hs).size(); j++){
			vector<int> coord_index = convertRawToPhysCh((*v_hs)[j],true);
			if(coord_index.size() < 3) continue;
			if(coord_index.at(0) != 0) hs_coord_x.push_back(convertPhysChToCoord(coord_index, true)[0]);
			if(coord_index.at(1) != 0) hs_coord_y.push_back(convertPhysChToCoord(coord_index, true)[1]);
		}

		for(int iX=0; iX<hs_coord_x.size(); ++iX){
			for(int jX=0; jX<hs_coord_x.size();++jX){
				if(iX == jX) continue;
				xDist_HS->Fill(hs_coord_x[iX] - hs_coord_x[jX]);
			}
		}

		for(int iy=0; iy<hs_coord_y.size(); ++iy){
			for(int jy=0; jy<hs_coord_y.size();++jy){
				if(iy == jy) continue;
				yDist_HS->Fill(hs_coord_y[iy] - hs_coord_y[jy]);
			}
		}


		// convert to HS coords
		int coord_x = 0;
		int coord_y = 0;
		if(barHits.at(0).column == 2) coord_x = 3;
		if(barHits.at(0).column == 1) coord_x = -3;
		if(barHits.at(0).row == 1) coord_y = 0;
		if(barHits.at(0).row == 2) coord_y = 6;
		if(barHits.at(0).row == 3) coord_y = 12;

		for(unsigned int j = 0; j < hs_coord_x.size(); j++){

			for(unsigned int iPath = 0; iPath < straightPaths.size(); iPath++){
				if(straightPaths[iPath]) {
						hDistX[iPath]->Fill(hs_coord_x[j] - coord_x);
					}

			}

			hDistX[6]->Fill(hs_coord_x[j] - coord_x);

		}

		for(unsigned int j = 0; j < hs_coord_y.size(); j++){

			for(unsigned int iPath = 0; iPath < straightPaths.size(); iPath++){
				if(straightPaths[iPath]) {
						hDistY[iPath]->Fill(hs_coord_y[j] - coord_y);
					}

			}

			hDistY[6]->Fill(hs_coord_y[j] - coord_y);

		}

		hs_coord_x.clear();
		hs_coord_y.clear();

	} // end chain loop

	gStyle->SetOptStat(0);

    TString outputString = "hDist_Beam.root";
	TFile * outputFile = new TFile(outputString, "RECREATE");
    TH1D* hDistX_total = new TH1D("totalX", "totalX; (HS x-coord) - (Bar x-coord) [cm] ",20,-10,10);
    TH1D* hDistY_total = new TH1D("totalY", "totalY; (HS x-coord) - (Bar y-coord) [cm] ",30,-15,15);
    
	for(int i = 0; i < 7; i++){
		hDistX[i]->Write();
		hDistY[i]->Write();
        hDistX_total->Add(hDistX[i]);
        hDistY_total->Add(hDistY[i]);
	}
    hDistX_total->SetTitle("totalX");
    hDistY_total->SetTitle("totalY");
    hDistX_total->Write();
    hDistY_total->Write();
	xDist_HS->Write();
	yDist_HS->Write();
	outputFile->Close();
    cout << "Wrote files to " << outputString << endl;


} // end function
