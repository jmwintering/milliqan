#include "treesV16Template.h"

void doLeastSquaresFit(const vector<int> hsChans);

void checkFitWithJae(){
	TChain * cOSU = new TChain("t");
	TChain * cJAE = new TChain("t");

	cOSU->Add("/data/users/methodFiles/barEfficiency/*.root");
	cJAE->Add("/data/users/skim_fourslabs_b0b1synched_hsadded_run1079to1375.root");

	SetAddressesChainHS(cOSU);
	SetAddressesChainHSJae(cJAE);

	vector<int> eventsJae(2000, 0);
	vector<int> eventsOSU(2000, 0);

	int entriesJae = cJAE->GetEntries();
	int entriesOSU = cOSU->GetEntries();

	TH1D * hFityz = new TH1D("hFityz", "hitFityz", 1000, -100, 100);
	TH1D * hFitxz = new TH1D("hFitxz", "hitFitxz", 1000, -100, 100);

	struct eventInfo {
		int run;
		int event;
		int file;
		vector<double> fit_xz;
		vector<double> fit_yz;
		vector<int> hp;
		double hs_time;
		vector<int> chan;
		vector<int> predictedChans;
	};

	vector<eventInfo> jaeSkim = {};
	vector<eventInfo> osuSkim = {};

	vector<vector<int>>  eventsNotMatched = {};

	for(int iE=0; iE < entriesJae; ++iE){
		cJAE->GetEntry(iE);


		if((*v_fit_xz).size() == 0 && (*v_fit_yz).size() == 0) continue;
		++eventsJae[runNum];


		vector<int> chanHS = {};
		vector<double> path_length= {};
		vector<float> parFit= {};
		bool sheetsHit = false;
		if((*v_fit_xz).size() >= 2  && (*v_fit_yz).size() >=  2) {

				//fill chanHS with channels hit by HS by the line fit
				vector<double> parFit= {};

				parFit.push_back((*v_fit_xz)[0]);
				parFit.push_back((*v_fit_xz)[1]);
				parFit.push_back((*v_fit_yz)[0]);
				parFit.push_back((*v_fit_yz)[1]);

				getPathLength(parFit, chanHS, path_length);
				parFit.clear();
		}

		eventInfo thisEvent;
		thisEvent.run = runNum;
		thisEvent.file = fileNum;
		thisEvent.event = event;
		thisEvent.fit_xz = (*v_fit_xz);
		thisEvent.fit_yz = (*v_fit_yz);
		thisEvent.hp = (*v_hs);
		thisEvent.chan = (*v_chan);
		thisEvent.hs_time = hs_time;
		thisEvent.predictedChans = chanHS;
		jaeSkim.push_back(thisEvent);

	}

	for(int iE=0; iE < entriesOSU; ++iE){
		cOSU->GetEntry(iE);

		//if(runNum < 1069 || runNum > 1375) continue;

		//if((*v_fit_xz).size() == 0 && (*v_fit_yz).size() == 0 ) continue;

		// boards synched
		// 4 slabs

		vector<vector<float>> hsChansPhysCoords = {};
		for(unsigned int i = 0; i < (*v_hs).size(); i++){
			if((*v_hs)[i] == -1) continue;
			hsChansPhysCoords.push_back(convertPhysChToCoord(convertRawToPhysCh((*v_hs).at(i), true), true));
		}
		if((*v_hs).size() != 0) {
		//cout << "run " << runNum << " event " << event << " file " << fileNum << endl;
		for(unsigned int iVector =0; iVector< hsChansPhysCoords.size(); ++iVector){
			for(unsigned int iCoord=0; iCoord < hsChansPhysCoords[iVector].size(); ++iCoord){
				//cout << hsChansPhysCoords[iVector][iCoord] << endl;
				}
			}
		}

		vector<int> chanHS = {};
		vector<double> path_length= {};
		vector<float> parFit= {};
		bool sheetsHit = false;
		if((*v_fit_xz).size() >= 2  && (*v_fit_yz).size() >=  2) {

				//fill chanHS with channels hit by HS by the line fit
				vector<double> parFit= {};

				parFit.push_back((*v_fit_xz)[0]);
				parFit.push_back((*v_fit_xz)[1]);
				parFit.push_back((*v_fit_yz)[0]);
				parFit.push_back((*v_fit_yz)[1]);

				getPathLength(parFit, chanHS, path_length);
				parFit.clear();
		}



		eventInfo thisEvent;
		thisEvent.run = runNum;
		thisEvent.file = fileNum;
		thisEvent.event = event;
		thisEvent.fit_xz = (*v_fit_xz);
		thisEvent.fit_yz = (*v_fit_yz);
		thisEvent.hp = (*v_hs);
		thisEvent.chan = (*v_chan);
		thisEvent.hs_time = hs_time;
		thisEvent.predictedChans = chanHS;
		osuSkim.push_back(thisEvent);
		++eventsOSU[runNum];
	}

	//return;
	cout << "yeet?" << endl;
	cout << jaeSkim.size() << " " << osuSkim.size() << endl;

	int amtMatched = 0;
	int amtNotMatched = 0;
	for(unsigned int i=0; i< jaeSkim.size(); ++i){
		eventInfo hJae = jaeSkim[i];
		for(unsigned int j=0; j< osuSkim.size(); ++j){
			eventInfo hOsu = osuSkim[j];
			if(hJae.run != hOsu.run || hJae.file != hOsu.file || hJae.event != hOsu.event) continue;
			//if(hOsu.file > 10 ) continue;

			cout << "run, file, event: " << hJae.run << " "  <<   hJae.file << " " <<  hJae.event  << endl;

			if(hJae.fit_yz != hOsu.fit_yz){

			hFityz->Fill(hJae.fit_yz[0] - hOsu.fit_yz[0]);
			hFityz->Fill(hJae.fit_yz[1] - hOsu.fit_yz[1]);

			//cout << "fit_yz: " << endl;
		//	cout << "\tJae:" << endl;
				 for(unsigned int iF=0; iF< hJae.fit_yz.size(); ++iF){
				// 	cout << " " << hJae.fit_yz[iF] << endl;
				}
			//cout << "\tOsu" << endl;
				for(unsigned int iF=0; iF< hOsu.fit_yz.size(); ++iF){
				  // cout << " " << hOsu.fit_yz[iF] << endl;
				}


			//cout << " Jae: HS, bars " << endl;
			//for(int ihit=0; ihit< hJae.hp.size(); ++ihit) cout << hJae.hp[ihit] << " ";
			//cout << endl;
			//for(int ihit=0; ihit< hJae.chan.size(); ++ihit) cout << hJae.chan[ihit] << " ";
			//cout << endl;

			/*cout << "\t Osu: HS, bars " << endl;*/
			//for(int ihit=0; ihit< hOsu.hp.size(); ++ihit) cout << hOsu.hp[ihit] << " ";
			//cout << endl;
			//for(int ihit=0; ihit< hOsu.chan.size(); ++ihit) cout << hOsu.chan[ihit] << " ";
			//cout << endl;

			//if(hOsu.predictedChans == hJae.predictedChans) continue;


			}

			if(hOsu.predictedChans == hJae.predictedChans) ++amtMatched;
			else{
				eventsNotMatched.push_back({hJae.run, hJae.file, hJae.event});
				++amtNotMatched;
			}
			cout << "Osu predicted: " << endl;
			for(unsigned int ihit=0; ihit< hOsu.predictedChans.size(); ++ihit) cout << hOsu.predictedChans[ihit] << " ";
			cout << endl;

			cout << "Jae predicted: " << endl;
			for(unsigned int ihit=0; ihit< hJae.predictedChans.size(); ++ihit) cout << hJae.predictedChans[ihit] << " ";
			cout << endl;

			if(hJae.fit_xz != hOsu.fit_xz){
			hFitxz->Fill(hJae.fit_xz[0] - hOsu.fit_xz[0]);
			hFitxz->Fill(hJae.fit_xz[1] - hOsu.fit_xz[1]);
			cout << "fit_xz: " << endl;
			cout << "\t Jae:" << endl;
				 for(unsigned int iF=0; iF< hJae.fit_xz.size(); ++iF){
				 //	cout << "\t " << hJae.fit_xz[iF] << endl;
				}
			//cout << "\tOsu" << endl;
				for(unsigned int iF=0; iF< hOsu.fit_xz.size(); ++iF){
				   cout << "\t " << hOsu.fit_xz[iF] << endl;
				}
			}

			if(hJae.hs_time != hOsu.hs_time){
				cout << "hs time" << endl;
				cout << "\t Jae: " << hJae.hs_time << endl;
				cout << "\t OSU: " << hOsu.hs_time << endl;
			}

			if(hJae.hp != hOsu.hp){
			cout << "hp: " << endl;
			cout << "\tJae:" << endl;
				 for(unsigned int iF=0; iF< hJae.hp.size(); ++iF){
				 	cout << "\t " << hJae.hp[iF] << endl;
				}
			cout << "\tOsu:" << endl;
				for(unsigned int iF=0; iF< hOsu.hp.size(); ++iF){
				   cout << "\t " << hOsu.hp[iF] << endl;
				}
			}

		}
	}

	cout << "matched " << amtMatched << endl;
	cout << "not matched " << amtNotMatched << endl;

	for(unsigned int i=0; i<eventsNotMatched.size(); ++i){
		cout << "{ ";
		for(unsigned int j=0; j<eventsNotMatched[i].size(); ++j){
			if(j == eventsNotMatched[i].size() -1) cout << eventsNotMatched[i][j] << " ";
			else cout << eventsNotMatched[i][j] << ", ";
		}
		cout << "},  ";

	}

	return;

	for(unsigned int iR=0; iR < eventsJae.size(); ++iR){
		if(eventsJae[iR] == 0 && eventsOSU[iR] == 0) continue;
		cout << "Run: " << iR << " Jae: " << eventsJae[iR] << " OSU: " << eventsOSU[iR] << endl;
	}

	TCanvas * c1 = new TCanvas("c1");
	TCanvas * c2 = new TCanvas("c2");
	c1->cd();
	hFityz->Draw();

	c2->cd();
	hFitxz->Draw();
	return;

	//return;
	int numFitEvents = 0;
	TChain * c = new TChain("t");
	c->Add("/store/HS_Run1370/*.root");
	SetAddressesChainHS(c);
	int entries = c->GetEntries();
	for(int iE=0; iE < entries; ++iE){
		c->GetEntry(iE);
		if((*v_fit_xz).size() == 0 && (*v_fit_yz).size() == 0) continue;
		++numFitEvents;
	}
	cout << "Run 1370: " << numFitEvents << endl;

} // end function
