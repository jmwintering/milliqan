#include "treesV16Template.h"
#include <time.h>
#include <TMultiGraph.h>

void dtimeV2beam(TString runDirectory, string runN);

void dtimeV2beam(TString runDirectory, string runN){
	int runNumber = toint(runN);

	TFile* outputFile = new TFile("/data/dTime/timing" + toTstring(tostr(runNumber)) + "_beam.root", "RECREATE");

	TChain* chain = new TChain("t");


	TH1D *hDt_beam = new TH1D("hDt_beam", "hDt_beam; hs_time - event_time_fromTDC [s]; Events", 100, -10, 1);
	TH1D *hDt_nobeam = new TH1D("hDt_nobeam", "hDt_nobeam; hs_time - event_time_fromTDC [s]; Events", 100, -10, 1);

  	TH1D *hSync_beam = new TH1D("hSync_beam","hSync_beam; Days since Jan.1, 2018; Unsynched Events",366+365,0,366+365);
  	TH1D *hSync_nobeam = new TH1D("hSync_nobeam","hSync_nobeam; Days since Jan.1, 2018; Unsynched Events",366+365,0,366+365);
	TH2D* h2d_beam = new TH2D("h2d_beam", "h2d_beam", 50, 0, 5, 25, 0, 25);
	TH2D* h2d_nobeam = new TH2D("h2d_nobeam", "h2d_nobeam", 50, 0, 5, 25, 0, 25);

	double current_time = 0;
	double previous_time = 0;
	int count = 0;
	Double_t sync_beam[1000];
	Double_t sync_nobeam[1000];
	Double_t hour[1000];

	int nUnsync_beam = 0;
	int nSync_beam = 0;
	int nUnsync_nobeam = 0;
	int nSync_nobeam = 0;

	Int_t underscore = runDirectory.Last('_');
	Int_t pos = runDirectory.First('_');
	Int_t len = runDirectory.Length();

	TString runTitle = runDirectory(pos+1, 7);

	int nFiles = numberFiles("/store/user/llavezzo/" + runDirectory);
	chain->Reset();

	//nFiles = 4; // testing

	for(int  iFileNum = 1; iFileNum <= nFiles; ++iFileNum){
		chain->Add("/store/user/llavezzo/" + runDirectory + Form("/UX5MilliQan_" + runTitle + ".%d_*_v16.root", iFileNum));
	}

	cout << runDirectory << endl;
    int entries = chain->GetEntries();
	cout << "Added " << entries << " entries" << endl;

  SetAddressesChainHS(chain);

	for(int i = 0; i < entries; i++){
		if(i% 100000 == 0) cout << "got to " << i << endl;
		chain->GetEntry(i);
		time_t timeepoch    = static_cast<int>(event_time_fromTDC);
		time_t timeHSepoch = static_cast<int>(hs_time);
		struct tm *date = gmtime(&timeepoch);

		//synched events
		if(hs_time - event_time_fromTDC > -2000000){

			if(beam){
				hDt_beam->Fill(hs_time - event_time_fromTDC);
				h2d_beam->Fill(abs(hs_time - event_time_fromTDC), date->tm_hour);
			}
			if(!beam){
				hDt_nobeam->Fill(hs_time - event_time_fromTDC);
				h2d_nobeam->Fill(abs(hs_time - event_time_fromTDC), date->tm_hour);
			}

		}

		//unsynched events
		if(hs_time == -1){
			if(beam) hSync_beam->Fill(date->tm_yday);
			if(!beam) hSync_nobeam->Fill(date->tm_yday);

			if(beam)nUnsync_beam++;
			if(!beam)nUnsync_nobeam++;
		}
		else{
			if(beam)nSync_beam++;
			if(!beam)nSync_nobeam++;
		}

		if(i == 0) previous_time = date->tm_yday;
		current_time = date->tm_yday;

		if(current_time != previous_time || i==0) {


			if(beam) sync_beam[count] = nUnsync_beam*1.0/(nUnsync_beam+nSync_beam);
			if(!beam) sync_nobeam[count] = nUnsync_nobeam*1.0/(nUnsync_nobeam+nSync_nobeam);
			hour[count] = date->tm_yday;

			previous_time = date->tm_yday;

			if(i!=0){
				nUnsync_beam = 0;
				nUnsync_nobeam = 0;
				nSync_beam = 0;
				nSync_nobeam = 0;
			}

			count++;
		}


        ClearVectorsHS();
	}


	TGraph *gSync_beam = new TGraph(count, hour, sync_beam);
	TGraph *gSync_nobeam = new TGraph(count, hour, sync_nobeam);
	gSync_beam->SetName("gSync_beam");
	gSync_beam->SetTitle("gSync_beam; Days since Jan.1, 2018; Unsynch events / Total Events");
	gSync_nobeam->SetName("gSync_nobeam");
	gSync_nobeam->SetTitle("gSync_nobeam; Days since Jan.1, 2018; Unsynch events / Total Events");

	hDt_beam->Write();
	hDt_nobeam->Write();
	hSync_beam->Write();
	hSync_nobeam->Write();
	gSync_beam->Write();
	gSync_nobeam->Write();
	h2d_beam->Write();
	h2d_nobeam->Write();

	outputFile->Close();
    cout << "wrote histos to " << outputFile->GetName() << endl;
}
