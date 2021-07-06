#include "treesV16Template.h"

void submitRuns();

void hsEfficiencyB(string runFile="hsRunName.txt");

# ifndef __CINT__  // the following code will be invisible for the interpreter
int main()
{
     gROOT->ProcessLine(".L /home/bmanley/hsEfficiencyB.cpp++");
     hsEfficiencyB();
}
# endif

void hsEfficiencyB(string runFile){
    ifstream inF;
    inF.open(runFile);
    if(!inF.is_open()){
        cout << "Error: file with run directory could not be opened" << endl;
        return;
    }
    string inputFileName = "";
    inF >> inputFileName;
    cout << "input: " << inputFileName<< endl;
    inF.close();
    string runNum = inputFileName;


    int runNumber = toint(runNum);
    TChain* chain = new TChain("t");
    TFile * fInput = new TFile("/store/user/llavezzo/HS_Run1340/UX5MilliQan_Run1340.992_Physics_v16.root", "READ");
    TTree * input = (TTree*)fInput->Get("t");
    TFile * fOutput = new TFile("hsEff" + toTstring(runNum) + ".root", "RECREATE");
    TTree * outputTree = input->CloneTree();
    outputTree->Reset();
    SetAddressesTreeHS(outputTree);
    fInput->Close();

    chain->Add("/store/user/llavezzo/HS_Run"+ toTstring(tostr(runNumber))+ "/*.root");

    SetAddressesChainHS(chain);

    double minCut = 100;

    int n = chain->GetEntries();
    cout << "Added run " << runNumber << endl;
    cout << "Added " << n << " entries" << endl;

    for(int iEvent = 0; iEvent < n; iEvent++){
          chain->GetEntry(iEvent);

          vector<int> barsHit;
          vector<bool> layersHit = {false, false, false, false};

          //select straight events
          for(unsigned int iPulse = 0; iPulse < (*v_ipulse).size(); iPulse++){
                if((*v_ipulse)[iPulse] != 0) continue;
                if((*v_nPE)[iPulse] < minCut) continue;
                barsHit.push_back((*v_chan)[iPulse]);
                layersHit[(*v_layer)[iPulse]] = true;
          }

          if(barsHit.size() < 3) continue;
          if(!layersHit.at(3) || !layersHit.at(1) || !layersHit.at(2)) continue;

          barsHit.clear();

          outputTree->Fill();
    }

    outputTree->Write();
    fOutput->Close();

}
