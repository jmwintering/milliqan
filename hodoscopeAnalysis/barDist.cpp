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

vector<float> convertBarToPhysCoords(int row, int column){
	// returns physical coordinates of bars in xy plane
	vector<float> barPhysCoords = {-1,-1};

	if(row == 1) barPhysCoords[1] = 0.8;
	else if(row == 2) barPhysCoords[1] = 7.5;
	else if(row == 3) barPhysCoords[1] = 13.9;
	if(column == 1) barPhysCoords[0] = -2.5;
	else if(column == 2) barPhysCoords[0] = 2.5;

	return barPhysCoords;
}

void barDist(){
	TChain * c = new TChain("t");
	fillBars();

	int nHits = 0;
	c->Add("/data/methodFiles/barEfficiency/*.root");
	SetAddressesChainHS(c);
	int entries = c->GetEntries();
	cout << "Added " << entries << " entries" << endl;

	TFile * outputFile = new TFile("distHists.root", "RECREATE");
	TH2D * barDist = new TH2D("barDist", "barDist ; {delta} X (bar - HS) [cm]; {delta} Y (bar - HS) [cm]", 25, -12.5, 12.5, 25,-12.5,12.5);
	TH1D * xDist = new TH1D("xDist", "xDist ; {delta} X (bar - HS)[cm] ", 25, -12.5, 12.5);
	TH1D * yDist = new TH1D("yDist", "yDist ; {delta} Y (bar - HS)[cm] ", 25, -12.5, 12.5);
	TH1D * barNPE[bars.size()];

	for(int i=0; i < bars.size(); ++i){
		barNPE[i] = new TH1D("bar"+toTstring(tostr(bars[i])), "bar"+toTstring(tostr(bars[i]))+"; nPE", 150, 0, 5000);
	}

	for(int iE=0; iE<entries;++iE){
		c->GetEntry(iE);

		if(!beam) continue;

		// convert to phys coords
		vector<int> botHor = {};
		vector<int> botVer = {};
		vector<int> topHor = {};
		vector<int> topVer = {};
		for(unsigned int i = 0; i < (*v_hs).size(); i++){
			if((*v_hs)[i] == -1) continue;
			if((*v_hs)[i] >= 0 && (*v_hs)[i] <= 15) botVer.push_back(convertRawToPhysCh((*v_hs).at(i), true)[0]);
			if((*v_hs)[i] >= 16 && (*v_hs)[i] <= 31) botHor.push_back(convertRawToPhysCh((*v_hs).at(i), true)[1]);
			if((*v_hs)[i] >= 32 && (*v_hs)[i] <= 47) topHor.push_back(convertRawToPhysCh((*v_hs).at(i), true)[1]);
			if((*v_hs)[i] >= 48 && (*v_hs)[i] <= 63) topVer.push_back(convertRawToPhysCh((*v_hs).at(i), true)[0]);
		}
		if(botHor.size() == 0 || botVer.size() == 0 ||topHor.size() == 0 || topVer.size() == 0) continue;

		// select straight HS events
		bool horHS(true), verHS(true);

		// hor events are all same
		for(auto coordi:botHor){
			for(auto coordj:topHor){
				if(coordi != coordj) horHS = false;
			}
		}
		// ver events all same
		for(auto coordi:botVer){
			for(auto coordj:topVer){
				if(coordi != coordj) horHS = false;
			}
		}
		if(!horHS || !verHS) ++nHits;

		vector<hit> barHits = {};

		bool sheetsActive = false;
		int slabsActive = 0;
		for(unsigned int ipulse=0; ipulse< (*v_ipulse).size(); ++ipulse){
			if((*v_ipulse)[ipulse]!=0) continue;
			if((*v_type)[ipulse] ==2) sheetsActive = true;
			if((*v_type)[ipulse] ==1) ++slabsActive;
		}

		for(unsigned int ipulse=0; ipulse < (*v_ipulse).size(); ++ipulse){
			if((*v_ipulse)[ipulse]!=0) continue;
			if((*v_type)[ipulse] != 0) continue;
			for(int h=0; h< bars.size(); ++h){
				if((*v_chan)[ipulse] != bars[h]) continue;
				if(sheetsActive) continue;
				if(slabsActive != 4) continue;
				barNPE[h]->Fill((*v_nPE)[ipulse]);
			}
			if((*v_nPE)[ipulse] < 50) continue;
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
		}
		// FIXME: loosen to allow small noise

		if(barHits.size() != 3) continue;
		bool isStraightPath = false;
		for(auto h1:barHits){
			for(auto h2:barHits){
				for(auto h3:barHits){
					if(h1.layer == h2.layer || h1.layer == h3.layer || h3.layer == h2.layer) continue;
					isStraightPath = h1.row == h2.row && h1.row == h3.row && h1.column == h2.column && h1.column == h3.column;
				}
			}
		}

		if(!isStraightPath) continue;



		// convert coords to phys coords
		vector<float> barPhysCoords = {};
		vector<float> hsPhysCoords = {};

		vector<float> hsPhysCoordsX = {};
		vector<float> hsPhysCoordsY = {};
		barPhysCoords = convertBarToPhysCoords(barHits[0].row, barHits[0].column);
		for(auto ihor: botHor){
			hsPhysCoordsX.push_back(convertPhysChToCoord({ihor},true)[0]);
		}
		for(auto ihor: topHor){
			hsPhysCoordsX.push_back(convertPhysChToCoord({ihor},true)[0]);
		}
		for(auto iver: botVer){
			hsPhysCoordsY.push_back(convertPhysChToCoord({iver},true)[1]);
		}
		for(auto iver: topVer){
			hsPhysCoordsY.push_back(convertPhysChToCoord({iver},true)[1]);
		}
		if(horHS && verHS){
			hsPhysCoords = {convertPhysChToCoord({botHor[0]},true)[0], convertPhysChToCoord({botVer[0]}, true)[1]};
			// Now graph difference between bar coordinates and HS coordinates
			barDist->Fill(barPhysCoords[0]-hsPhysCoords[0],barPhysCoords[1]-hsPhysCoords[1]);
		}

		for(auto icoord: hsPhysCoordsX) xDist->Fill(barPhysCoords[0]-icoord);
		for(auto icoord: hsPhysCoordsY) yDist->Fill(barPhysCoords[1]-icoord);
	}

	cout << "nHits: " << nHits << endl;
	gStyle->SetPalette(56);
	barDist->SetOption("colz");
	barDist->Write();
	yDist->Write();
	xDist->Write();

	for(auto h:barNPE) h->Write();

	outputFile->Close();
	cout << "output file closed." << endl;

}
