#include "treesV19Template.h"
#define line  __LINE__


void abcdAnalyze(TString workDir, TString plotDir, TString closurePlotLabel, TString inputString, TString plotOutputString) {

	TChain * tree = new TChain("outputTree");
	tree->Add(inputString);
	
	// from matt: {'beam': 3984710.2445580107, 'noBeam': 3751188.332908044}
	double totalTimeOff = 3751188.332908044; 
	totalTimeOff /= 3600; // change to evts/hr

	int entries = tree->GetEntries();


	/*
		deltaTime
			|
	bad		|  D        C
			|
			|
	good	|  A        B
			_________________ path
			  good      bad
	
	A/B ~ D/C
	estimate = B * D / C 
	*/

	int timeCut = 15;
	double nPEcut = 1;

	TH1D::SetDefaultSumw2();
	gStyle->SetOptStat(0);
	gStyle->SetOptTitle(0);

	double binsNPE[10] = {0,0.5,1.5,5,10,20,50,100,1000,10000};

	TH2D * hStraight = new TH2D("hStraight", "hStraight", 9, binsNPE, 385, 0, 120);
	TH2D * hBent = new TH2D("hBent", "hBent", 9, binsNPE, 385, -120, 120);

	// val plots
	TCut a_event = "isStraightPath";
	TH1D * hDeltaTime = new TH1D("hDeltaTime", "hDeltaTime", 385, -120, 120);
	
	int nStraight = tree->Draw("maxDeltaT:minBarNPE >> hStraight", "isStraightPath", "goff");
	tree->Draw("maxDeltaT:minBarNPE >> hBent", "!isStraightPath && sameDigi", "goff");
	tree->Draw("maxDeltaT >> hDeltaTime", a_event, "goff");

	
	/* Plots as a function of minNPE ////////////////////
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

	hAp->Scale(1.0/totalTimeOff);
	hBp->Scale(1.0/totalTimeOff);
	hCp->Scale(1.0/totalTimeOff);
	hDp->Scale(1.0/totalTimeOff);
	hAPred->Scale(1.0/totalTimeOff);

	if(hAp->GetEntries() == 0){ cout << "A empty" << endl; }
	cout << "straight: " << nStraight << endl;
	cout << "signal: " << hAp->GetEntries() << endl;

	if(hBp->GetEntries() == 0) cout << "B empty" << endl;
	if(hCp->GetEntries() == 0) cout << "C empty" << endl;
	if(hDp->GetEntries() == 0) cout << "D empty" << endl;
	if(hAPred->GetEntries() == 0) cout << "A pred empty" << endl;
	if(hBp->GetEntries() == 0 && hCp->GetEntries() == 0 && hDp->GetEntries() == 0 && hAPred->GetEntries() == 0 && hAp->GetEntries() == 0){
		cerr << "Error: no entries in any plot" << endl;
		return;
	}

	// scaled histograms
	TFile * fOutputScaled = new TFile(workDir+plotOutputString+"_scaled.root", "recreate");
	fOutputScaled->cd();
	hAp->Write();
	hBp->Write();
	hCp->Write();
	hDp->Write();
	hAPred->Write();
	hStraight->Write();
	hDeltaTime->Write();
	fOutputScaled->Close();

	TCanvas *c1 = new TCanvas("cBackgroundRate");
	c1->cd();
	c1->SetLogy();
	c1->SetLogx();
	hAp->SetTitle("observed (black) v. predicted (red)");
	hAPred->SetTitle("observed (black) v. predicted (red)");
	hAp->GetYaxis()->SetTitle("Total Background");
	hAp->GetXaxis()->SetTitle("Max #Delta t (in bars)[ns]");
	hAPred->SetLineColor(kRed);
	hAp->SetLineColor(kBlue);
	
	hAPred->Draw();
	hAp->Draw();

	auto legend = new TLegend(0.9,0.7,0.6,0.9);
	legend->AddEntry(hAp,"Observation","l");
	legend->AddEntry(hAPred,"ABCD Prediction","l");
	legend->Draw();
	c1->SaveAs(plotDir+closurePlotLabel+"_scaled.pdf");

	// Unscaled histograms
	TH1D * hAp_u = (TH1D*)hAp->Clone();
	TH1D * hBp_u = (TH1D*)hBp->Clone();
	TH1D * hCp_u = (TH1D*)hCp->Clone();
	TH1D * hDp_u = (TH1D*)hDp->Clone();
	TH1D * hAPred_u = (TH1D*)hAPred->Clone();

	hAp_u->Scale(totalTimeOff);
	hBp_u->Scale(totalTimeOff);
	hCp_u->Scale(totalTimeOff);
	hDp_u->Scale(totalTimeOff);
	hAPred_u->Scale(totalTimeOff);

	TFile * fOutputUnscaled = new TFile(workDir+plotOutputString+"_unscaled.root", "recreate");
	fOutputUnscaled->cd();
	hAp_u->Write();
	hBp_u->Write();
	hCp_u->Write();
	hDp_u->Write();
	hAPred_u->Write();
	fOutputUnscaled->Close();

	TCanvas *c2 = new TCanvas("c2");
	c2->cd();
	c2->SetLogy();
	c2->SetLogx();
	hAp_u->GetYaxis()->SetTitle("Background rate [evts/hr]");
	hAPred_u->Draw();
	hAp_u->Draw();
	
	auto legend_u = new TLegend(0.15 ,0.82, 0.35 ,0.87);
	legend_u->AddEntry(hAp_u,"Observation","l");
	legend_u->AddEntry(hAPred_u,"ABCD Prediction","l");
	legend_u->Draw();
	c2->SaveAs(plotDir+closurePlotLabel+"_unscaled.pdf");
	*/


	// Plots as a function of max delta t
	TH1D * hAp_t = new TH1D();
	hAp_t = hStraight->ProjectionY("A", hStraight->GetXaxis()->FindBin(nPEcut), hStraight->GetNbinsX() );
	TH1 * hAp_cumulative_t = hAp_t->GetCumulative();

	hAp_cumulative_t->Scale(1.0/totalTimeOff);
	hAp_t->Scale(1.0/totalTimeOff);
	hAp_cumulative_t->SetTitle("Cumulative (scaled)");
	
	// scaled histograms
	TFile * fOutputScaled_t = new TFile(workDir+plotOutputString+"_scaled_t.root", "recreate");
	fOutputScaled_t->cd();
	hAp_t->Write();
	hAp_cumulative_t->Write();
	hStraight->Write();
	hDeltaTime->Write();
	fOutputScaled_t->Close();

	TCanvas *c1 = new TCanvas("cBackgroundRate");
	c1->cd();
	hAp_t->SetTitle("Observed Background Rate");
	hAp_t->GetYaxis()->SetTitle("Total Background [evts/hr]");
	hAp_t->GetXaxis()->SetTitle("Max #Delta t (in bars)[ns]");
	hAp_t->SetLineColor(kBlue);
	hAp_t->Draw();

	auto legend = new TLegend(0.15 ,0.82, 0.35 ,0.87);
	legend->AddEntry(hAp_t,"Observation","l");
	legend->Draw();
	c1->SaveAs(plotDir+closurePlotLabel+"_scaled_t.pdf");

	TCanvas * c2 = new TCanvas("cCumulative");
	c2->cd();
	hAp_cumulative_t->GetYaxis()->SetTitle("Cumulative background rate [evts/hr]");
	hAp_cumulative_t->GetXaxis()->SetTitle("Max #Delta t (in bars)[ns]");
	hAp_cumulative_t->SetLineColor(kBlue);
	hAp_cumulative_t->Draw();
	auto legend_c = new TLegend(0.15 ,0.82, 0.35 ,0.87);
	legend_c->AddEntry(hAp_cumulative_t,"Observation","l");
	legend_c->Draw();
	c2->SaveAs(plotDir+closurePlotLabel+"_scaled_cumulative_t.pdf");

	// Unscaled histograms
	TH1D * hAp_u_t = (TH1D*)hAp_t->Clone();
	TH1D * hAp_u_cumulative_t = (TH1D*)hAp_cumulative_t->Clone();

	hAp_u_t->Scale(totalTimeOff);
	hAp_u_cumulative_t->Scale(totalTimeOff);
	hAp_u_cumulative_t->SetTitle("Cumulative background (unscaled)");

	TFile * fOutputUnscaled_t = new TFile(workDir+plotOutputString+"_unscaled_t.root", "recreate");
	fOutputUnscaled_t->cd();
	hAp_u_t->Write();
	hAp_u_cumulative_t->Write();
	fOutputUnscaled_t->Close();

	TCanvas * c3 = new TCanvas("cBackgroundUnscaled");
	c3->cd();
	hAp_u_t->GetYaxis()->SetTitle("Background count");
	hAp_u_t->Draw();
	auto legend_u = new TLegend(0.15 ,0.82, 0.35 ,0.87);
	legend_u->AddEntry(hAp_u_t,"Observation","l");
	legend_u->Draw();
	c3->SaveAs(plotDir+closurePlotLabel+"_unscaled_t.pdf");

	TCanvas * c4 = new TCanvas("cCumUnscaled");
	c4->cd();
	hAp_u_cumulative_t->GetYaxis()->SetTitle("Cumulative background count");
	hAp_u_cumulative_t->Draw();
	auto legend_u_c = new TLegend(0.15 ,0.82, 0.35 ,0.87);
	legend_u_c->AddEntry(hAp_u_cumulative_t,"Observation","l");
	legend_u_c->Draw();
	c4->SaveAs(plotDir+closurePlotLabel+"_unscaled_cumulative_t.pdf");

	// Plot background rate increase in % as a function of delta T
	TH1D * hPercent_increase = (TH1D*)hAp_u_cumulative_t->Clone();

	int bin15 = hPercent_increase->GetXaxis()->FindBin(15);
	int count15 =  hPercent_increase->GetBinContent(bin15);

	for(int ibin = 1; ibin < hPercent_increase->GetNbinsX()+1; ++ibin){
		double percent_increase = 100.0*((1.0*hPercent_increase->GetBinContent(ibin) - count15) / count15); 
		hPercent_increase->SetBinContent(ibin, percent_increase);
	}

	TCanvas * c5 = new TCanvas("cPercentIncrease");
	c5->cd();
	hPercent_increase->SetLineColor(kBlack);
	hPercent_increase->GetYaxis()->SetTitle("Percent increase from cut at 15 ns");
	hPercent_increase->Draw();
	c5->SaveAs(plotDir+"per_increase_t.pdf");
	
}

vector<double> nPE_corrs(32, 1);


void abcdMeasure(
	bool singlePulseSel=0, bool slabHitSel=0, 
	TString workDir="methodFiles/", 
	TString inputDir= //ADD FILe, 
	TString plotsDir="closurePlots/",
	TString valPlotLabel="validationPlots.root",
	bool signal = 0
){ 
	
	gSystem->Exec("mkdir -p "+workDir);
	gSystem->Exec("mkdir -p "+plotsDir);
  	gSystem->Exec("mkdir -p "+plotsDir+"/slabPlots/");
	gSystem->Exec("mkdir -p progressOutputs/");
	gSystem->Exec("mkdir -p eventComp/");
	
	if(singlePulseSel){
		cout << "sel: no other pulse" << endl;
	}
	else{
		cout << "sel: no pre pulse" << endl;
	}
	if(slabHitSel){
		cout << "sel: at least 1 slab hit" << endl;
	}
	else{
		cout << "sel: no slab hit" << endl;
	}

	bool eventComp = false;
	bool debugEvent = false;
	bool signalInjection = signal;
	if(debugEvent){ eventComp = false; }
	if(eventComp){ cout << "EVENT COMP MODE" << endl; }
	if(debugEvent){ cout << "DEBUG EVENT MODE" << endl; }
	if(signalInjection){ cout << "SIG INJECT MODE" << endl; }

	auto eventCompEntry = -1;
	int eventComp_run = 1000;
	int eventComp_file = 12;
	int eventComp_event = 507;

	TChain * chain = new TChain("t");
	chain->Add(//ADD file);
	int entries = SetAddresses(chain);

	cout << "Added " << entries << " entries to abcdMeasure" << endl << endl;
	TTree * outputTree = new TTree("outputTree", "outputTree");

	bool isStraightPath = false;
	bool beam_on;
	double minNPE = 1e9;
	double maxNPE = -1;
	double minSlabNPE = 1e9;
	double maxSlabNPE = -1;
	double minBarNPE = 1e9;
	double maxBarNPE = -1;
	double maxDeltaT = -1;
	double minDeltaT = 1e9;
	double maxSlabDeltaT = -1;
	double minSlabDeltaT = 1e9;
	bool sameDigi  = false;
	int run = -1;
	int file = -1;
	int eventNum = -1;

	outputTree->Branch("isStraightPath", &isStraightPath);
	outputTree->Branch("beam_on", &beam_on);
	outputTree->Branch("minNPE", &minNPE);
	outputTree->Branch("maxNPE", &maxNPE);
	outputTree->Branch("maxSlabNPE", &maxSlabNPE);
	outputTree->Branch("minSlabNPE", &minSlabNPE);
	outputTree->Branch("minBarNPE", &minBarNPE);
	outputTree->Branch("maxBarNPE", &maxBarNPE);
	outputTree->Branch("maxDeltaT", &maxDeltaT);
	outputTree->Branch("minDeltaT", &minDeltaT);
	outputTree->Branch("maxSlabDeltaT", &maxSlabDeltaT);
	outputTree->Branch("minSlabDeltaT", &minSlabDeltaT);
	outputTree->Branch("sameDigi", &sameDigi);
	outputTree->Branch("run", &run);
	outputTree->Branch("file", &file);
	outputTree->Branch("eventNum", &eventNum);

	// VALIDATION PLOTS
	TH1D * hSheet = new TH1D("sheet nPE", "sheet nPE", 200, 0 ,1000);
	TH1D * hSlab = new TH1D("slab nPE", "slab nPE", 200, 0 ,1000);
	TH1D * hPathType = new TH1D("path type", "path type", 3, 0, 3);
	TH1D * hRow0 = new TH1D("row 0 NPE", "row 0 NPE", 200,0,1000);
	TH1D * hRow1 = new TH1D("row 1 NPE", "row 1 NPE", 200,0,1000);
	TH1D * hRow2 = new TH1D("row 2 NPE", "row 2 NPE", 200,0,1000);
	TH1D * hMaxMin = new TH1D("max/min", "max/min", 200, 0, 100);
	TH1D * hSlabNPE = new TH1D("hSlabNPE", "hSlabNPE", 30, 0, 1000);
	TH1D * hSlabBarDeltaTSameLayer = new TH1D("hSlabBarDeltaTSameLayer", "hSlabBarDeltaTSameLayer", 50, 0, 120);
	TH1D * hSlabBarDeltaTDiffLayer = new TH1D("hSlabBarDeltaTDiffLayer", "hSlabBarDeltaTDiffLayer", 50, 0, 120);
	
	int nTubeBins = 160;
	int nTubeLow(-50), nTubeHigh(50);

	TH1D * h878 = new TH1D("h878", "h878", nTubeBins, nTubeLow, nTubeHigh);
	TH1D * h878_ET = new TH1D("h878_ET", "h878_ET", nTubeBins, nTubeLow, nTubeHigh);
	TH1D * h878_7725 = new TH1D("h878_7725", "h878_7725", nTubeBins, nTubeLow, nTubeHigh);
	TH1D * hET = new TH1D("hET", "hET", nTubeBins, nTubeLow, nTubeHigh);
	TH1D * hET_7725 = new TH1D("hET_7725", "hET_7725", nTubeBins, nTubeLow, nTubeHigh);
	TH1D * hAllTubes = new TH1D("hAllTubes", "hAllTubes", nTubeBins, nTubeLow, nTubeHigh);

	bool printedEvent = false; 
	string aString = "";
	string bString = "";
	string cString = "";
	string dString = "";

	for(int i = 0; i < entries; i++) {
		// if(i > 10000){ continue;}

		if(!debugEvent){
			displayProgress(i, entries);
			chain->GetEntry(i);
		}
		else{
			if(!printedEvent){ chain->GetEntry(eventCompEntry); }
			printedEvent = true;
		}

		bool flag = false;
		if(runNum == eventComp_run && fileNum == eventComp_file && event == eventComp_event && eventComp){ flag = debugEvent; }

		// quiet sideband/boards synched
		bool quietRMS = true;
		for(int iPulse = 0; iPulse < (*v_sideband_RMS).size(); iPulse++) {
			if(iPulse==15) continue;
			if((*v_sideband_RMS)[iPulse] > 1.3){ quietRMS = false; }
		}

		if(!quietRMS){ continue;}
		if(!signalInjection){
			if((*v_groupTDC_b0)[0] != (*v_groupTDC_b1)[0]){ continue;}
			if(beam){ continue; } 
		}
	

		beam_on = beam;
		run = runNum;
		file = fileNum;
		eventNum = event;
		vector<int> channel = {};
		vector<int> type = {};
		vector<double> time = {};
		vector<double> slabTime = {};
		vector<int> slabChan = {};
		vector<int> row = {};
		vector<int> col = {};
		vector<int> barRow = {};
		vector<int> barCol = {};
		vector<int> barLayer = {};
		vector<double> nPE = {};
		vector<int> bars = {};
		vector<int> layer = {};

		// by channel nPE 
		vector<vector<double>> pulseList = {};
		for(int ich=0;ich<32;++ich){ pulseList.push_back({-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}); }

		// re initialize desired variables
		minNPE = 1e9;
		maxNPE = -1;
		minSlabNPE = 1e9;
		maxSlabNPE = -1;
		maxBarNPE = -1;
		minBarNPE = 1e9;
		minSlabNPE = 1e9;
		isStraightPath = false;
		minDeltaT = 1e9;
		maxDeltaT = -1;
		minSlabDeltaT = 1e9;
		maxSlabDeltaT = -1;
		sameDigi = false;

		bool singlePulse = true;
		bool slabVeto = false;
		bool failPulseTimeSelection = false;
		bool failfirstPulseMax = false;
		int slabsActive = 0;
		int anySlabActive = 0;
		int sheetsActive = 0;
		vector<int> nBarsActiveInLayer(3, 0);

		for(int j = 0; j < (*v_ipulse).size(); j++) {

			if((*v_chan)[j] == 15){ continue; } // timing card

			// store event info in simpler vectors
			channel.push_back((*v_chan)[j]);
			nPE.push_back(  (*v_nPE)[j] * nPE_corrs[(*v_chan)[j]] );
			vector<int> info = checkType((*v_chan)[j]);
			type.push_back(info[0]);
			row.push_back(info[3]);
			col.push_back(info[2]);
			layer.push_back(info[1]);

			if(info[0] != 2){ pulseList[(*v_chan)[j]][(*v_ipulse)[j]] = (*v_nPE)[j]*nPE_corrs[(*v_chan)[j]];}
			
			if((*v_ipulse)[j] != 0){ singlePulse = false;}

			if(info[0]==kSheet){
				if((*v_nPE)[j]*nPE_corrs[(*v_chan)[j]]  > 0) sheetsActive++;
				hSheet->Fill((*v_nPE)[j]*nPE_corrs[(*v_chan)[j]]);
			};
			
			if(info[0] == kSlab){
				
					hSlab->Fill((*v_nPE)[j]*nPE_corrs[(*v_chan)[j]]);
					if((*v_ipulse)[j]==0){
						slabChan.push_back((*v_chan)[j]);
						slabTime.push_back((*v_time_module_calibrated)[j]);
						// slabTime.push_back(correctTime((*v_time_module_calibrated)[j], (*v_area)[j]));
					}
					anySlabActive++;

					if(((*v_chan)[j] == 18 || (*v_chan)[j] == 20 || (*v_chan)[j] == 21) && (*v_nPE)[j]*nPE_corrs[(*v_chan)[j]]  < 300) slabsActive++; 
					else if((*v_chan)[j] == 28 && (*v_nPE)[j]*nPE_corrs[(*v_chan)[j]]  < 200) slabsActive++;
					if(((*v_chan)[j] == 18 || (*v_chan)[j] == 20 || (*v_chan)[j] == 21) && (*v_nPE)[j]*nPE_corrs[(*v_chan)[j]]  > 300) slabVeto = true; 
					else if((*v_chan)[j] == 28 && (*v_nPE)[j]*nPE_corrs[(*v_chan)[j]]  > 200) slabVeto = true;
			}

			if(info[0]== kBar){
				if((*v_ipulse)[j] == 0){
					nBarsActiveInLayer[info[1]]++;
					barLayer.push_back(info[1]);
					time.push_back(correctTime((*v_time_module_calibrated)[j], (*v_area)[j], tubeSpecies((*v_chan)[j]), signalInjection));
					barRow.push_back(info[3]);
					barCol.push_back(info[2]);
					bars.push_back((*v_chan)[j]);
				}
			}


			if((*v_nPE)[j]*nPE_corrs[(*v_chan)[j]] < minNPE) minNPE = (*v_nPE)[j]*nPE_corrs[(*v_chan)[j]];
			if((*v_nPE)[j]*nPE_corrs[(*v_chan)[j]] > maxNPE) maxNPE = (*v_nPE)[j]*nPE_corrs[(*v_chan)[j]];
			if(info[0]== kSlab && (*v_nPE)[j]*nPE_corrs[(*v_chan)[j]] < minSlabNPE) minSlabNPE = (*v_nPE)[j]*nPE_corrs[(*v_chan)[j]];
			if(info[0]== kSlab && (*v_nPE)[j]*nPE_corrs[(*v_chan)[j]] > maxSlabNPE) maxSlabNPE = (*v_nPE)[j]*nPE_corrs[(*v_chan)[j]];
			if(info[0]== kBar && (*v_nPE)[j]*nPE_corrs[(*v_chan)[j]] < minBarNPE && (*v_ipulse)[j] == 0) minBarNPE = (*v_nPE)[j]*nPE_corrs[(*v_chan)[j]];
			if(info[0]== kBar && (*v_nPE)[j]*nPE_corrs[(*v_chan)[j]] > maxBarNPE && (*v_ipulse)[j] == 0) maxBarNPE = (*v_nPE)[j]*nPE_corrs[(*v_chan)[j]];
		} // end ipulse loop
	

		if(minElement(time)<300 || maxElement(time)>440) failPulseTimeSelection = true;
		if(slabTime.size() > 0){
			if(minElement(slabTime)<300 || maxElement(slabTime)>440){ failPulseTimeSelection = true;}
		}

		if(bars.size() < 3){ continue; }
		if(nBarsActiveInLayer.size() < 3){ continue; }
		if((nBarsActiveInLayer[0] != 1 || nBarsActiveInLayer[1] != 1 || nBarsActiveInLayer[2] != 1)) continue;
		
		int row_var = abs(barRow[0] - barRow[1]) + abs(barRow[1] - barRow[2]) + abs(barRow[0] - barRow[2]);
		int col_var = abs(barCol[0] - barCol[1]) + abs(barCol[1] - barCol[2]) + abs(barCol[0] - barCol[2]);
	
		for(int ichan=0;ichan<32;++ichan){
			if(pulseList[ichan][0] < 0){ continue;}
			if(pulseList[ichan][0] < maxElement(pulseList[ichan])) failfirstPulseMax = true;
		}

		if(row_var + col_var == 0) isStraightPath = true;
		else isStraightPath = false;
		if(bars[0]/16  == bars[1] /16 &&  bars[2]/16 == bars[0]/16) sameDigi = true;
		if(barRow.size() > 0){
			if(isStraightPath && barRow[0] == 0) {hPathType->Fill(0); hRow0->Fill(minBarNPE);}
			else if(isStraightPath && barRow[0] == 1) {hPathType->Fill(1);  hRow1->Fill(minBarNPE);}
			else if(isStraightPath && barRow[0] == 2) {hPathType->Fill(2);   hRow2->Fill(minBarNPE);}
		}

		// find max and min times
		maxDeltaT = -1e99;
		minDeltaT = 1e99;
		minDeltaT = findTimeExtrema(time, false, barLayer)[0];
		maxDeltaT = abs(findTimeExtrema(time, false, barLayer)[1]);

		if(slabTime.size() >= 2){
			minSlabDeltaT = findTimeExtrema(slabTime, true)[0];
			maxSlabDeltaT = findTimeExtrema(slabTime, true)[1];
		}

		bool aSel =  isStraightPath && (abs(maxDeltaT) < 15 && !(abs(abs(maxDeltaT/15) - 1) < 0.01));
		bool bSel =  !isStraightPath && sameDigi && (abs(maxDeltaT) < 15 || abs(abs(maxDeltaT/15) - 1) < 0.01);
		bool cSel =  !isStraightPath && sameDigi && !(abs(maxDeltaT) < 15 || abs(abs(maxDeltaT/15) - 1) < 0.01);
		bool dSel =  isStraightPath && !(abs(maxDeltaT) < 15 || abs(abs(maxDeltaT/15) - 1) < 0.01);

		if(aSel){
			for(int k=0; k < (*v_ipulse).size(); ++k) if((*v_type)[k] == kSlab) hSlabNPE->Fill((*v_nPE)[k]*nPE_corrs[(*v_chan)[k]]);

			for(int t1=0; t1 < slabTime.size(); ++t1){
				for(int t2=0; t2 < time.size(); ++t2){
					//layer 0: slabs 18
					//layer 1: slabs 20, 28
					//layer 2: slabs 28, 21

					if(barLayer[t2] == 1 && slabChan[t1] == 18){
						hSlabBarDeltaTSameLayer->Fill(abs(slabTime[t1]-time[t2]));
					}
					if(barLayer[t2] == 2 && (slabChan[t1] == 20 || slabChan[t1] == 28) ){
						hSlabBarDeltaTSameLayer->Fill(abs(slabTime[t1]-time[t2]));
					}
					if(barLayer[t2] == 3 && (slabChan[t1] == 21 || slabChan[t1] == 28) ){
						hSlabBarDeltaTSameLayer->Fill(abs(slabTime[t1]-time[t2])); 
					}

					if(barLayer[t2] == 1 && slabChan[t1] != 18){
						hSlabBarDeltaTDiffLayer->Fill(abs(slabTime[t1]-time[t2]));
					}
					if(barLayer[t2] == 2 && (slabChan[t1] != 20 && slabChan[t1] != 28)){
						hSlabBarDeltaTDiffLayer->Fill(abs(slabTime[t1]-time[t2]));
					}
					if(barLayer[t2] == 3 && (slabChan[t1] != 21 && slabChan[t1] != 28)){
						hSlabBarDeltaTDiffLayer->Fill(abs(slabTime[t1]-time[t2])); 
					}

				} // end time loop
			} // end slabTime loop
		} // end aSel if
		hMaxMin->Fill(maxNPE/minNPE);

		// CUTS
		if(!signalInjection){ if(slabVeto){continue;} }
		if(failPulseTimeSelection) continue;
		if(failfirstPulseMax) continue;
		if(sheetsActive>0) continue;
		if(singlePulseSel) if(!singlePulse) continue;
		if(!signalInjection){
			if(slabHitSel) if(slabsActive<1) continue;
			if(!slabHitSel) if(anySlabActive>0) continue;
		}

		// Event validation
		if(flag && debugEvent){
			cout << "isStraight: " << isStraightPath << endl;
			cout << "sameDigi: " << sameDigi << endl; 
			cout << "max delta t: " << maxDeltaT << endl;
			// cout << "time vector: " << printVector(time)<< endl;
			// cout << "barLayer vector: " << printVector(barLayer) << endl;
		}

		
		if(isStraightPath && signalInjection){

			// fill tube delta t plots
			vector<double> tube878_deltas = {};
			vector<double> tubeET_deltas = {};
			vector<double> tube878_ET_deltas = {};
			vector<double> tubeET_7725_deltas = {};
			vector<double> tube878_7725_deltas = {};


			for(int firstPulse=0; firstPulse < (*v_ipulse).size(); ++firstPulse){
				if((*v_type)[firstPulse] != 0 ){ continue;}
				if((*v_ipulse)[firstPulse] != 0){ continue; }

				for(int secondPulse = firstPulse; secondPulse < (*v_ipulse).size(); ++secondPulse){
					if((*v_type)[secondPulse] != 0 ){ continue;}
					if((*v_ipulse)[secondPulse] != 0){ continue; }

					if(firstPulse == secondPulse){ continue; }

					int tube1 = tubeSpecies((*v_chan)[firstPulse]);
					int tube2 = tubeSpecies((*v_chan)[secondPulse]);
					if(tube1 < 0 || tube2 < 0){ continue; }

					double firstPulseT = correctTime((*v_time_module_calibrated)[firstPulse],(*v_area)[firstPulse], tube1, signalInjection);
					double secondPulseT = correctTime((*v_time_module_calibrated)[secondPulse],(*v_area)[secondPulse], tube2, signalInjection);
					double deltaT_tubes = firstPulseT - secondPulseT;

					if(tube1 == 0 && tube2 == 0){ 
						tube878_deltas.push_back(deltaT_tubes);
						h878->Fill(deltaT_tubes);
					}
					else if(tube1 == 1 && tube2 == 1){ 
						tubeET_deltas.push_back(deltaT_tubes);
						hET->Fill(deltaT_tubes);
					}
					else if((tube1 == 0 && tube2 == 1) || (tube1 == 1 && tube2 == 0)){ 
						tube878_ET_deltas.push_back(deltaT_tubes);
						h878_ET->Fill(deltaT_tubes);
					}
					else if((tube1 == 0 && tube2 == 2) || (tube1 == 2 && tube2 == 0)){ 
						tube878_7725_deltas.push_back(deltaT_tubes);
						h878_7725->Fill(deltaT_tubes);
					}
					else if((tube1 == 2 && tube2 == 1) || (tube1 == 1 && tube2 == 2)){ 
						tubeET_7725_deltas.push_back(deltaT_tubes);
						hET_7725->Fill(deltaT_tubes);
					}

				}
			}

			// plot max delta times
			h878->Fill(maxElement(tube878_deltas));
			h878_ET->Fill(maxElement(tube878_ET_deltas));
			hET->Fill(maxElement(tubeET_deltas));
			h878_7725->Fill(maxElement(tube878_7725_deltas));
			hET_7725->Fill(maxElement(tubeET_7725_deltas));

			vector<double> maxTubes = {maxElement(tube878_ET_deltas),maxElement(tubeET_deltas),maxElement(tube878_7725_deltas), maxElement(tubeET_7725_deltas), maxElement(tube878_deltas) };

			double maxDeltaT_tube = maxElement(maxTubes);
			hAllTubes->Fill(maxDeltaT_tube);
		}
			
		
	
		if(debugEvent){
			if(aSel) cout << "A event" << endl;
			else if(bSel) cout << "B event" << endl;
			else if(cSel) cout << "C event" << endl;
			else if(dSel) cout << "D event" << endl;
		}

		outputTree->Fill();
	} // end event loop

	if(!debugEvent){
		cout << endl << "Output tree has " << outputTree->GetEntries() << " entries." << endl;
		if(outputTree->GetEntries() == 0){
			cerr << "Error: output tree has no entries" << endl;

			return;
		}
	}

	// write prepared root tree out
	TString modeSel = "";
	if(singlePulseSel){
		modeSel += "_noOtherPulse_";
	}
	else{
		modeSel += "_noPrePulse_";
	}
	if(slabHitSel){
		modeSel += "_slabHit";
	}
	else{
		modeSel += "_noSlabHit";
	}

	TString outputString = workDir+"abcdPrepared_allPhysics2018"+modeSel+".root";
	TFile * fOutput = new TFile(outputString, "recreate");
	outputTree->Write();
	fOutput->Close();
	delete fOutput;
	cout << "Wrote measure output to: " << outputString << endl;

	TString closureString = "";
	if(singlePulseSel){ closureString += "noOtherPulse_";}
	else{ closureString += "noPrePulse_";}
	if(slabHitSel){closureString += "atLeastOneSlab";}
	else{closureString += "noSlabs";}

	TString plotOutputString = "plotsProjected_15ns_"+closureString;

	if(!debugEvent){ 
		abcdAnalyze(workDir, plotsDir, closureString, outputString, plotOutputString);
	} 
	

	if(eventComp){
		string suff = "";
		if(singlePulseSel) suff += "nop_";
		else suff += "npp_";
		if(slabHitSel) suff += "s";
		else suff += "ns";

		ofstream fout;
		fout.open("eventComp/blA_"+suff+".txt");
		fout << aString << endl;
		fout.close();
		fout.open("eventComp/blB_"+suff+".txt");
		fout << bString << endl;
		fout.close();
		fout.open("eventComp/blC_"+suff+".txt");
		fout << cString << endl;
		fout.close();
		fout.open("eventComp/blD_"+suff+".txt");
		fout << dString << endl;
		fout.close();

		cout << "Wrote event comp to eventComp/bl*"+suff+".txt" << endl;
	}

	if(eventComp){ return; }
	// Write validation plots to a file
	TFile * fVal = new TFile(workDir+"valplots"+valPlotLabel+".root", "recreate");
	fVal->cd();
	hSheet->Write();
	hSlab->Write();
	hPathType->Write();
	hRow0->Write();
	hRow1->Write();
	hRow2->Write();
	hMaxMin->Write();
	hSlabNPE->Write();
	hSlabBarDeltaTSameLayer->Write();
	hSlabBarDeltaTDiffLayer->Write();
	if(signalInjection){
		h878->Write();
		h878_ET->Write();
		h878_7725->Write();
		hET->Write();
		hET_7725->Write();
		hAllTubes->Write();
		fVal->Close();
	}
	delete fVal;

	cout << endl;
} // end abcdMeasure