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

void barEffRadiationStudies(){
	TChain * chain = new TChain("t");
	chain->Add(/*add radiation studies*/);
	SetAddressesChainHS(chain);
	int entries = chain->GetEntries();
	if(entries == 0){
		cout << "Error: added 0 entries" << endl;
		return;
	}
	else{
		cout << "added " << entries << " entries" << endl;
	}

	fillBars();

	int firstRow = 0;
	int secRow = 0;



	for(int iE=0; iE<entries; ++iE){
		chain->GetEntry(iE);

		vector<hit> barsHit = {};
		for(unsigned iP=0; iP<(*v_ipulse).size(); ++iP){
			if((*v_ipulse)[iP] != 0) continue;
			if((*v_type)[iP] == 0){
				hit thisHit;
				thisHit.type = (*v_type)[j];
		    	thisHit.column = (*v_column)[j];
		    	thisHit.row = (*v_row)[j];
		    	thisHit.layer = (*v_layer)[j];
		    	thisHit.height = (*v_height)[j];
		    	thisHit.area = (*v_height)[j];
		    	thisHit.nPE = (*v_nPE)[j];
		    	thisHit.iPulse = (*v_ipulse)[j];
		    	thisHit.channel = (*v_chan)[j];
		    	thisHit.time = (*v_time_module_calibrated)[j];
				barsHit.push_back(thisHit);
			}
		}

		for(unsigned int iB1=0; iB1<barShit.size(); ++iB1){
			hit h1 = barsHit[iB1];
			for(unsigned int iB3=0; iB3<barShit.size(); ++iB3){
				hit h3 = barsHit[iB3];
				if(h1.layer != h2.layer) continue;
				if(h1.column != h2.column) continue;

				if(h3.row == 3){
					if(h2.row == 2) ++secRow;
					if(h2.row == 1) ++firstRow;

				}


			}
		}


	} // end chain loop








}
