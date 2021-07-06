#include "treesV12Template.h"

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

void cmsMuonRate() {
  TChain * chain = new TChain("t");
  SetAddresses(chain);
  
  gStyle->SetOptStat(0);
  
  /*
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
  */
  
  cout << "Added " << chain->GetEntries() << " entries" << endl;
  

 vector<double> cuts(32,-1);
 

 int startTime = 0;
 
 for (int i = 0; i < chain->GetEntries(); i++) {
   chain->GetEntry(i);
   
   if(event ==0 && fileNum ==1){
     startTime = event_time_fromTDC;
   }
 }

 double maxTime = startTime;
 int num_muons = 0;

 for (int i = 0; i < chain->GetEntries(); i++) {
   chain->GetEntry(i);
   
   vector<bool> channels(32, false);
   vector<double> areas(32, -1.0);
   vector<double> heights(32, -1.0);
   vector<bool> layers(3, false);
   if(event_time_fromTDC > maxTime) maxTime = event_time_fromTDC;
   
   for (int j = 0; j<(*v_ipulse).size(); j++) {
     if ((*v_ipulse)[j] == 0) {
       channels[(*v_chan)[j]] = true;
       areas[(*v_chan)[j]] = (*v_area)[j];
       heights[(*v_chan)[j]] = (*v_height)[j];
       layers[(*v_layer)[j]] = true;
     }
   }

   for(int iPulse=0; iPulse<(*v_area).size(); iPulse++){
     if((*v_ipulse)[iPulse]!=0) continue;
     if(layers[0] && layers[1] && layers[2]){
       h1->Fill((*v_area)[iPulse]); // To see where we should cut to decide what is a muon
     }
   }
   
   ClearVectors();
 } // end of chain loop  
 
 // cout << "Rate of cms muons: " << num_muons / ((maxTime-startTime)/something that makes this Hz)

}


