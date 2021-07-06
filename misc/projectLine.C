#include "treesV16Template.h"

vector<TFormula> createFittedLine(int event,TString directory = "/data/users/MilliQan/skim_fourslabs_b0b1synched_hsadded_run1079to1375.root"){

gStyle->SetOptStat(0);
TChain * chain = new TChain("t");
//TH2D * htest = new TH2D("h","h", 200, -1e10, 1e10, 200,-1e10,1e10);
//TH2D * htest2 = new TH2D("h2","h2", 200, -1e10, 1e10, 200,-1e10,1e10);
chain->Add(directory);
SetAddresses(chain);

vector<TFormula> fitEqs = {};

//cout << "Added " << chain->GetEntries() << "events." << endl;
  for (int i=0; i<chain->GetEntries(); ++i){
    if(i != event) continue;
    TFormula * z = new TFormula("", "x");
    TFormula * y = new TFormula("", "(x-[0])/[1]");
    TFormula * x = new TFormula("", "(x-[0])/[1]");
    chain->GetEntry(i);
    double a(1),b(0),c(1),d(0);
    // cuts
        if((*v_fit_xz).size() == 0){
            cout << "Error, event has no fit params" << endl;
            return {};
        }
    //cout << "EVENT " << i << endl;
    for(int j =0; j < (*v_fit_xz).size(); ++j){
     // cout << "xz,yz"<<"["<<j<<"]: " << (*v_fit_xz)[j] << " " << (*v_fit_yz)[j] << endl;
    }
    //htest->Fill((*v_fit_yz)[0], (*v_fit_xz)[0]);
    //htest2->Fill((*v_fit_yz)[1], (*v_fit_xz)[1]);
    d = (*v_fit_yz)[1];
    c = (*v_fit_yz)[0];
    b = (*v_fit_xz)[1];
    a = (*v_fit_xz)[0];
    // create functions to express fitted line
    y->SetParameter(0,d);
    y->SetParameter(1,c);
    x->SetParameter(0,b);
    x->SetParameter(1,a);
    fitEqs.push_back(x);
    fitEqs.push_back(y);
    fitEqs.push_back(z);
  } //chain loop
    /*TCanvas * c1 = new TCanvas();
    TCanvas * c2 = new TCanvas();
    c1->cd();
    htest->Draw("colz");
    c2->cd();
    htest2->Draw("colz");*/
  return fitEqs;
}
