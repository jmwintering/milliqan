#include "treesV19Template.h"

////////////////////////////////////////////////
bool triggerablePath = false;
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
int nSlabsActive = 0;
vector<double> * barNPEs = new vector<double>();
vector<double> * deltaTimeAbs = new vector<double>();
////////////////////////////////////////////////

void SetInputTreeAddresses(TTree * tree){
    tree->SetBranchAddress("triggerablePath", &triggerablePath);    
    tree->SetBranchAddress("beam_on", &beam_on);
    tree->SetBranchAddress("minNPE", &minNPE);
    tree->SetBranchAddress("maxNPE", &maxNPE);
    tree->SetBranchAddress("minSlabNPE", &minSlabNPE);
    tree->SetBranchAddress("maxSlabNPE", &maxSlabNPE);
    tree->SetBranchAddress("minBarNPE", & minBarNPE);
    tree->SetBranchAddress("maxBarNPE", & maxBarNPE);
    tree->SetBranchAddress("maxDeltaT", & maxDeltaT);
    tree->SetBranchAddress("minDeltaT", &minDeltaT);
    tree->SetBranchAddress("maxSlabDeltaT", &maxSlabDeltaT);
    tree->SetBranchAddress("minSlabDeltaT", & minSlabDeltaT);
    tree->SetBranchAddress("sameDigi", &sameDigi);
    tree->SetBranchAddress("run",&run);
    tree->SetBranchAddress("file",&file);
    tree->SetBranchAddress("eventNum", &eventNum);
    tree->SetBranchAddress("nSlabsActive",&nSlabsActive);
    tree->SetBranchAddress("barNPEs", &barNPEs);
    tree->SetBranchAddress("deltaTimeAbs",&deltaTimeAbs);
    
}


void plotInvertedPanelPlots(TString inputFile="fourLayer_outputTree.root", TString tag=""){

    gSystem->Exec("mkdir outputPlots/compareInverted");
    gROOT->SetBatch(kTRUE);
    gStyle->SetOptStat(0);

    tag = "compareInverted";

    TString plotsDir = "outputPlots/compareInverted/";
    TFile * inFile = new TFile("outputTrees/"+inputFile, "read");
    TFile * inFileInv = new TFile("outputTrees/fourLayer_outputTree_invertedPanel.root", "read");

    TTree * inputTree = (TTree*)inFile->Get("outputTree");
    TTree * inputTreeInverted = (TTree*)inFileInv->Get("outputTree");

    SetInputTreeAddresses(inputTree);
    SetInputTreeAddresses(inputTreeInverted);

    int inEntries = inputTree->GetEntries();
    int inEntriesInv = inputTreeInverted->GetEntries();

    TFile * histFile = new TFile(plotsDir+"abcd4layerAnalysisHistograms_"+tag+".root", "recreate");

    // double binsNPE[10] = {0,0.5,1.5,5,10,20,50,100,1000,10000};

     // Write means out
    ofstream fMeans;
    fMeans.open("npeMeans_"+tag+".txt");

    // plot nPE 
	for(auto timingSel: {15,20,30}){

		TH1D * hAfterMin = new TH1D(Form("hAfter_%dns",timingSel), Form("NPE dist after Max/min cut (timing cut: %dns);Min Bar NPE;Count", timingSel), 100, 0, 1000);
		TH1D * hAfterMax = new TH1D(Form("hAfter_%dns",timingSel), Form("NPE dist after Max/min cut (timing cut: %dns);Max Bar NPE; Count", timingSel), 100, 0, 1000);

        TH1D * hAfterMinInv = new TH1D(Form("hAfter_%dnsInv",timingSel), Form("NPE dist after Max/min cut (timing cut: %dns);Min Bar NPE;Count", timingSel), 100, 0, 1000);
		TH1D * hAfterMaxInv = new TH1D(Form("hAfter_%dnsInv",timingSel), Form("NPE dist after Max/min cut (timing cut: %dns);Max Bar NPE; Count", timingSel), 100, 0, 1000);
        
        for(int iE =0; iE < inEntries; ++iE){
            inputTree->GetEntry(iE);

            if(fabs(maxDeltaT) > timingSel && fabs(fabs(maxDeltaT/timingSel)-1) > 0.01) continue;
            if(fabs(maxDeltaT/timingSel - 1) < 0.01) continue;
            if(maxBarNPE/minBarNPE > 10) continue; 
            hAfterMin->Fill(minBarNPE);
            hAfterMax->Fill(maxBarNPE);
        }

        for(int iE =0; iE < inEntriesInv; ++iE){
            inputTreeInverted->GetEntry(iE);

            if(fabs(maxDeltaT) > timingSel && fabs(fabs(maxDeltaT/timingSel)-1) > 0.01) continue;
            if(fabs(maxDeltaT/timingSel - 1) < 0.01) continue;
            if(maxBarNPE/minBarNPE > 10) continue; 
            hAfterMinInv->Fill(minBarNPE);
            hAfterMaxInv->Fill(maxBarNPE);
        }

        // store mins
        fMeans << Form("hAfterMinInv: %d: %f",timingSel, hAfterMinInv->GetMean(1)) << endl;
        fMeans << Form("hAfterMin: %d: %f",timingSel, hAfterMin->GetMean(1)) << endl;
        fMeans << Form("hAfterMaxInv: %d: %f",timingSel, hAfterMaxInv->GetMean(1)) << endl;
        fMeans << Form("hAfterMax: %d: %f",timingSel, hAfterMax->GetMean(1)) << endl;

    
        // plot min
        double offsetx = 0.1;
        double offsety = 0.05;

        auto legendMin = TLegend(0.76-offsetx, 0.75-offsety, 0.9+offsetx, 0.9+offsety);
        legendMin.AddEntry(hAfterMinInv, "Inverted panel veto", "l");
        legendMin.AddEntry(hAfterMin, "Panel veto", "l");

        auto legendMax = TLegend(0.76-offsetx, 0.75-offsety, 0.9+offsetx, 0.9+offsety);
        legendMax.AddEntry(hAfterMaxInv, "Inverted panel veto", "l");
        legendMax.AddEntry(hAfterMax, "Panel veto", "l");

        legendMin.SetBorderSize(0);
        legendMax.SetBorderSize(0);

        legendMin.SetFillStyle(0);
        legendMax.SetFillStyle(0);

        TCanvas * cMin = new TCanvas(Form("cMin_%d", timingSel));
        TCanvas * cMax = new TCanvas(Form("cMax_%d", timingSel));

        cMin->SetCanvasSize(2100, 1500);
        cMax->SetCanvasSize(2100, 1500);

        cMin->cd();
        cMin->SetLogx();
        cMin->SetLogy();

        vector<TH1*> minHists = findHigherHistogram(hAfterMinInv, hAfterMin);
        minHists[0]->Draw();
        minHists[1]->Draw("same");

        hAfterMinInv->SetLineColor(kBlue);
        hAfterMin->SetLineColor(kRed);

        hAfterMinInv->SetLineWidth(6);
        hAfterMin->SetLineWidth(4);

        legendMin.Draw();
        cMin->SaveAs(plotsDir+Form("minNPE_after_%dns"+tag+".png",timingSel));

        cMax->cd();
        cMax->SetLogx();
        cMax->SetLogy();

        vector<TH1*> maxHists = findHigherHistogram(hAfterMaxInv, hAfterMax);
        maxHists[0]->Draw();
        maxHists[1]->Draw("same");

        hAfterMaxInv->SetLineColor(kBlue);
        hAfterMax->SetLineColor(kRed);

        hAfterMaxInv->SetLineWidth(6);
        hAfterMax->SetLineWidth(4);

        legendMax.Draw();
        cMax->SaveAs(plotsDir+Form("maxNPE_after_%dns"+tag+".png",timingSel));

        hAfterMinInv->Write();
        hAfterMin->Write();
        hAfterMaxInv->Write();
        hAfterMax->Write();

        cMin->Write();
        cMax->Write();
    }  
    histFile->Close(); 
    fMeans.close();
}