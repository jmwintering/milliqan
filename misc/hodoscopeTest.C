#include "treesV16Template.h"
#include <iostream>

void hodoscopeTest(){

  TString directory = "/data/users/MilliQan/skim_fourslabs_b0b1synched_hsadded_run1079to1375.root";


  TCanvas * c1 = new TCanvas("c1", "c1", 200,10,700,500);

  // graphs for each pulse height per bar
  TH2D *h2 = new TH2D("h2", "h2", 35, 0, 35, 35, 0, 35);

  TChain * chain = new TChain("t");
  chain->Add(directory);

  SetAddresses(chain);

  cout << "Added " << chain->GetEntries() << "events." << endl;

  for (int i=0; i<chain->GetEntries(); ++i){

    chain->GetEntry(i);


    // if(i > 100) break; //test

    int top_count = 0;
    int bottom_count = 0;
    for(int j = 0; j < (*v_hs).size();j++) {
      //cout << (*v_hs)[j] << endl;
      if((*v_hs)[j] < 32) top_count ++;
      if((*v_hs)[j] >= 32) bottom_count ++;
    }

    for(int j = 0; j < (*v_tp).size();j++) {
      cout << (*v_tp)[j] << endl;
      //if((*v_hs)[j] < 32) top_count ++;
      //if((*v_hs)[j] >= 32) bottom_count ++;
    }

    h2->Fill(top_count, bottom_count);

  } //chain loop

  c1->SetLogz();
  h2->Draw("colz");
}
