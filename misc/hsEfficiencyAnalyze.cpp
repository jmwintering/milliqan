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

vector<int> barCuts(32, 0);


void hsEfficiencyAnalyze();

# ifndef __CINT__  // the following code will be invisible for the interpreter
int main()
{
     gROOT->ProcessLine(".L /home/llavezzo/hsEfficiencyAnalyze.cpp++");
     hsEfficiencyAnalyze();
}
# endif


void hsEfficiencyAnalyze(){
	TChain * chain = new TChain("t");
	chain->Add("/data/users/llavezzo/methodFiles/hsEfficiency/*.root");
	SetAddressesChainHS(chain);

	TH1D *h1 = new TH1D("h1", "h1", 1000, 0, 10000);

	TH1D *hNPE[32];
	char name2[20];
 	char title2[100];
 	for(Int_t i=0;i<32;i++) {
    sprintf(name2,"hNPE%d",i);
    sprintf(title2,"hNPE%d",i);
    hNPE[i] = new TH1D(name2,title2,1000,0,6000);
 	}


	int entries = chain->GetEntries();

    cout << "added " << entries << " entries" << endl;


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


    //layers = {1:[0,1,24,25,8,9],2:[6,7,16,17,12,13],3:[2,3,22,23,4,5]}
	vector<int> layer1_bars = {0,1,24,25,8,9};
	vector<int> layer_2_bars = {6,7,16,17,12,13};
	vector<int> layer_3_bars = {2,3,22,23,4,5};

	int straightBarEvents = 0;
	int straightHSevents = 0;
	int straightBarAndHSevents = 0;

	double minCut = 0;

	for(int iE=0; iE<entries; ++iE){
		chain->GetEntry(iE);

		//check for straight path through bars
        bool isStraightPath = false;
        bool isStraightHS = false;

		vector<bool> straightPaths(6,false);

		vector<int> chanHS;
		vector<double> path_length;

		double sheetNPE = 0;
		double slabNPE = 0;
		double otherBarsNPE = 0;
		vector<bool> layersHit = {false, false, false, false};

		vector<hit> barHits;

		bool skipEvent = false;
		for(unsigned int j = 0; j < (*v_hs).size(); j++){
			if((*v_hs)[j] == -1) skipEvent = true;
		}
		if(skipEvent) continue;

		for(unsigned int ipulse=0; ipulse < (*v_ipulse).size(); ++ipulse){
			if((*v_ipulse)[ipulse]!=0) continue;
			if((*v_type)[ipulse] == 1) slabNPE += (*v_nPE)[ipulse];
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

			if((*v_type)[ipulse]== 0 && (*v_nPE)[ipulse]< barCuts[(*v_chan)[ipulse]]){
				otherBarsNPE += (*v_nPE)[ipulse];
			}

		}

		if(sheetNPE > 50) continue;
		if(slabNPE < 1000) continue;
		if(otherBarsNPE > 500) continue;
		if(!layersHit.at(3) || !layersHit.at(1) || !layersHit.at(2)) continue;
 		if(barHits.size() != 3) continue;

		for(unsigned int i = 0; i < barHits.size(); i++){
			hNPE[barHits.at(i).channel]->Fill(barHits.at(i).nPE);
		}

		// check for straight paths in bars
		for(const auto firstHit : barHits){
		  for(const auto secondHit : barHits){
			  for(const auto thirdHit : barHits){
					if(firstHit.layer != secondHit.layer && secondHit.layer != thirdHit.layer && firstHit.layer != thirdHit.layer){
						isStraightPath = firstHit.row - secondHit.row == 0 && secondHit.row - thirdHit.row == 0 && firstHit.column - secondHit.column == 0 && secondHit.column - thirdHit.column ==0;

						for(int path=0;path<6;++path){
							if(firstHit.channel == layer1_bars[path] && secondHit.channel == layer_2_bars[path] && thirdHit.channel == layer_3_bars[path]) straightPaths[path] = true;
						} // paths
					} // if
			  } // 3rd hit
		  } //2nd hit
	  } // 1st hit
		if(isStraightPath) ++straightBarEvents;


		/*
    vector<int> hs_botver;
    vector<int> hs_topver;
    vector<int> hs_bothor;
    vector<int> hs_tophor;

		//check for straight HS hits
    for(unsigned int ihs = 0; ihs < (*v_hs).size(); ihs++){
      if((*v_hs)[ihs]<16) hs_botver.push_back(ihs);
      if((*v_hs)[ihs]>=48) hs_topver.push_back(ihs);
      if((*v_hs)[ihs]>=16 && (*v_hs)[ihs]<32) hs_bothor.push_back(ihs);
      if((*v_hs)[ihs]>=32 && (*v_hs)[ihs]<48) hs_tophor.push_back(ihs);
    }

		//FIXME: make code more flexible for, say more than 2 hits for those vectors
		if(hs_topver.size() >= 2 && hs_botver.size() >= 2 &&
			hs_tophor.size() >=2 && hs_bothor.size() >= 2){

	    //if vertical hs bars have same x coordinate, horizontal hs bars
	    //have same y cordinate, straight path
	    int x_var = abs(convertRawToPhysCh(hs_topver[0], true)[0] - convertRawToPhysCh(hs_botver[0], true)[0])
			  + abs(convertRawToPhysCh(hs_topver[1], true)[0] - convertRawToPhysCh(hs_botver[1], true)[0]);
	    int y_var = abs(convertRawToPhysCh(hs_tophor[0], true)[1] - convertRawToPhysCh(hs_bothor[0], true)[1])
	     + abs(convertRawToPhysCh(hs_tophor[1], true)[1] - convertRawToPhysCh(hs_bothor[1], true)[1]);

	    if(x_var + y_var == 0) isStraightHS = true;
	    else isStraightHS = false;
		}
		*/

		if((*v_fit_xz).size() >= 2  && (*v_fit_yz).size() >=  2){

			//fill chanHS with channels hit by HS by the line fit
			vector<double> parFit;

			parFit.push_back((*v_fit_xz)[0]);
			parFit.push_back((*v_fit_xz)[1]);
			parFit.push_back((*v_fit_yz)[0]);
			parFit.push_back((*v_fit_yz)[1]);

			getPathLength(parFit, chanHS, path_length);
			parFit.clear();
	       }


    if(isStraightHS) ++straightHSevents;

		// check for straight bar AND hs events
		if(!isStraightPath) continue;
		int matched = 0;
		for(const auto h : barHits){
			for(unsigned int ihs = 0; ihs < chanHS.size(); ihs++){
				if(h.channel == chanHS.at(ihs)) matched++;
			}
		}
		if(matched >= 3) ++straightBarAndHSevents;
		else{
				//cout << "Bars Hit ";
				for(unsigned int j = 0; j < barHits.size(); j++){
					//cout << barHits.at(j).channel << "(" << barHits.at(j).nPE << ") ";
				}
				//cout << endl;
				//cout << "HS Packs Hit ";
				for(unsigned int j = 0; j < (*v_hs).size(); j++){
				//	cout << (*v_hs)[j] << " ";
				}
				//cout << endl;
				//cout << "Predicted Bars Hit ";
				for(unsigned int j = 0; j < chanHS.size(); j++){
				//	cout << chanHS.at(j) << " ";
				}
				//cout << endl;
			//	cout << "Fit Parameters sizes ";
				//cout << (*v_fit_xz).size() << " " << (*v_fit_yz).size() << endl;

			}

			if(isStraightHS && (*v_fit_xz).size() == 0 && (*v_fit_yz).size() == 0){
					int coord_x = barHits.at(0).column;
					int coord_y = barHits.at(0).row;

					vector<int> hs_coord_x;
					vector<int> hs_coord_y;
					for(unsigned int j = 0; j < (*v_hs).size(); j++){
						hs_coord_x.push_back(convertRawToPhysCh((*v_hs)[j],true))[0];
						hs_coord_y.push_back(convertRawToPhysCh((*v_hs)[j],true))[1];
					}

					for(unsigned int j = 0; j < hs_coord_x.size(); j++){

						//convert HS x coordinates to columns
						if(hs_coord_x.at(j) > 0) hs_coord_x.at(j) = 1;
						if(hs_coord_x.at(j) < 0) hs_coord_x.at(j) = 0;

						//convert HS y coordinates to rows
						if(hs_coord_y.at(j) <= 3) hs_coord_y.at(j) == 0;
						if(hs_coord_y.at(j) >= 4 && hs_coord_y.at(j) <= 5) hs_coord_y.at(j) == 1;
						if(hs_coord_y.at(j) >= 6) hs_coord_y.at(j) == 2;
					}



					bool notAligned = false;
					for(unsigned int j = 0; j < hs_coord_x.size(); j++){
						if(hs_coord_x[j] != coord_x  || hs_coord_y[j] != coord_y) notAligned = true;
					}

					if(!notAligned) weak_straightBarAndHSevents++;


			}



	} // end chain loop

	cout << "Bar: " << straightBarEvents << endl;
    cout << "HS: " << straightHSevents << endl;
    cout << "Bar & HS: " << straightBarAndHSevents << endl;
	cout << "Weak Bar & HS: " << weak_straightBarAndHSevents << endl;

	h1->Draw();

	TFile* outputFile = new TFile("bar_muon_nPE.root", "RECREATE");
	for(int i = 0; i < 32; i++){
		hNPE[i]->Write();
	}
	outputFile->Close();

} // end function
