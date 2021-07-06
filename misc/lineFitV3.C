#include "treesV16TemplateHodoscope.h"
#include <fstream>

vector<TFormula*> createFittedLine(double a, double b, double c, double d){
  vector<TFormula*> fitEqs = {};
    TFormula * z = new TFormula("", "x");
    TFormula * y = new TFormula("", "(x-[0])/[1]");
    TFormula * x = new TFormula("", "(x-[0])/[1]");
    // create functions to express fitted line
    y->SetParameter(0,d);
    y->SetParameter(1,c);
    x->SetParameter(0,b);
    x->SetParameter(1,a);

    fitEqs.push_back(x);
    fitEqs.push_back(y);
    fitEqs.push_back(z);
  return fitEqs;
} // end function


void testChannels(vector<TFormula*> fitEqs, vector<int> &channelsHit, vector<double> &pathlengths) {
  //bottom left corners coordinates in xy plane
  // bar is 80 cm long
  /*
  (-5.5, 10.8) (.2, 10.8)
  (-5.5, 4.5)  (.2, 4.5)
  (-5.5, -1.8) (.2, -1.8)
  */

  // bar dims
  /*
  double x_length = 5.4;
  double y_length = 5.2;
  z_length = 80;


  //separation values
  z_hodoscope_bottom_table = .5;
  z_hodoscope_top_table = 10.5;
  z_bars = 14;
    */

  double startValue = -180; // all in cm
  double endValue = 180;
  double increment = 1;

  TFormula * x = fitEqs[0];
  TFormula * y = fitEqs[1];
  TFormula * z = fitEqs[2];

  //vector<int> channelsHit = {};
  int previous_channel_hit = -1;
  int channel_hit = -1;
  //vector<double> pathlengths(32,0);
  vector<double> p0(3,0);

  for(double iZ = startValue; iZ < endValue; iZ += increment){
    //layer
    int layer_hit(-1), col_hit(-1), row_hit(-1);
    if(z->Eval(iZ) > -179.5+12.6 && z->Eval(iZ) <= -179.5+12.6+80) layer_hit = 0;
    if(z->Eval(iZ) > -179.5+12.6+80+35.6+12.6 && z->Eval(iZ) <= -179.5+12.6+80+35.6+12.6+80) layer_hit = 1;
    if(z->Eval(iZ) > -179.5+12.6+80+35.6+12.6+80+35.6+12.6 && z->Eval(iZ) <= -179.5+12.6+80+35.6+12.6+80+35.6+12.6+80) layer_hit = 2;

    //col
    if(x->Eval(iZ) > -5.08 -.1 && x->Eval(iZ) < -.1) col_hit = 0;
    if(x->Eval(iZ) > .2 && x->Eval(iZ) < .2 +5.08) col_hit = 1;
    //row
    if(y->Eval(iZ) > 10.8 && y->Eval(iZ) < 16) row_hit = 2;
    if(y->Eval(iZ) > 4.5 && y->Eval(iZ) < 9.7) row_hit = 1;
     if(y->Eval(iZ) > -1.8 && y->Eval(iZ) < 3.4) row_hit = 0;

    // add channel if hit

    if(layer_hit != -1 && col_hit != -1 && row_hit != -1) channel_hit = channelMap[layer_hit][col_hit][row_hit];
    else channel_hit = -1;

    if(previous_channel_hit != channel_hit && channel_hit != -1) channelsHit.push_back(channel_hit);
    if((previous_channel_hit != -1 && channel_hit == -1) || (previous_channel_hit != channel_hit  && channel_hit != -1 && previous_channel_hit != -1)){
      // if going from channel to empty space OR switching channels AND not going from empty space to empty space
     pathlengths[previous_channel_hit] = sqrt(pow((x->Eval(iZ-increment) - p0[0]),2.0) + pow((y->Eval(iZ-increment) - p0[1]),2.0)  + pow((z->Eval(iZ-increment) - p0[2]),2.0));
    }
    if(previous_channel_hit != channel_hit && channel_hit != -1){
      // store starting points only when you are switching channels AND not switching into empty space
      p0[0] = x->Eval(iZ);
      p0[1] = y->Eval(iZ);
      p0[2] = z->Eval(iZ);
    }
    //cout << previous_channel_hit << " end: " << x->Eval(iZ - increment) << " "<< y->Eval(iZ-increment) << " " << z->Eval(iZ-increment) << endl;
    //cout << channel_hit << " p0:  "<< p0[0] << " " << p0[1] << " " << p0[2] << endl;
    previous_channel_hit = channel_hit;

  }

  for(int i = 0; i < pathlengths.size(); i++) {
    //cout << "Channel " << i << " path " << pathlengths[i] << endl;
  }

  return;
} // end function

void efficiency(vector<int> barsHit, vector<int> chansPredicted, vector<int> barsHitLayer, vector<double> barsHitnPE, vector<int> &hit, vector<int> &missed, vector<double> &hit_1, vector<double> &hit_2, vector<double> &hit_3) {

  for(int k = 0; k < chansPredicted.size(); k++) {

    int chanX = chansPredicted[k];

    bool layer_1 = false;
    bool layer_2 = false;
    bool layer_3 = false;

    int layer = -1;

    for(int l = 0; l < barsHit.size(); l++) {
      if(barsHit[l] == chanX) layer = barsHitLayer[l];
      if(barsHitLayer[l] == 1 || layer == 1) layer_1 = true;
      if(barsHitLayer[l] == 2 || layer == 2) layer_2 = true;
      if(barsHitLayer[l] == 3 || layer == 3) layer_3 = true;
    }


    if(layer_1 && layer_2 && layer_3) {
      bool path = true;
      //check that all predicted layers have a pulse EXCEPT CHANX
      for(int m = 0; m < chansPredicted.size(); m++) {
        if(chansPredicted[m] == chanX) continue;
        bool match = false;
        for(int n = 0; n < barsHit.size(); n++) {
          if(barsHit[n] == chansPredicted[m]) {
            match = true;
          }
        }
        //if predicted channel has no hit in it
        if(!match) {
          path = false;
        }
      }

      if(path) {
        bool pulse = false;
        for(int o = 0; o < barsHit.size(); o++) {
          if(barsHit[o] == chanX){
            pulse = true;
            hit[chanX]++;
            if(layer == 1) hit_1.push_back(barsHitnPE[o]);
            if(layer == 2) hit_2.push_back(barsHitnPE[o]);
            if(layer == 3) hit_3.push_back(barsHitnPE[o]);
          }
        }
        if(!pulse) {
          missed[chanX]++;
        }
      }
    }
  }
  //check channel wanted predicted
  //check one hit per layer
  //check channel area

}



void lineFitV3() {
  TChain * chain = new TChain("t");
  TString directory = "/data/users/MilliQan/skim_fourslabs_b0b1synched_hsadded_run1079to1375.root";
  SetAddresses(chain);
  chain->Add(directory);

  TH2D * h1 = new TH2D("h1","h1", 40, 0, 100, 40, 0, 100000);
  TCanvas *c1 = new TCanvas("c1", "c1", 200,10,700,500);
  TH1D * h2 = new TH1D("h2", "h2", 200, 0, 350000);
  TH1D * h3 = new TH1D("h3", "h3", 200, 0, 350000);
  TCanvas *c2 = new TCanvas("c2", "c2", 200, 10, 700, 500);
  TH1D *h4 = new TH1D("h4", "h4", 100, 0, 350000);
  TH1D *h5 = new TH1D("h5", "h5", 100, 0, 350000);
  TH1D *h6 = new TH1D("h6", "h6", 100, 0, 350000);
  TCanvas *c3 = new TCanvas("c3", "c3", 200, 10, 700, 500);
  TCanvas *c4 = new TCanvas("c4", "c4", 200, 10, 700, 500);

  int agree = 0;
  int not_predicted = 0;
  int over_predicted = 0;

  vector<int> hit(32, 0);
  vector<int> missed(32, 0);
  vector<double> hit_1, hit_2, hit_3;

  for(int iEvent=0; iEvent<chain->GetEntries(); ++iEvent){

    chain->GetEntry(iEvent);
    // don't look at events with no fit parameters
    if((*v_fit_xz).size() <= 1 || (*v_fit_yz).size() <= 1) continue;

    int top_count = 0;
    int bottom_count = 0;

    /*
    for(int j = 0; j < (*v_hs).size();j++) {

      if((*v_hs)[j] < 32) top_count ++;
      if((*v_hs)[j] >= 32) bottom_count ++;

     // cout << (*v_hs)[j] << " ";

    }
    //cout << endl;


    if(top_count != 4 || bottom_count != 4) continue;*/

    vector<TFormula*> fitEquations = createFittedLine((*v_fit_xz)[0],(*v_fit_xz)[1],(*v_fit_yz)[0],(*v_fit_yz)[1]);
    vector<int>  chansPredicted = {};
    vector<double> pathlengths(32,0);
    testChannels(fitEquations,  chansPredicted, pathlengths);

    int agree_event = 0;
    int channels_pulse = 0;

/*
    for(int iPulse = 0; iPulse < (*v_ipulse).size(); ++iPulse){
      if((*v_ipulse)[iPulse] != 0 || (*v_type)[iPulse] != 0) continue;
      bool hitChan = false;
      for(int iChan = 0; iChan < chansPredicted.size(); ++iChan){
        if((*v_chan)[iPulse] == chansPredicted[iChan]){
          hitChan = true;
          //if(pathlengths[iChan] < 0.00000001) continue;
          h1->Fill((*v_area)[iPulse],pathlengths[iChan]);
          h2->Fill((*v_area)[iPulse]);
        }
      }
      if(!hitChan) h3->Fill((*v_area)[iPulse]);
    }

*/
    vector<int> barsHit;
    vector<int> barsHitColumn;
    vector<int> barsHitRow;
    vector<double> barsHitHeight;
    vector<int> barsHitLayer;
    vector<double> barsHitArea;
    vector<double> barsHitnPE;

    for(int j=0; j<(*v_ipulse).size(); ++j){

      //count initial pulses
      if((*v_ipulse)[j] != 0 || (*v_type)[j] != 0)continue;

      barsHit.push_back((*v_chan)[j]);
      barsHitRow.push_back((*v_row)[j]);
      barsHitColumn.push_back((*v_column)[j]);
      barsHitHeight.push_back((*v_height)[j]);
      barsHitArea.push_back((*v_area)[j]);
      barsHitnPE.push_back((*v_nPE)[j]);
      barsHitLayer.push_back((*v_layer)[j]);

    } // end ipulse loop

    for(int iHit = 0; iHit < barsHit.size(); iHit++) {
          channels_pulse++;
          bool predicted = false;

          for(int iPred = 0; iPred <  chansPredicted.size(); iPred++){
            if(barsHit[iHit]== chansPredicted[iPred]) {
              agree_event++;
              predicted = true;

              //expected path length vs area in demonstrator
              h1->Fill(pathlengths[iPred],barsHitArea[iHit]);

              //area of pulse predicted and seen
              h2->Fill(barsHitArea[iHit]);
            }
          }

          //area of pulse not predicted
          if(!predicted) h3->Fill(barsHitArea[iHit]);

    }

    if(barsHit.size() >= 3 && chansPredicted.size() >= 3) {
      efficiency(barsHit, chansPredicted, barsHitLayer, barsHitArea, hit, missed, hit_1, hit_2, hit_3);
    }



    agree += agree_event;
    not_predicted += channels_pulse - agree_event;
    over_predicted +=  chansPredicted.size()-agree_event;

  } // chain loop

  cout << endl << "agree " << agree << endl;
  cout << "not predicted " << not_predicted << endl;
  cout << "over predicted " << over_predicted << endl;

  //print counts of hits, missed, efficiencies, and error to file
  ofstream fout;
  fout.open("efficiencies.txt", ios::out);

  fout << setw(8) << "channel" << setw(8) << "hit" << setw(8) <<  "missed" << setw(15) <<  "efficiency" << setw(15) <<  "error" << endl;
  for(int i = 0; i < hit.size(); i++) {
    if(hit[i] != 0 || missed[i] != 0) {
      fout << setw(8) << i << setw(8) << hit[i] << setw(8) << missed[i] << setw(15) <<  hit[i] * 1.0 / (missed[i]+hit[i]) << setw(15) <<  sqrt((hit[i]/pow(hit[i]+missed[i], 2.0)) + ((hit[i]+missed[i])*(pow(hit[i],2.0))/pow(hit[i]+missed[i],4.0)));
      fout << endl;
    }
  }
  fout.close();


  c1->cd();
  h1->Draw("colz");
  h1->GetYaxis()->SetTitle("Area of first pulse in channel");
  h1->GetXaxis()->SetTitle("Path Length (cm)");
  c1->SaveAs("pathlength_area.pdf");

  c4->cd();
 	TH1D * hprojection = h1->ProjectionY("hprojection", h1->GetXaxis()->FindBin(75), h1->GetYaxis()->FindBin(85));
 	hprojection->Draw();
 c4->SaveAs("projection.pdf");

  auto legend = new TLegend(0.1,0.7,0.48,0.9);
   legend->AddEntry(h2,"Predicted pulse","l");
   legend->AddEntry(h3,"No predicted pulse","l");

  c2->cd();
  c2->SetLogy();
  h3->Draw();
  h3->GetXaxis()->SetTitle("Area [mVps]");
  h3->GetYaxis()->SetTitle("Events");
  h3->Scale(1./h3->Integral());
  h2->Draw("same");
  h2->Scale(1./h2->Integral());
  legend->Draw();
  h3->SetLineColor(kRed);
  c2->SaveAs("area_predicted_non_predicted.pdf");



  c3->cd();
  for(int i = 0; i < hit_1.size(); i++) {
    h4->Fill(hit_1[i]);
  }
  for(int i = 0; i < hit_2.size(); i++) {
    h5->Fill(hit_2[i]);
  }
  for(int i = 0; i < hit_3.size(); i++) {
    h6->Fill(hit_3[i]);
  }
  h4->Draw();
  h5->Draw("same");
  h6->Draw("same");
  auto legend2 = new TLegend(0.1,0.7,0.48,0.9);
  legend2->AddEntry(h4,"layer 1","l");
  legend2->AddEntry(h5,"layer 2","l");
  legend2->AddEntry(h6,"layer 3","l");
  h4->GetXaxis()->SetTitle("Area [pVns]");
  h4->GetYaxis()->SetTitle("Events");
  legend2->Draw();
  h4->SetLineColor(kBlue);
  h5->SetLineColor(kRed);
  h6->SetLineColor(kBlack);
  c3->SaveAs("nPE_predicted_paths.pdf");


}
