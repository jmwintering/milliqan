/*
	
	Selecting through-going particles, use the hodoscope fit to determine
	how many events are well predicted, how many are just well aligned
	(i.e. the hits in the HS match the x and y coordinates of the bars, but there
	aren't enough hits to draw a line), and how many aren't good.

*/



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


void hsEfficiencyAnalyze(){

	TChain * chain = new TChain("t");
	chain->Add("/data/hsEfficiency2/*.root");
	SetAddressesChainHS(chain);

	vector<int> barCuts(32, 400);
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

	vector<double> slabCuts(4,0);
	slabCuts[0] = 300;
	slabCuts[1] = 350;
	slabCuts[2] = 150;
	slabCuts[3] = 350;

	vector<int> layer_1_bars = {0,1,24,25,8,9};
	vector<int> layer_2_bars = {6,7,16,17,12,13};
	vector<int> layer_3_bars = {2,3,22,23,4,5};
	vector<int> slabs = {18, 20, 28, 21};
	int straightBarEvents = 0;
	int straightHSevents = 0;
	int straightBarAndHSevents = 0;
	int weak_straightBarAndHSevents = 0;
	double minCut = 0;
	int hit_events = 0;
	int no_hit_events = 0;
	int pred_hit_pulses = 0;
	int nopred_hit_pulses = 0;
	int wellaligned_x(0), wellaligned_y(0), badlyaligned_x(0), badlyaligned_y(0);

	int entries = chain->GetEntries();
	cout << "added " << entries << " entries" << endl;

	for(int iE=0; iE<entries; ++iE){
		chain->GetEntry(iE);


		//check for straight path through bars
    	bool isStraightPath = false;
		bool isStraightHS = false;
		bool matched = false;
		bool weak_matched = false;
		vector<bool> straightPaths(6,false);
		vector<bool> straightPathsHS(6,false);
		vector<int> chanHS;
		vector<double> path_length;
		double sheetNPE = 0;
		double slabNPE = 0;
		double otherBarsNPE = 0;
		vector<bool> layersHit = {false, false, false, false};
		vector<bool> slabsHit = {false, false, false, false};
		vector<hit> barHits;
		
		if(!beam) continue;

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

		
		//select muons
		if(sheetNPE > 100) continue;
		////if(slabNPE < 1000) continue;
		if(otherBarsNPE > 200) continue;
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
		if(isStraightPath) ++straightBarEvents;

		

		//if((*v_fit_xz).size() >= 2  && (*v_fit_yz).size() >=  2){

			//fill chanHS with channels hit by HS by the line fit
			// vector<double> parFit;

			// parFit.push_back((*v_fit_xz)[0]);
			// parFit.push_back((*v_fit_xz)[1]);
			// parFit.push_back((*v_fit_yz)[0]);
			// parFit.push_back((*v_fit_yz)[1]);

			// getPathLength(parFit, chanHS, path_length);
			// parFit.clear();
			vector<double> parFit = {};
			parFit = doCircleFitInverted(v_hs);
			if(parFit.size() == 0 ) continue;
			bool test = false;
			getCirclePathLengthInverted(parFit, chanHS, path_length, runNum, fileNum, event, "test",test );
		parFit.clear();
    	//}
		

    	
		// check for straight bar AND hs events
		if(!isStraightPath) continue;
		int pulses_matched = 0;
		for(const auto h : barHits){
			for(unsigned int ihs = 0; ihs < chanHS.size(); ihs++){
				if(h.channel == chanHS.at(ihs)) pulses_matched++;
			}
		}

		

		//for events with predictions, how many were pulses were predicted?
		if(chanHS.size() != 0){
			pred_hit_pulses += pulses_matched;
			nopred_hit_pulses += (3-pulses_matched);
			if(nopred_hit_pulses < 0) cout << "ERROR" << endl;
		

			cout << runNum << " " << fileNum << " " << event << endl;
			cout << barHits[0].channel << " " << barHits[1].channel << " " << barHits[2].channel << endl;
			for(int j = 0; j < chanHS.size(); j++){
				cout << chanHS[j] << " ";
			}
			cout << endl << endl;
		}

		// check to see which pulses match
		for(unsigned int chan1 = 0; chan1 < chanHS.size(); ++chan1){
			for(unsigned int chan2 = 0; chan2 < chanHS.size(); ++chan2){
				for(unsigned int chan3 = 0; chan3 < chanHS.size(); ++chan3){
					for(int path=0; path<6;++path){
						if(chanHS[chan1] == layer_1_bars[path] && chanHS[chan2] == layer_2_bars[path] && chanHS[chan3] == layer_3_bars[path]){
							straightPathsHS[path] = true;
							isStraightHS = true;
						}
					}
				}
			}
		}
		if(isStraightHS) ++straightHSevents;


		

		if(pulses_matched >= 3) {
			++straightBarAndHSevents;
			matched = true;
		}/*
		else{
			cout << chanHS.size() << endl;

			if(chanHS.size() != 0){
				cout << "v_hs" << endl;
				for(unsigned int j = 0; j < (*v_hs).size();j++){
					cout << (*v_hs)[j] << " ";
				}
				cout << endl;
				cout << "chanHS" << endl;
				for(unsigned int j = 0; j < chanHS.size();j++){
					cout << chanHS[j] << " ";
				}
				cout << endl;
				cout << "barHits" << endl;
				for(unsigned int j = 0; j < barHits.size();j++){
					cout << barHits[j].channel << " ";
				}
				cout << endl;
				cout << "v_fit_xz" << endl;
				for(unsigned int j = 0; j < (*v_fit_xz).size();j++){
					cout << (*v_fit_xz)[j] << " ";
				}
				cout << endl;
				cout << "v_fit_yz" << endl;
				for(unsigned int j = 0; j < (*v_fit_yz).size();j++){
					cout << (*v_fit_yz)[j] << " ";
				}
				cout << endl;
			}
			for(unsigned int ihs = 0; ihs < chanHS.size(); ihs++){
				bool therewasahit = false;
				for(const auto h : barHits){
					if(h.channel == chanHS.at(ihs)) therewasahit = true;
				}
				if(!therewasahit) {
					cout << "there was no hit" << endl; 
					no_hit_events++;
				}
				if(therewasahit) {
					cout << "there was a hit" << endl;
					hit_events++;
				}
			}
		}*/

		bool offaxis = false;
		if(isStraightHS && false){
			int coord_x = 0;
			int coord_y = 0;
			/*
			if(barHits.at(0).column == 2) coord_x = 3;
			if(barHits.at(0).column == 1) coord_x = -3;
			if(barHits.at(0).row == 1) coord_y = 0;
			if(barHits.at(0).row == 2) coord_y = 6;
			if(barHits.at(0).row == 3) coord_y = 12;
			*/
			if(chanHS.at(0)%2 == 1) coord_x = 3;
			if(chanHS.at(0)%2 == 0) coord_x = -3;
			if(chanHS.at(0) == 0 || chanHS.at(0) == 1 || 
				chanHS.at(0) == 6 || chanHS.at(0) == 7 ||
				chanHS.at(0) == 2 || chanHS.at(0) == 3) coord_y = 0;
			if(chanHS.at(0) > 15) coord_y = 6;
			if(chanHS.at(0) == 8 || chanHS.at(0) == 9 || 
				chanHS.at(0) == 12 || chanHS.at(0) == 13 ||
				chanHS.at(0) == 4 || chanHS.at(0) == 5) coord_y = 12;
			vector<double> hs_coord_x = {};
			vector<double> hs_coord_y = {};
			for(unsigned int j = 0; j < (*v_hs).size(); j++){
				vector<int> coord_index = convertRawToPhysCh((*v_hs)[j],true);
				if(coord_index.size() < 3) continue;
				if(coord_index.at(0) != 0) hs_coord_x.push_back(convertPhysChToCoord(coord_index, true)[0]);
				if(coord_index.at(1) != 0) hs_coord_y.push_back(convertPhysChToCoord(coord_index, true)[1]);
			}
			for(unsigned int j = 0; j < hs_coord_x.size(); j++){
				if(abs(hs_coord_x[j] - coord_x) > 2.5) offaxis = true;
			}
			for(unsigned int j = 0; j < hs_coord_y.size(); j++){
				if(abs(hs_coord_y[j] - coord_y) > 2.5) offaxis = true;
			}
			hs_coord_x.clear();
			hs_coord_y.clear();
		}

				

		vector<double> hs_coord_x2 = {};
		vector<double> hs_coord_y2 = {};
		bool wellaligned = true;
		if(isStraightPath){
			int bar_coord_x = 0;
			int bar_coord_y = 0;
			
			if(barHits.at(0).column == 2) bar_coord_x = 3;
			if(barHits.at(0).column == 1) bar_coord_x = -3;
			if(barHits.at(0).row == 1) bar_coord_y = 0;
			if(barHits.at(0).row == 2) bar_coord_y = 6;
			if(barHits.at(0).row == 3) bar_coord_y = 12;
		

			for(unsigned int j = 0; j < (*v_hs).size(); j++){
				vector<int> coord_index2 = convertRawToPhysCh((*v_hs)[j],true);
				if(coord_index2.size() < 3) continue;

				if(coord_index2.at(0) != 0) {
					hs_coord_x2.push_back(convertPhysChToCoord(coord_index2, true)[0]);
				}
				if(coord_index2.at(1) != 0) {
					hs_coord_y2.push_back(convertPhysChToCoord(coord_index2, true)[1]);
				}

			}
	
			for(unsigned int j = 0; j < hs_coord_x2.size(); j++){
				if(abs(hs_coord_x2[j] - bar_coord_x) > 3.5) {
					wellaligned = false;
					badlyaligned_x++;
				}
				else wellaligned_x++;
			}
			for(unsigned int j = 0; j < hs_coord_y2.size(); j++){
				if(abs(hs_coord_y2[j] - bar_coord_y) > 3.5) {
					wellaligned = false;
					badlyaligned_y++;
				}
				else wellaligned_y++;
			}

		}

				

		// if(!matched && chanHS.size() != 0){
		// 	for(int j = 0; j < chanHS.size(); j++){
		// 		cout << chanHS[j] << " ";
		// 	}
		// 	cout << endl;
		// 	for(int j = 0; j < barHits.size(); j++){
		// 		cout << barHits.at(j).channel << " ";
		// 	}
		// 	cout << endl;

		// 	cout << "v_fit_xz" << endl;
		// 	for(unsigned int j = 0; j < (*v_fit_xz).size();j++){
		// 		cout << (*v_fit_xz)[j] << " ";
		// 	}
		// 	cout << endl;
		// 	cout << "v_fit_yz" << endl;
		// 	for(unsigned int j = 0; j < (*v_fit_yz).size();j++){
		// 		cout << (*v_fit_yz)[j] << " ";
		// 	}
		// 	cout << endl;
		// 	cout << "v_hs" << endl;
		// 	for(unsigned int j = 0; j < (*v_hs).size();j++){
		// 		cout << (*v_hs)[j] << " ";
		// 	}
		// 	cout << endl;
		// 	cout << wellaligned << endl;
		// 	cout << endl;

		// }


		if(isStraightPath){
			if(wellaligned && (hs_coord_x2.size() >= 1 || hs_coord_y2.size() >= 1)) {
				weak_straightBarAndHSevents++;
			}
		}

		hs_coord_x2.clear();
		hs_coord_y2.clear();

			/*

			total straight, high energy events (378) out of? total time range?
			- predicted (116)
			- not perfect prediction (378-116 = 262)
				- wrong prediction (35)
					- how many of the three possible pulses are matched?
						- 2 (28)
						- 1 (7)
						- 0 (8)                   -> Why were some predictions wrong? Look at events
				- no prediction (262-35 = 227)
					- 0 hodoscope hits (21)         -> Why were there no hodoscope hits? Inefficiency? Look at events
					- not enough hs hits (206)
						- aligned (145)              -> Why do so many not have enough hits to make a prediction, but are generrally aligned?
						- not fully aligned
						- not at all aligned

			*/

	} // end chain loop

	cout << "Bar: " << straightBarEvents << endl;
  	cout << "HS: " << straightHSevents << endl;
  	cout << "Bar & HS: " << straightBarAndHSevents << endl;
	cout << "Weak Bar & HS: " << weak_straightBarAndHSevents << endl;
	cout << "Weak Efficiency: " << (weak_straightBarAndHSevents+straightBarAndHSevents*1.0 ) / straightBarEvents;
	cout << " +- " << sqrt( ((weak_straightBarAndHSevents+straightBarAndHSevents)/(pow(straightBarEvents,2)) ) + ( (pow(weak_straightBarAndHSevents+straightBarAndHSevents,2)) / (pow(straightBarEvents,3)))) << endl;
	//cout << "Predicted pulses with a hit: " << hit_events << endl;
	//cout << "Predicted pulses with no hit in them: " << no_hit_events << endl;
	cout << endl;
	cout << "Pulses with hit and pred: " << pred_hit_pulses << endl;
	cout << "Pulses with hit and no pred: " << nopred_hit_pulses << endl;
	double error = sqrt( ((pred_hit_pulses*1.0)/(pow(pred_hit_pulses+nopred_hit_pulses,2))) + ( (pow(pred_hit_pulses,2))*1.0 / (pow(pred_hit_pulses+nopred_hit_pulses,3))));
	cout << "Efficiency of line fit for muons: " << pred_hit_pulses*1.0 / (pred_hit_pulses + nopred_hit_pulses) << " +- " << error << endl;
	cout << endl;
	cout << "Well aligned in x: " << wellaligned_x << endl;
	cout << "Well aligned in y: " << wellaligned_y << endl;
	cout << "Badly aligned in x: " << badlyaligned_x << endl;
	cout << "Badly aligned in y: " << badlyaligned_y << endl;

	gStyle->SetOptStat(0);

} // end function
