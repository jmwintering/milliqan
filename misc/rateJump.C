#include "treesV11Template.C"
#include <iomanip>


  int coincidenceTest(vector<double> heights, vector<bool> channels, vector<int> testChannels, vector<double> cuts){
    int validHeights = 0;
    int coincidenceLevel = 0;

    for(int i=0; i <testChannels.size(); ++i){
      if(channels[testChannels[i]]){
          ++validHeights;
      }
    }
    if(validHeights == 0){
      return -1; 
    }

    for(int i=0; i <testChannels.size(); ++i){
      if(channels[testChannels[i]] && (heights[testChannels[i]] > cuts[testChannels[i]])){
        ++coincidenceLevel;
        for(int j=0; j < 32; ++j){
          if(j!= testChannels[i]){
            if(channels[j]){
              ++coincidenceLevel;
            }
          }
        }
        return coincidenceLevel;
      }
    }
    return coincidenceLevel;
  }

void realTime(){
    time_t rawtime; 
    struct tm * timeinfo; 
    time( &rawtime ); 
    timeinfo = localtime( &rawtime ); 
    cout<< "Current local time and date: " << asctime (timeinfo)<< endl; 
}


void rateJump() {

  realTime();
  
  TChain * chain = new TChain("t");
  SetAddresses(chain);
  
  gStyle->SetOptStat(0);
  TCanvas * c1 = new TCanvas("c1");
  TCanvas * c2 = new TCanvas("c2");
  //range for whole run is 0 to 800 (in min)
  TH1D * h1 = new TH1D("h1", "Chan 30 Only", 125, 0, 800);
  TH1D * h2 = new TH1D("h2", "All Channels", 125, 0, 800);
  TH1D * h3 = new TH1D("h3", "Board 1", 125, 0, 800);
  TH1D * h4 = new TH1D("h4", "Board 0", 125, 0, 800);
  TH1D * h5 = new TH1D("h5", "Total", 500, 0, 800);
  TH2D * h6 = new TH2D("h6", "Event Num of Single Coin. Events v. File Num", 100, 0, 10000, 100, 0, 15);

  TString directory806 = "/data/users/MilliQan/trees_v12/Run806_doubleChannelCosmicApr23HV1350TD6/";
  TString directory805 = "/data/users/MilliQan/trees_v12/Run805_doubleChannelCosmicApr22HV1300TD6/";
  TString directory802 = "/data/users/MilliQan/trees_v12/Run802_doubleChannelCosmicApr21HV1200TD6/";


for(int i = 1; i <= 15; ++i){
   chain->Add(directory802 + Form("UX5MilliQan_Run802.%d_doubleChannelCosmicApr21HV1200TD6_v12.root", i));
} 

for(int i = 1; i <= 15; ++i){
   chain->Add(directory805 + Form("UX5MilliQan_Run805.%d_doubleChannelCosmicApr22HV1300TD6_v12.root", i));
}  
for(int i = 1; i <= 15; ++i){
   chain->Add(directory806 + Form("UX5MilliQan_Run806.%d_doubleChannelCosmicApr23HV1350TD6_v12.root", i));
}


cout << "Added " << chain->GetEntries() << " entries" << endl;
  

  vector<double> cuts(32,-1);
  /* {0.400,0.400,0.500,0.500,0.400,0.500,
  0.400,0.500,0.010,0.100,0.015,0.030,0.500,
  0.500,0.012, 0.500, 0.100, 0.250, 0.040, 0.250, 0.250,
   0.400,0.500,0.100,0.050,0.020,0.010,0.100,0.010,0.020,0.020};
   for(int i=0; i< cuts.size(); ++i){
    cuts[i] *= 1000;
   }
   */

  //Counts for coincidence rates per channel
  int totalSingle = 0;
  int totalDouble = 0;
  vector<int> singleCount(32,0);
  vector<int> doubleCount(32,0);
  
  //Test combinations
  vector<int> allChans = {};
  vector<int> boardZero = {};
  vector<int> boardOne = {};
  vector<int> chanThirty = {};
 
for(int i=0; i < 32; ++i){
    allChans.push_back(i);
    if(i < 16){
      boardZero.push_back(i);
    }
    else{
      boardOne.push_back(i);
    }
}
chanThirty.push_back(30);

vector<int> strangeEvents = {};
int startTime = 0;

 for (int i = 0; i < chain->GetEntries(); i++) {
  chain->GetEntry(i);

  if(event ==0 && fileNum ==1){
    startTime = event_time_fromTDC;
  }
}

  for (int i = 0; i < chain->GetEntries(); i++) {
    chain->GetEntry(i);
    
    vector<bool> channels(32, false);
    vector<double> areas(32, -1.0);
    vector<double> heights(32, -1.0);
    double eventTime = (event_time_fromTDC - startTime) / 60;
    h5->Fill(eventTime);
 
    for (int j = 0; j<(*v_ipulse).size(); j++) {
      if ((*v_ipulse)[j] == 0) {
      	channels[(*v_chan)[j]] = true;
      	areas[(*v_chan)[j]] = (*v_area)[j];
        heights[(*v_chan)[j]] = (*v_height)[j];
      }
    }
    if (coincidenceTest(heights, channels, chanThirty, cuts) == 1) {
      h1->Fill(eventTime);
    }
    
    if (coincidenceTest(heights, channels, allChans, cuts) == 1){
      h2->Fill(eventTime);
      strangeEvents.push_back((fileNum-1)*10000 + event);
      h6->Fill(event, fileNum);
      for (int i = 0; i < channels.size(); ++i)
      {
        if(!channels[i]) continue;
        //h6->Fill(heights[channels[i]]);
      }
    }

    if(coincidenceTest(heights, channels, boardOne, cuts) == 1){
      h3->Fill(eventTime);
    }

    if(coincidenceTest(heights, channels, boardZero, cuts) == 1){
      h4->Fill(eventTime);
    }  

    ClearVectors();
} // end of chain loop  
 
  TLegend * legend = new TLegend(0.1,0.7,0.48,0.9);
  legend->AddEntry(h1, "Ch 30 Only", "l");
  legend->AddEntry(h2, "All Channels", "l");
  legend->AddEntry(h3, "Board 1", "l");
  legend->AddEntry(h4, "Board 0", "l");
  

  h5->Draw();
  h2->Draw("same");
  h1->Draw("same");
  h3->Draw("same");
  h4->Draw("same");

  h1->SetLineColor(kRed);
  h2->SetLineColor(kBlue);
  h3->SetLineColor(kGreen);
  h4->SetLineColor(kBlack);
  h5->SetLineColor(kMagenta);
  h2->GetXaxis()->SetTitle("Rate"); 

  c2->cd();
  h6->Draw("colz");

  /*
c2->cd();
h5->Draw();
h5->SetLineColor(kPink);
*/

for (int i = 0; i < strangeEvents.size(); ++i)
{
 // cout << "Event " << strangeEvents[i] << endl;
}

  realTime();

}


