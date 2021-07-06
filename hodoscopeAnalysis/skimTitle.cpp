#include "treesV16TemplateTest.h"

void untitled(string runNum){

    int runNumber = toint(runNum);
    TChain* chain = new TChain("t");
    TFile * fInput = new TFile("/store/user/wintering/hsTrees_v16/HS_Run1340/9/UX5MilliQan_Run1340.992_Physics_v16.root", "READ");
    TTree * input = (TTree*)fInput->Get("t");
    SetAddressesTreeHS(input);
    TFile * fOutput = new TFile("barEff" + toTstring(runNum) + ".root", "RECREATE");
    TTree * outputTree = input->CloneTree();
    outputTree->Reset();
    fInput->Close();
    SetAddressesTreeHS(outputTree);

    chain->Add("/store/user/wintering/hsTrees_v16/HS_Run"+ toTstring(tostr(runNumber))+ "/1/*.root");
    chain->Add("/store/user/wintering/hsTrees_v16/HS_Run"+ toTstring(tostr(runNumber))+ "/2/*.root");
    chain->Add("/store/user/wintering/hsTrees_v16/HS_Run"+ toTstring(tostr(runNumber))+ "/3/*.root");
    chain->Add("/store/user/wintering/hsTrees_v16/HS_Run"+ toTstring(tostr(runNumber))+ "/4/*.root");
    chain->Add("/store/user/wintering/hsTrees_v16/HS_Run"+ toTstring(tostr(runNumber))+ "/5/*.root");
    chain->Add("/store/user/wintering/hsTrees_v16/HS_Run"+ toTstring(tostr(runNumber))+ "/6/*.root");
    chain->Add("/store/user/wintering/hsTrees_v16/HS_Run"+ toTstring(tostr(runNumber))+ "/7/*.root");
    chain->Add("/store/user/wintering/hsTrees_v16/HS_Run"+ toTstring(tostr(runNumber))+ "/8/*.root");
    chain->Add("/store/user/wintering/hsTrees_v16/HS_Run"+ toTstring(tostr(runNumber))+ "/9/*.root");


    SetAddressesChainHS(chain);

    int n = chain->GetEntries();
    cout << "Added run " << runNumber << endl;
    cout << "Added " << n << " entries" << endl;

    for(int iEvent = 0; iEvent < n; iEvent++){
      chain->GetEntry(iEvent);

      if((*v_groupTDC_b0)[0] != (*v_groupTDC_b1)[0]) continue;

      //select events with fit data
      int n_hs_botver=0;
      int n_hs_bothor=0;
      int n_hs_topver=0;
      int n_hs_tophor=0;
      
      for(unsigned int ihs=0; ihs<(*v_hs).size(); ihs++)
      {
        int ch = (*v_hs)[ihs];
        if(ch<16) n_hs_botver++;
        if(ch>=48) n_hs_topver++;
        if(ch>=16 && ch<32) n_hs_bothor++;
        if(ch>=32 && ch<48) n_hs_tophor++;
      }

      bool doTrackFit = false;
      doTrackFit = n_hs_topver>0 && n_hs_botver>0 && n_hs_tophor>0 && n_hs_bothor>0; 
      if(!doTrackFit) continue;
     
      outputTree->Fill();
    }

    outputTree->Write();
    fOutput->Close();

}
