#include "../treesV16Template.h"
#include <time.h>



void dtime(){

	TFile* outputFile = new TFile("timing.root", "RECREATE");

	TChain* chain = new TChain("t");

	vector<TString> directories = {
		"HS_Run1000",
		"HS_Run1003",
		"HS_Run1012",
		"HS_Run1013",
		"HS_Run1018",
		"HS_Run1022",
		"HS_Run1026",
		"HS_Run1040",
		"HS_Run1059",
		"HS_Run1060",
		"HS_Run1062",
		"HS_Run1063",
		"HS_Run1065",
		"HS_Run1068",
		"HS_Run1069",
		"HS_Run1070",
		"HS_Run1071",
		"HS_Run1072",
		"HS_Run1073",
		"HS_Run1074",
		"HS_Run1076",
		"HS_Run1077",
	};

	int numRuns = directories.size();

	TH1D *hDt[numRuns];
   	for(Int_t i=0;i<numRuns;i++) {
      TString name = "hDt"+tostr(i);
      cout << name << endl;
      cout << name << endl;
      hDt[i] = new TH1D(name,name,100,-10,1);
   	}



   	TH1D *hSync[numRuns];
	char name2[20];
   	char title2[100];
   	for(Int_t i=0;i<numRuns;i++) {
      sprintf(name2,"hSync%d",i);
      sprintf(title2,"hSync%d",i);
      hSync[i] = new TH1D(name2,title2,1000,1.532e9,1.535e9);
   	}

	TH2D* h2d = new TH2D("h2d", "h2d", 50, 0, 5, 25, 0, 25);

	TCanvas* c1= new TCanvas("c1");
	TCanvas* c2 = new TCanvas("c2");
	TCanvas* c3 = new TCanvas("c3");
	TCanvas* c4 = new TCanvas("c4");

	double current_time = 0;
	double previous_time = 0;
	vector<int> count(numRuns, 0);
	Double_t sync[numRuns][100];
	Double_t hour[numRuns][100];

	int nUnsync = 0;
	int nSync = 0;
	int current_run = 0;
	int previous_run = 0;
	int run = 0;

	for(int iRun = 0; iRun < numRuns; iRun++){

		TString dirT = directories[iRun];
		string dirS = directories[iRun].Data();;

    	Int_t underscore = dirT.Last('_');
    	Int_t pos = dirT.First('_');
    	Int_t len = dirT.Length();

    	TString runDescription = dirT(underscore+1,len-underscore);

    	TString runTitle = dirT(pos+1, 7);

		int nFiles = numberFiles("/data/HSdata/HS_trees_v16/" + directories[iRun]);
		chain->Reset();

		for(int  iFileNum = 1; iFileNum < nFiles; ++iFileNum){
    		chain->Add("/store/" + dirT + Form("/UX5MilliQan_" + runTitle + ".%d_" + runDescription + "_v16.root", iFileNum));
  		}

  		SetAddressesChainHS(chain);

		for(int i = 0; i < chain->GetEntries(); i++){
			chain->GetEntry(i);
			time_t timeepoch    = static_cast<int>(event_time_fromTDC);
			time_t timeHSepoch = static_cast<int>(hs_time);
			struct tm *date = gmtime(&timeepoch);

			if(i == 0) previous_run = runNum;

			//synched events
			if(hs_time - event_time_fromTDC > -2000000){

				hDt[iRun]->Fill(hs_time - event_time_fromTDC);
				h2d->Fill(abs(hs_time - event_time_fromTDC), date->tm_hour);


				//f << Form("%d%02d%02d%02d\n", date->tm_year+1900, date->tm_mon+1, date->tm_mday,   date->tm_hour) << endl;

			}

			//unsynched events
			if(hs_time == -1){
				hSync[iRun]->Fill(event_time_fromTDC);
			}


			if(hs_time == -1){
					nUnsync++;
				}
			else{
				nSync++;
			}

			if(i == 0) previous_time = date->tm_hour;
			current_time = date->tm_hour;

			if(current_time != previous_time) {
				count[iRun]++;

				sync[iRun][count[iRun]] = nUnsync*1.0/(nUnsync+nSync);
				hour[iRun][count[iRun]] = date->tm_hour + date->tm_mday*24;

				previous_time = date->tm_hour;

				nUnsync = 0;
				nSync = 0;
			}



		}

	}


    TGraph *gSync[numRuns];
    TMultiGraph *mg = new TMultiGraph();
    for (int i=0; i<numRuns; i++) {
       gSync[i] = new TGraph(count[i], hour[i], sync[i]);
       gSync[i]->SetLineColor(i);
       mg->Add(gSync[i]);
    }

	c1->cd();
	for(int i = 0; i < numRuns; i++){
		if(i == 0) hDt[i]->Draw();
		else {
			hDt[i]->Draw("same");
			hDt[i]->SetLineColor(i);
		}
		hDt[i]->GetYaxis()->SetRange(0, 50000);
		hDt[i]->Write();
	}
	c1->Write();

	c2->cd();
	for(int i = 0; i < numRuns; i++){
		if(i == 0) hSync[i]->Draw();
		else {
			hSync[i]->Draw("same");
			hSync[i]->SetLineColor(i);
		}
		hSync[i]->Write();
	}
	c2->Write();

	c3->cd();
	mg->Draw("AL");
	mg->GetXaxis()->SetTitle("Hours since beginning of run");
	mg->GetYaxis()->SetTitle("N Unsync Events / Total Events");
	c3->Write();


	c4->cd();
	h2d->Draw("colz");
	c4->Write();

	outputFile->Close();
}
