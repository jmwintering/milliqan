/* 
*  Method: creates a skim of the data, relevant variables about the events
*  Analyze: runs ABCD method on the data, timing and path selections
*
* Cuts used between abcdMethodUCSBCuts.cpp and abcdMethodUCSBCuts.cpp:
* delta t 30ns, absolute value of difference between each two layers < 30 (DONE)
* Low → medium NPE (??)
* Single pulse per channel (DONE)
* single channel hit per layer (DONE)
* Pointing path in three layers (same column/row in each layer) (DONE)
* No activity in rest of detector (DONE)
* same runs
*
* Meant to be the same exact cuts and runs of Matthew Citron's ABCD method
*
*
*/

vector<TString> injectionOuts = {
   "p3", "p2", "p1", "p14", "p07", "p05", "p03", "p02", "p01", "p005"
  };

  vector<int> injectionOuts_nEvents = {
	5393,
	35582,
	97756,
	76719,
  	103417,
	106922,
	107544,
	109193,
  	110443,
  	110617
  };


void abcdAnalyze_injected() {

	TString selection = "noSlab";

	gSystem->Exec("rm /data/abcd_data/cutEfficienciesAnalyze_"+selection+".txt");
  	gSystem->Exec("touch /data/abcd_data/cutEfficienciesAnalyze_"+selection+".txt");

	for(int iFile = 0; iFile < injectionOuts.size(); iFile++){
		cout << endl << injectionOuts[iFile] << endl;
		TString	path = "/data/abcd_data/methodFiles/abcdInjectedPrepared_";

		TChain * tree = new TChain("outputTree");
		tree->Add(path + injectionOuts[iFile] + "_"+selection+"hit.root");
		
		double totalNumberEvents = injectionOuts_nEvents[iFile]; //( 127464 + 48432.7)/ 3600; //3.34e6 / 3600; //  //0; // //  //

		int entries = tree->GetEntries();
		cout << "Added " << tree->GetEntries() << " entries" <<  endl;

		// deltaTime
		// |
		// |  D        C
		// |
		// |
		// |  A        B
		// _________________ path
		//   good      bad
		//
		// A/B ~ D/C
		// estimate = B * D / C

		int timeCut = 15;

		TH1D::SetDefaultSumw2();
		gStyle->SetOptStat(0);
		gStyle->SetOptTitle(0);

		TCanvas * c4 = new TCanvas("c4");

		double binsNPE[10] = {0,0.5,1.5,5,10,20,50,100,1000,10000};

		TH2D * hStraight = new TH2D("hStraight", "hStraight", 9, binsNPE, 385, -120, 120);
		TH2D * hBent = new TH2D("hBent", "hBent", 9, binsNPE, 385, -120, 120);

		int nStraight = tree->Draw("maxDeltaT:minNPE >> hStraight", "isStraightPath && maxSlabDeltaT < 20", "goff");
		tree->Draw("maxDeltaT:minNPE >> hBent", "!isStraightPath && sameDigi && maxSlabDeltaT < 20", "goff");

		TH1D * hAp = new TH1D();
		TH1D * hBp = new TH1D();
		TH1D * hCp = new TH1D();
		TH1D * hBp2 = new TH1D();
		TH1D * hCp2 = new TH1D();
		TH1D * hDp = new TH1D();

		hAp = hStraight->ProjectionX("A", hStraight->GetYaxis()->FindBin(-timeCut),hStraight->GetYaxis()->FindBin(timeCut)-1 );
		hBp = hStraight->ProjectionX("B", hStraight->GetYaxis()->FindBin(-100),hStraight->GetYaxis()->FindBin(-timeCut)-1 );
		hBp2 = hStraight->ProjectionX("B2", hStraight->GetYaxis()->FindBin(timeCut),hStraight->GetYaxis()->FindBin(100) );
		hBp->Add(hBp2);

		hDp = hBent->ProjectionX("D", hBent->GetYaxis()->FindBin(-timeCut),hBent->GetYaxis()->FindBin(timeCut)-1 );
		hCp = hBent->ProjectionX("C", hBent->GetYaxis()->FindBin(-100),hBent->GetYaxis()->FindBin(-timeCut)-1 );
		hCp2 = hBent->ProjectionX("C2", hBent->GetYaxis()->FindBin(timeCut),hBent->GetYaxis()->FindBin(100) );
		hCp->Add(hCp2);

		TH1D* hAPred = (TH1D*)hBp->Clone("APred");
		hAPred->Multiply(hDp);
		hAPred->Divide(hCp);

		hAp->Scale(1.0/totalNumberEvents);
		hBp->Scale(1.0/totalNumberEvents);
		hCp->Scale(1.0/totalNumberEvents);
		hDp->Scale(1.0/totalNumberEvents);
		hAPred->Scale(1.0/totalNumberEvents);

		if(hAp->GetEntries() == 0) cout << "A empty" << endl;
		ofstream fout;
		fout.open("/data/abcd_data/cutEfficienciesAnalyze_"+selection+".txt", ios::app);
		fout << endl << injectionOuts[iFile] << endl;
		fout << "straight: " << nStraight << endl;
		fout << "signal: " << hAp->GetEntries() << endl;
    	fout.close();
		
		if(hBp->GetEntries() == 0) cout << "B empty" << endl;
		if(hCp->GetEntries() == 0) cout << "C empty" << endl;
		if(hDp->GetEntries() == 0) cout << "D empty" << endl;
		if(hAPred->GetEntries() == 0) cout << "A pred empty" << endl;

		TFile * fOutput1 = new TFile("/data/abcd_data/methodFiles/projectedABCDplots_15ns_"+injectionOuts[iFile]+".root", "RECREATE");
		hAp->Write();
		hBp->Write();
		hCp->Write();
		hDp->Write();
		hAPred->Write();
		fOutput1->Close();

		TCanvas *c1 = new TCanvas("c1");
		c1->cd();
		c1->SetLogy();
		c1->SetLogx();
		hAp->SetTitle(injectionOuts[iFile]);
		hAPred->SetTitle(injectionOuts[iFile]);
		hAp->GetYaxis()->SetTitle("Efficiency [N events/total events] ("+selection+"/sheet hits)");
		hAp->GetXaxis()->SetTitle("minNPE");
		hAPred->SetLineColor(kRed);
		hAp->SetLineColor(kBlack);
		hAp->Draw();
		hAPred->Draw("same");
		c1->SaveAs("/data/abcd_data/closurePlots/efficiency_"+injectionOuts[iFile]+"_"+selection+"hit.pdf");
	}

}
