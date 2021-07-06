#include "treesV16Template.h"

void hsEfficiency(string runNum);

# ifndef __CINT__  // the following code will be invisible for the interpreter
int main(int narg, char * argv[])
{
     gROOT->ProcessLine(".L hsEfficiency.cpp++");
     hsEfficiency(argv[1]);
}
# endif

void hsEfficiency(string runNum){
    int runNumber = toint(runNum);
    TChain* chain = new TChain("t");
    TFile * fInput = new TFile("/store/user/wintering/HS_Run1340/UX5MilliQan_Run1340.992_Physics_v16.root", "READ");
    TTree * input = (TTree*)fInput->Get("t");
    TFile * fOutput = new TFile(toTstring(runNum) + ".root", "RECREATE");
    TTree * outputTree = input->CloneTree();
    outputTree->Reset();
    SetAddressesTree(outputTree);

    chain->Add("/store/user/wintering/HS_Run"+ toTstring(tostr(runNumber))+ "/*.root");
    cout << "Added run " << runNumber << endl;

    SetAddressesChainHS(chain);

    double minCut = 500;

    int n = chain->GetEntries();
    cout << "Added " << n << " entries" << endl;
    //ProgressBar progressBar(n, 50);

    for(int iEvent = 0; iEvent < n; iEvent++){
          //++progressBar;
          //progressBar.display();

          chain->GetEntry(iEvent);

          //FIXME: require cleaner hits
          //if((*v_hs).size() < 8) continue;

          vector<int> barsHit;
          vector<int> barsHitRow;
          vector<int> barsHitCol;
          vector<bool> layersHit = {false, false, false, false};

          //select straight events
          for(unsigned int iPulse = 0; iPulse < (*v_ipulse).size(); iPulse++){
                if((*v_ipulse)[iPulse] != 0) continue;
                if((*v_nPE)[iPulse] < minCut) continue;
                barsHit.push_back((*v_chan)[iPulse]);
                barsHitRow.push_back((*v_row)[iPulse]);
                barsHitCol.push_back((*v_column)[iPulse]);
                layersHit[(*v_layer)[iPulse]] = true;
          }

          if(barsHit.size() < 3) continue;
          if(!layersHit.at(3) || !layersHit.at(1) || !layersHit.at(2)) continue;

          outputTree->Fill();
    }

    outputTree->Write();
    fOutput->Close();

}
