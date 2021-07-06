#include "treesV19Template.h"

////////////////////////////////////////////////
int run = -1;
int file = -1;
int eventNum = -1;
bool beam_on;
bool triggerablePath = false;
bool passPanelVeto = false;
bool oneHitPerLayer = false;
vector<int> * nBarsActiveInLayer = new vector<int>();
int nSlabsActive = 0;
double maxDeltaT = 0;
double minDeltaT = 0;
vector<int> * firstBars = new vector<int>();
vector<double> * firstBarNPE = new vector<double>();
vector<int> * firstBarLayer = new vector<int>();
vector<double> * firstBarArea = new vector<double>();
vector<double> * firstBarT = new vector<double>();
vector<int> * barLayer = new vector<int>();
vector<double> * sheetNPEs = new vector<double>();
vector<int> * sheetChans = new vector<int>();
vector<int> * slabsHit = new vector<int>();
vector<double> * slabTime = new vector<double>();
vector<int> * bars = new vector<int>();
vector<int> * otherBars = new vector<int>();
////////////////////////////////////////////////

void SetInputTreeAddresses(TChain *& tree){
	tree->SetBranchAddress("triggerablePath", &triggerablePath);
	tree->SetBranchAddress("beam_on", &beam_on);
	tree->SetBranchAddress("passPanelVeto", &passPanelVeto);
	tree->SetBranchAddress("run", &run);
	tree->SetBranchAddress("file", &file);
	tree->SetBranchAddress("eventNum", &eventNum);
	tree->SetBranchAddress("nSlabsActive", &nSlabsActive);
	tree->SetBranchAddress("oneHitPerLayer",&oneHitPerLayer);
	tree->SetBranchAddress("nBarsActiveInLayer",&nBarsActiveInLayer);
	tree->SetBranchAddress("maxDeltaT",&maxDeltaT);
	tree->SetBranchAddress("minDeltaT",&minDeltaT);
	tree->SetBranchAddress("firstBars", &firstBars);
	tree->SetBranchAddress("firstBarNPE", &firstBarNPE);
	tree->SetBranchAddress("firstBarArea",&firstBarArea);
	tree->SetBranchAddress("firstBarLayer",&firstBarLayer);
	tree->SetBranchAddress("firstBarT",&firstBarT);
	tree->SetBranchAddress("barLayer",&barLayer);
	tree->SetBranchAddress("sheetNPEs", &sheetNPEs);
	tree->SetBranchAddress("sheetChans",&sheetChans);
	tree->SetBranchAddress("slabsHit",&slabsHit);
	tree->SetBranchAddress("slabTime",&slabTime);
	tree->SetBranchAddress("bars", &bars);
	tree->SetBranchAddress("otherBars", &otherBars);
}


void analyze(bool onehit=0, bool signal=0, int NLAYERS=3){

	int tCut = 30;

	TString fileKind = signal ? "sim":"data";
	TString tag = onehit ? "oneHitPerLayer":"atLeastOneHitPerLayer";
	TString mode = signal ? "<< SIM MODE >> " : "<< DATA MODE >> ";
	TString layerMode = onehit ? "1 HIT/LAYER" : ">= 1 HIT/LAYER";

	TString inputFile = "data/" + fileKind + "_4layer/outTree_" + msl::tstring(to_string(NLAYERS)) + "layers.root";
	if(signal) inputFile = "data/" + fileKind + "_4layer/outTree_" + msl::tstring(to_string(NLAYERS)) + "layers_sim.root";
	TString outputDir = "data/" + fileKind + "_4layer/alo_plots/";
	if(onehit) outputDir = "data/" + fileKind + "_4layer/o_plots/";
	TString outputFile = outputDir + "analyzePlots_" + msl::tstring(to_string(NLAYERS)) + "layers_" + tag + "_" + fileKind + ".root";
	
	cout << BOLDYELLOW  << mode << endl;
	cout << layerMode << RESET << endl;
	cout << RED << "Output file: " << BOLDRED << outputFile << RESET << endl;

	TChain * inputTree = new TChain("outputTree");
	inputTree->Add(inputFile);
	SetInputTreeAddresses(inputTree);
	int inEntries = inputTree->GetEntries();

	gROOT->SetBatch(kTRUE);
	gStyle->SetOptFit();
	TFile * histFile = new TFile(outputFile, "RECREATE");
	// double binsNPE[10] = {0,0.5,1.5,5,10,20,50,100,1000,10000};

	vector<int> chans = {
		10,29,27,           //L1
		11,30,19,           //L2
		14,26,31            //L3
	};
	vector<double> means = {
		0,0,0,
		0,0,0,
		0,0,0
	};
	vector<int> nhits = {
		0,0,0,
		0,0,0,
		0,0,0
	};

	// npe plots
	TH1D * hMinNPE = new TH1D("hMinNPE", "Min NPE Before Timing Cut", 100, 0, 20);
	TH1D * hMaxMin = new TH1D("hMaxMin", "Max/Min NPE (N-1) - w/ min NPE > 3", 50, 0, 100);
	TH2D * hMaxVsMin = new TH2D("hMaxVsMin","Max NPE v. Min NPE ("+fileKind+");Max NPE;Min NPE", 50, 0, 100, 50, 0, 20);

	TH1D * hMiddleGivenTop = new TH1D("hMiddleGivenTop", "Middle Given Top", 50, 0, 100);
	TH1D * hMiddleGivenBottom = new TH1D("hMiddleGivenBottom", "Middle Given Bottom", 50, 0, 100);

	// plot timing
	TH1D * hTime = new TH1D("hTime", "Bars abs(deltaTime) between all layers;#Delta Time;Count", 160, 0, 100);
	TH1D * hTime_L1L4 = new TH1D("hTime_L1L4", "Bars deltaTime between L1 and L4;#Delta Time(L4-L1);Count", 160, -100, 100);
	TH1D * hTime_L2L3 = new TH1D("hTime_L2L3", "Bars deltaTime between L2 and L3;#Delta Time(L3-L2);Count", 160, -100, 100);
	TH1D * hTime_L1L2 = new TH1D("hTime_L1L2", "Bars deltaTime between L1 and L2;#Delta Time(L2-L1);Count", 160, -100, 100);
	TH1D * hTime_L3L4 = new TH1D("hTime_L3L4", "Bars deltaTime between L3 and L4;#Delta Time(L4-L3);Count", 160, -100, 100);
	TH1D * hTime_L3L4_20 = new TH1D("hTime_L3L4_20", "Ch 20, Mean: ;#Delta Time(L4-L3);Count", 160, -100, 100);
	TH1D * hTime_L3L4_28 = new TH1D("hTime_L3L4_28", "Ch 28, Mean: ;#Delta Time(L4-L3);Count", 160, -100, 100);
	TH1D * hTime_Slab21_L3 = new TH1D("hTime_Slab21_L3","Time Difference Slab 21 - L3;#Delta Time;Count",160,-100,100);
	TH1D * hTime_Slab21_L4 = new TH1D("hTime_Slab21_L4","Time Difference L4 - Slab 21;#Delta Time;Count",160,-100,100);

	// distance from signal plots
	TH2D * dfs_tb13 = new TH2D("dfs_tb13","R v. C Dist From Signal Path (top/bottom L1-3);#Rows;#Cols", 3, 0, 3, 2, 0, 2);
	TH2D * dfs_m13 = new TH2D("dfs_m13","R v. C Dist From Signal Path (middle L1-3);#Rows;#Cols", 2, 0, 2, 2, 0, 2);
	TH2D * dfs_4 = new TH2D("dfs_4","R v. C Dist From Signal Path (L4);#Rows;#Cols", 2, 0, 2, 2, 0, 2);

	TH1D * dfs_r_tb13 = new TH1D("dfs_r_tb13","R Dist From Signal Path (top/bottom L1-3);#Rows;Count",3,0,3);
	TH1D * dfs_r_m13 = new TH1D("dfs_r_m13","R Dist From Signal Path (middle L1-3);#Rows;Count", 3,0,3);
	TH1D * dfs_c_tb13 = new TH1D("dfs_c_tb13","C Dist From Signal Path (top/bottom L1-3);#Cols;Count",2,0,2);
	TH1D * dfs_c_m13 = new TH1D("dfs_c_m13","C Dist From Signal Path (middle L1-3);#Cols;Count", 2, 0, 2);
	
	TH1D * hNHits[4];
	for(int iH = 0; iH < 4; iH++){
		hNHits[iH] = new TH1D("hNHits"+msl::tstring(std::to_string(iH)),"hNHits"+msl::tstring(std::to_string(iH))+";Number of Hits;Count",10,0,10);
	}
	TH1D * hArea[4];
	for(int iH = 0; iH < 4; iH++){
		hArea[iH] = new TH1D("hAreaL"+msl::tstring(std::to_string(iH)),"hArea"+msl::tstring(std::to_string(iH))+";Area;Count",100,0,10000);
	}

	TH1D * hSheets = new TH1D("hSheets", "Sheet NPEs Passing All Selections;NPE;Count", 100, 0, 1000);
	TH1D * hNumSheetHits = new TH1D("hNumSheetHits", "Number of Sheet Hits Passing All Selections;Number;Count", 10, 0, 16);
	TH2D * hSheetNumVsMinNPE = new TH2D("hSheetNumVsMinNPE","Number of Sheets Hit Vs. Min NPE in the Sheets;Num;Min NPE",6,0,6,20,0,100);
	TH2D * hSheetNumVsMaxNPE = new TH2D("hSheetNumVsMaxNPE","Number of Sheets Hit Vs. Max NPE in the Sheets;Num;Max NPE",10,0,10,40,0,400);
	
	TH1D * hSheetChans[10];
	for(int iH = 0; iH < 10; iH++){
		hSheetChans[iH] = new TH1D(msl::tstring(std::to_string(chans[iH])),msl::tstring(std::to_string(chans[iH]))+";NPE;Count",10,0,100);
	}

	
	int topAndMiddle(0), topAndBottom(0), bottomAndMiddle(0), bottomAndTop(0);
	int top(0), bottom(0);
	
	for(int iE = 0; iE < inEntries; iE++){
		inputTree->GetEntry(iE);

		bool passTiming(true), passMaxMin(true), pass3(true);

		if(onehit) if(!oneHitPerLayer) continue;

		if(msl::minElement((*firstBarNPE)) < 3) pass3 = false;
		
		double maxBarNPE = msl::maxElement((*firstBarNPE));
		double minBarNPE = msl::minElement((*firstBarNPE));
		if(maxBarNPE/minBarNPE > 10) passMaxMin = false;
		else passMaxMin = true;
		
		if(fabs(maxDeltaT) > tCut && fabs(fabs(maxDeltaT/tCut)-1) > 0.01) passTiming = false;
		else if(fabs(maxDeltaT/tCut - 1) < 0.01) passTiming = false;
		else passTiming = true;
		
		if(passMaxMin) hMinNPE->Fill(minBarNPE);
		if(passTiming && passPanelVeto) hMaxVsMin->Fill(maxBarNPE, minBarNPE);
		if(passTiming && passPanelVeto) hMaxMin->Fill(maxBarNPE/minBarNPE);

		if(passPanelVeto && passMaxMin && pass3){
	
			// delta t between specific layers
			for(int iTime = 0; iTime < (*firstBarT).size(); ++iTime){
				for(int jTime = 0; jTime < (*firstBarT).size(); ++jTime){
					if(iTime >= jTime) continue;

					if((*firstBarLayer)[iTime] > (*firstBarLayer)[jTime]){
						hTime->Fill((*firstBarT)[iTime] - (*firstBarT)[jTime]);
					}
					if((*firstBarLayer)[iTime] < (*firstBarLayer)[jTime]){
						hTime->Fill((*firstBarT)[jTime] - (*firstBarT)[iTime]);
					}
					
					if(((*firstBarLayer)[iTime]==1 && (*firstBarLayer)[jTime]==4)){
						hTime_L1L4->Fill(((*firstBarT)[jTime] - (*firstBarT)[iTime]));
					}
					if(((*firstBarLayer)[iTime]==4 && (*firstBarLayer)[jTime]==1)){
						hTime_L1L4->Fill(((*firstBarT)[iTime] - (*firstBarT)[jTime]));
					}

					if(((*firstBarLayer)[iTime]==2 && (*firstBarLayer)[jTime]==3)){
						hTime_L2L3->Fill(((*firstBarT)[jTime] - (*firstBarT)[iTime]));
					}
					if(((*firstBarLayer)[iTime]==3 && (*firstBarLayer)[jTime]==2)){
						hTime_L2L3->Fill(((*firstBarT)[iTime] - (*firstBarT)[jTime]));
					}

					if(((*firstBarLayer)[iTime]==4 && (*firstBarLayer)[jTime]==3)){
						hTime_L3L4->Fill(((*firstBarT)[iTime] - (*firstBarT)[jTime]));
						if((*firstBars)[iTime] == 20) hTime_L3L4_20->Fill(((*firstBarT)[iTime] - (*firstBarT)[jTime]));
						if((*firstBars)[iTime] == 28) hTime_L3L4_28->Fill(((*firstBarT)[iTime] - (*firstBarT)[jTime]));
					}
					if(((*firstBarLayer)[iTime]==3 && (*firstBarLayer)[jTime]==4)){
						hTime_L3L4->Fill(((*firstBarT)[jTime] - (*firstBarT)[iTime]));
						if((*firstBars)[jTime] == 20) hTime_L3L4_20->Fill(((*firstBarT)[jTime] - (*firstBarT)[iTime]));
						if((*firstBars)[jTime] == 28) hTime_L3L4_28->Fill(((*firstBarT)[jTime] - (*firstBarT)[iTime]));
					}

					if(((*firstBarLayer)[iTime]==2 && (*firstBarLayer)[jTime]==1)){
						hTime_L1L2->Fill(((*firstBarT)[iTime] - (*firstBarT)[jTime]));
					}
					if(((*firstBarLayer)[iTime]==1 && (*firstBarLayer)[jTime]==2)){
						hTime_L1L2->Fill(((*firstBarT)[jTime] - (*firstBarT)[iTime]));
					}
				}
			}

			for(int iSlab = 0; iSlab < (*slabsHit).size(); iSlab++){
				if((*slabsHit)[iSlab] == 21){
					for(int iBar = 0; iBar < (*firstBars).size(); iBar++){
						if((*firstBarLayer)[iBar]==3){
							hTime_Slab21_L3->Fill((*slabTime)[iSlab]-(*firstBarT)[iBar]);
						}
						if((*firstBarLayer)[iBar]==4){
							hTime_Slab21_L4->Fill((*firstBarT)[iBar]-(*slabTime)[iSlab]);
						}
					}
				}
			}
			
		} // end timing


		if(passPanelVeto && passMaxMin && passTiming){

			bool printed = false;
			for(auto b: (*otherBars)){
				vector<int> ob = getInfo(b, true);

				for(int iBar=0; iBar < (*firstBars).size(); ++iBar){
					int i = (*firstBars)[iBar];

					vector<int> fb = getInfo(i, true);

					/*
					info[0] = type;
					info[1] = layer;
					info[2] = column;
					info[3] = row;
					*/

					int rd = abs(ob[3] - fb[3]);
					int cd = abs(ob[2] - fb[2]);

					// same layer
					if(ob[1] != fb[1]) continue;

					if(fb[3] == 0){ // bottom
						bottom++;
						if(ob[3] == 1){
							bottomAndMiddle++;
							hMiddleGivenBottom->Fill((*firstBarNPE)[iBar]);
						}
						if(ob[3] == 2) bottomAndTop++;
					}
					if(fb[3] == 2){ // top
						top++;
						if(ob[3] == 0) topAndBottom++;
						if(ob[3] == 1){
							topAndMiddle++;
							hMiddleGivenTop->Fill((*firstBarNPE)[iBar]);
						}
					}

					// cout << ob[1] << ": " << ob[3] << "(" << b << ") " << fb[3];
					// cout << "(" << i << ")" << endl << endl;

					if(ob[1] != 4){
						if(fb[3] == 1){
							dfs_m13->Fill(rd, cd);
							dfs_c_m13->Fill(cd);
							dfs_r_m13->Fill(rd);
						}
						else {
							dfs_tb13->Fill(rd, cd);
							dfs_c_tb13->Fill(cd);
							dfs_r_tb13->Fill(rd);
						}
					}
					else{
						cd = 0;
						rd = b!=i;
						dfs_4->Fill(rd, cd);
					}


				}
			}
			
			
			if(pass3){
				for(int iL = 0; iL < 4; iL++){
					hNHits[iL]->Fill((*nBarsActiveInLayer)[iL]);
				}
				for(int iHit = 0; iHit < (*firstBarLayer).size(); iHit++){
					hArea[(*firstBarLayer)[iHit]-1]->Fill((*firstBarArea)[iHit]);
				}

				//pass panel veto = 0 sheets active
				hNumSheetHits->Fill(0);
			}
		}
		

		if(passTiming && passMaxMin){
			vector<int> sheetsHit = {};
		
			//cumulative
			for(auto s:(*sheetNPEs)) hSheets->Fill(s);
		
			//by channel
			for(int iHit = 0; iHit < (*sheetChans).size(); iHit++){

				for(int iCh = 0; iCh < chans.size(); iCh++){
					if(chans[iCh] == (*sheetChans)[iHit]){
						hSheetChans[iCh]->Fill((*sheetNPEs)[iHit]);
						means[iCh] += (*sheetNPEs)[iHit];
						nhits[iCh]++;
					}
				}

				// //event list
				// if((*sheetNPEs)[iHit] > 50){
				// 	fOut << run << ",\t" << file << ",\t" << eventNum << endl;
				// }

				//number of hits in the sheets
				sheetsHit = msl::set<int>((*sheetChans));
				
				double nSheetsHit = sheetsHit.size();
				hNumSheetHits->Fill(nSheetsHit);
				
				double maxSheetNPE = msl::maxElement((*sheetNPEs));
				hSheetNumVsMaxNPE->Fill(nSheetsHit,maxSheetNPE);

				double minSheetNPE = msl::minElement((*sheetNPEs));
				hSheetNumVsMinNPE->Fill(nSheetsHit,minSheetNPE);
			}
		}

		
	} // end entry loop
	
	// return;


	// now plot histograms
	vector<TH1*> sheetVec = vector<TH1*>(begin(hSheetChans), end(hSheetChans));
	vector<TH1*> nHitVec = std::vector<TH1*>(std::begin(hNHits), std::end(hNHits));
	double offsetx3 = 0.1;
	double offsety3 = 0.05;
	auto * legendNHits = new TLegend(0.76-offsetx3, 0.75-offsety3, 0.9+offsetx3, 0.9+offsety3);
	for(int iL = 0; iL < 4; iL++){
		legendNHits->AddEntry(hNHits[iL],"Layer " + msl::tstring(std::to_string(iL+1)) + " , Mean " + msl::tstring(std::to_string(hNHits[iL]->GetMean())) ,"l");
	}

	double offsetx2 = 0.1;
	double offsety2 = 0.05;
	auto * legendSheets = new TLegend(0.76-offsetx2, 0.75-offsety2, 0.9+offsetx2, 0.9+offsety2);
	for(int iCh = 0; iCh < chans.size(); iCh++){
		legendSheets->AddEntry(hSheetChans[iCh],"Chan " + msl::tstring(std::to_string(chans[iCh])) + " Mean " +  msl::tstring(std::to_string(means[iCh]/nhits[iCh])) ,"l");
	}

	plot1DHistograms({hMiddleGivenBottom, hMiddleGivenTop}, outputDir+"hMiddle"+tag+".png", "l");
	plot1DHistograms(nHitVec, outputDir+"hNHits_"+tag+".png", "ylm", legendNHits);
	plot1DHistograms({hMinNPE}, outputDir+"hMinNPE_"+tag+".png", "y");
	plot1DHistograms({hMaxMin}, outputDir+"hMaxMin_"+tag+".png", "");
	plot2DHistograms({hMaxVsMin}, outputDir+"hMaxVsMin_"+tag+".png", "");

	plot1DHistograms({hTime}, outputDir+"diffTime_allSRs"+tag+".png", "y");
	plot1DHistograms({hTime_L1L4}, outputDir+"diffTime_L1L4"+tag+".png", "y");
	plot1DHistograms({hTime_L2L3}, outputDir+"diffTime_L2L3"+tag+".png", "y");
	plot1DHistograms({hTime_L1L2}, outputDir+"diffTime_L1L2"+tag+".png", "y");
	plot1DHistograms({hTime_L3L4}, outputDir+"diffTime_L3L4"+tag+".png", "y");
	plot1DHistograms({hTime_L3L4_20, hTime_L3L4_28}, outputDir+"diffTime_L3L4_separate"+tag+".png", "lmy");
	plot1DHistograms({hSheets}, outputDir+"hSheets"+tag+".png", "xy");
	plot1DHistograms({hTime_Slab21_L4}, outputDir+"diffTime_S21_L4"+tag+".png","y");
	plot1DHistograms({hTime_Slab21_L3}, outputDir+"diffTime_S21_L3"+tag+".png","y");
	for(int iL = 0; iL < 4; iL++){
		plot1DHistograms({hArea[iL]},outputDir+"hAreasL"+msl::tstring(to_string(iL+1))+"_"+tag+".png", "y");
		hArea[iL]->Write();
	}
	plot1DHistograms(vector<TH1*>(begin(hSheetChans), end(hSheetChans)), outputDir+"hSheetsByChan"+tag+".pdf", "ly", legendSheets);
	plot1DHistograms({hNumSheetHits}, outputDir+"hNumSheetHits"+tag+".png");
	plot2DHistograms({hSheetNumVsMaxNPE}, outputDir+"hSheetNumVsMaxNPE"+tag+".png", "z");
	plot2DHistograms({hSheetNumVsMinNPE}, outputDir+"hSheetNumVsMinNPE"+tag+".png", "z");
	plot2DHistograms({dfs_m13}, outputDir+"dfs_m13.png");
	plot2DHistograms({dfs_tb13}, outputDir+"dfs_tb13.png");
	plot1DHistograms({dfs_r_m13}, outputDir+"dfs_r_m13.png"); 
	plot1DHistograms({dfs_c_m13}, outputDir+"dfs_c_m13.png");
	plot1DHistograms({dfs_r_tb13}, outputDir+"dfs_r_tb13.png");
	plot1DHistograms({dfs_c_tb13}, outputDir+"dfs_c_tb13.png");
	plot2DHistograms({dfs_4}, outputDir+"dfs_4.png");

	for(auto h: nHitVec) h->Write();
	hMiddleGivenBottom->Write();
	hMiddleGivenTop->Write();
	hMinNPE->Write();
	hMaxMin->Write();
	hTime->Write();
	hTime_L1L2->Write();
	hTime_L1L4->Write();
	hTime_L2L3->Write();
	hTime_L3L4->Write();
	hTime_L3L4_20->Write();
	hTime_L3L4_28->Write();
	hSheets->Write();
	hTime_Slab21_L4->Write();
	hTime_Slab21_L3->Write();
	for(auto h: sheetVec) h->Write();
	hNumSheetHits->Write();
	hSheetNumVsMaxNPE->Write();
	hSheetNumVsMinNPE->Write();
	dfs_m13->Write();
	dfs_tb13->Write();
	dfs_r_m13->Write();
	dfs_c_m13->Write();
	dfs_r_tb13->Write();
	dfs_c_tb13->Write();
	dfs_4->Write();

	auto keyList = histFile->GetListOfKeys();
	vector<string> keyListNames;
	for(auto k: (*keyList))keyListNames.push_back(k->GetName());

	// cout << BOLDGREEN << "Plots written to file: \n" << MAGENTA;
	// for(auto k: keyListNames) cout << k << " \n";
	cout << RESET << endl;

	histFile->Close();

	// output probabilities
	if(!onehit){
		cout << BOLDCYAN << "P(m|t): " << topAndMiddle*1.0/top << "+-" << sqrt(topAndMiddle)/top << endl;
		cout << BOLDCYAN << "P(b|t): " << topAndBottom*1.0/top << "+-" << sqrt(topAndBottom)*1.0/top <<endl;
		cout << BOLDCYAN << "P(m|b): " << bottomAndMiddle*1.0/bottom << "+-" << sqrt(bottomAndMiddle)/bottom <<endl;
		cout << BOLDCYAN << "P(t|b): " << bottomAndTop*1.0/bottom << "+-" << sqrt(bottomAndTop)/bottom << RESET << endl;
	}

} // end analyze