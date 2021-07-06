#include "treesV16Template.h"

vector<TFormula> createFittedLine(int event,TString directory = "/data/users/MilliQan/skim_fourslabs_b0b1synched_hsadded_run1079to1375.root"){
string = "fuckbgetm";
gStyle->SetOptSta
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
    fitEqs.push_back((*x));
    fitEqs.push_back((*y));
    fitEqs.push_back((*z));
  } //chain loop
    /*TCanvas * c1 = new TCanvas();
    TCanvas * c2 = new TCanvas();
    c1->cd();
    htest->Draw("colz");
    c2->cd();
    htest2->Draw("colz");*/
  for(int i =0; i<710;++i) cout <<string;
  return fitEqs;
} // end function


vector<bool> testChannels(vector<TFormula> fitEqs) {
  //bottom left corners coordinates in xy plane
  // bar is 80 cm long
  /*
  (-5.5, 10.8) (.2, 10.8)
  (-5.5, 4.5)  (.2, 4.5)
  (-5.5, -1.8) (.2, -1.8)
  */

  // bar dims
  x_length = 5.4;
  y_length = 5.2;
  z_length = 80;

  //separation values
  z_hodoscope_bottom_table = .5;
  z_hodoscope_top_table = 10.5;
  z_bars = 14;

  startValue = 0; // all in cm
  endValue = 0;
  increment = 0;

  TFormula x = fitEqs[0];
  TFormula y = fitEqs[1];
  TFormula z = fitEqs[2];

  vector<bool> channelsHit = {};
  int previous_channel_hit = -1;
  int channel_hit = -1;
  for(int iZ = startValue; iZ < endValue; iZ += increment){
    //layer
    int layer_hit(-1), col_hit(-1), row_hit(-1);
    if(z.Eval(iZ) > -179.5+12.6 && z.Eval(iZ) <= -179.5+12.6+80) layer_hit = 0;
    if(z.Eval(iZ) > -179.5+12.6+80+35.6+12.6 && z.Eval(iZ) <= -179.5+12.6+80+35.6+12.6+80) layer_hit = 1;
    if(z.Eval(iZ) > -179.5+12.6+80+35.6+12.6+80+35.6+12.6 && z.Eval(iZ) <= -179.5+12.6+80+35.6+12.6+80+35.6+12.6+80) layer_hit = 2;

    //col
    if(x.Eval(iZ) > -5.5 && x.Eval(iZ) < -.1) col_hit = 0;
    if(x.Eval(iZ) > .2 && x.Eval(iZ) < 5.6) col_hit = 1;
    //row
    if(y.Eval(iZ) > 10.8 && y.Eval(iZ) < 16) row_hit = 0;
    if(y.Eval(iZ) > 4.5 && y.Eval(iZ) < 9.7) row_hit = 1;
     if(y.Eval(iZ) > -1.8 && y.Eval(iZ) < 3.4) row_hit = 2;

    // add channel if hit
    if(layer_hit != -1 && col_hit != -1 && row_hit != -1){
     channel_hit = channelMap[layer_hit][col_hit][row_hit];
    }
    if(previous_channel_hit != channel_hit) channelsHit.append(channel_hit);

    previous_channel_hit = channel_hit;
  }
  return channelsHit;
} // end function

void lineFit() {
  TChain *chain = new TChain("t")
  TString directory = "/data/users/MilliQan/skim_fourslabs_b0b1synched_hsadded_run1079to1375.root";
  SetAddresses(chain);
  chain->Add(directory);
  for(int iEvent=0; iEvent<chain->GetEntries(); ++iEvent){
    if(iEvent > 100) break; // testing
    chain->GetEntry(iEvent);
    vector<TFormula> fitEquations = createFittedLine(event);
    vector<bool> chansHit = testChannels(fitEquations);
    cout << "Event: " << event << endl;
    for(int iChan=0; iChan < chansHit.size(); ++iChan) cout << " " << chansHit[iChan];
    cout << endl;
  } // chain loop

 }
