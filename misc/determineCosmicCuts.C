#include "treesV9Template.C"

vector<double> cosmicCuts(32, -1.0);
void setCosmicCuts(){
 cosmicCuts[2] = 4900;
 cosmicCuts[22] = 5100;
 cosmicCuts[4] = 5000;
 cosmicCuts[3] = 4853;
 cosmicCuts[23] = 2700;
 cosmicCuts[5] = 4139;
 cosmicCuts[6] = 4700;
 cosmicCuts[16] = 5200;
 cosmicCuts[12] = 5000;
 cosmicCuts[7] = 4750;
 cosmicCuts[17] = 26500;
 cosmicCuts[13] = 4000;
 cosmicCuts[0] = 4900;
 cosmicCuts[24] = 5010;
 cosmicCuts[8] = 0;
 cosmicCuts[1] = 4850;
 cosmicCuts[25] = 2700;
 cosmicCuts[9] = 4100;
}

vector<vector<vector<int>>> tagChannelsFunction(int layer){

  //top sheet
   vector<int> topTagChannelsLeft;
   vector<int> topTagChannelsRight;
   for (int j=0; j<HEIGHT; j++){
     topTagChannelsLeft.push_back(channelMap[layer][LEFT][j]);
     topTagChannelsRight.push_back(channelMap[layer][RIGHT][j]);
   }

   //side sheets
   vector<int> sideTagChannelsBot;
   vector<int> sideTagChannelsMid;
   vector<int> sideTagChannelsTop;
   for (int i=0; i<WIDTH; i++){
     sideTagChannelsBot.push_back(channelMap[layer][i][BOTTOM]);
     sideTagChannelsMid.push_back(channelMap[layer][i][MIDDLE]);
     sideTagChannelsTop.push_back(channelMap[layer][i][TOP]);
   }

   vector<vector<int>> sideTagChannels;
   sideTagChannels.push_back(sideTagChannelsBot);
   sideTagChannels.push_back(sideTagChannelsMid);
   sideTagChannels.push_back(sideTagChannelsTop);
   vector<vector<int>> topTagChannels;
   topTagChannels.push_back(topTagChannelsLeft);
   topTagChannels.push_back(topTagChannelsRight);

   //first index is sheet
   //second index is tag channel combo
   //third index is individual tag channel
   vector<vector<vector<int>>> tagChannels;
   tagChannels.push_back(sideTagChannels);
   tagChannels.push_back(topTagChannels);
   tagChannels.push_back(sideTagChannels);

  return tagChannels;

}

bool testChannels(vector<bool> channels, vector<double> areas, vector<int> channelsToTest){
  for (int i=0; i<channelsToTest.size(); i++){
    int testChannel = channelsToTest.at(i);
    if (testChannel<0 || testChannel>31){
      cout << "error. trying to test a channel that doesn't exist" << endl;
      return false;
    }
    if (!channels[testChannel] || areas[testChannel]<= 0) return false;
  }
  return true;
}

bool vetoChannels(vector<bool> channels, vector<double> areas, vector<int> channelsToVeto, int layer){
  for(int i=0; i <channelsToVeto.size(); ++i){
    int vetoChannel = channelsToVeto.at(i);
    if(areas[vetoChannel] > 0){
      return false;
    }
  }
  for(int j =0; j < 3; j = j+2){
    if(areas[sheetMap[layer][j]] > 0){
      return false;
    }
  }
  for(int k=0; k < 2; ++k){
    if(areas[slabMap[layer][k]] > 0){
      return false;
    }
  }
  
  
  return true;
}

  
string rowName(int row){
  if (row==0) return "Bottom";
  else if (row==1) return "Middle";
  else if (row==2) return "Top";
  else return "Error";
}

string columnName(int column){
  if (column==0) return "Left";
  else if (column==1) return "Right";
  else return "Error";
}

string sheetName(int num){
  if (num==0) return "Left";
  else if (num==1) return "Top";
  else if (num==2) return "Right";
  else return "Error";
}

void printResults(int misses[3][3][3], int totals[3][3][3]){

  for (int layer=0; layer<LAYERS; layer++){
    cout << "LAYER:" << layer << endl;
    for (int sheet=1; sheet<2; sheet++){
      cout << sheetName(sheet) << " Sheet: " << endl;
      int numCombos = (sheet==1) ? 2 : 3;
      for (int combo=0; combo<numCombos; combo++){
        string comboName = (sheet==1) ? columnName(combo) : rowName(combo);
        cout << " " << comboName << endl;
        cout << "  Missed: " << misses[layer][sheet][combo] << endl;
        cout << "  Events: " << totals[layer][sheet][combo] << endl;
	cout << "  Miss Rate: " << (double)misses[layer][sheet][combo]/(double)totals[layer][sheet][combo] << endl;
      }
    }
    cout << endl;
  }
}


string tostr(int x){
  stringstream str;
  str << x;
  return str.str();
}


void determineCosmicCuts() {

 setCosmicCuts();

 gStyle->SetOptStat(0);

 TChain * chain = new TChain("t");
 SetAddresses(chain);

 chain->Add("/data/users/MilliQan/trees_v10/Run795_doubleChannelCosmicApr15BarsPanelsSlabsTriggerDelay6/U*.root");
 // chain->Add("/data/users/MilliQan/trees_v9/Run422_singleChannelAllSlabsTopLayerFineTunedThresh/U*.root");
 // chain->Add("/data/users/MilliQan/trees_v9/Run423_singleChannelAllSlabsTopLayerFineTunedThresh/U*.root");

 cout << "Added " << chain->GetEntries() << " entries" << endl;

 //layer,sheet,combo
 int misses[3][3][3] = {0};
 int totals[3][3][3] = {0};

 vector<TCanvas*> cans = {};
 TCanvas * c0 = new TCanvas("c0");
 TCanvas * c1 = new TCanvas("c1");
 TCanvas * c2 = new TCanvas("c2");
 TCanvas * c3 = new TCanvas("c3");
 TCanvas * c4 = new TCanvas("c4");
 TCanvas * c5 = new TCanvas("c5");
 TCanvas * c6 = new TCanvas("c6");
 TCanvas * c7 = new TCanvas("c7");
 TCanvas * c8 = new TCanvas("c8");
 TCanvas * c9 = new TCanvas("c9");
 TCanvas * c10 = new TCanvas("c10");
 TCanvas * c11 = new TCanvas("c11");
 TCanvas * c12 = new TCanvas("c12");
 TCanvas * c13 = new TCanvas("c13");
 TCanvas * c14 = new TCanvas("c14");
 TCanvas * c15 = new TCanvas("c15");
 TCanvas * c16 = new TCanvas("c16");
 TCanvas * c17 = new TCanvas("c17");
 TCanvas * c18 = new TCanvas("c18");
 TCanvas * c19 = new TCanvas("c19");
 TCanvas * c20 = new TCanvas("c20");
 TCanvas * c21 = new TCanvas("c21");
 TCanvas * c22 = new TCanvas("c22");
 TCanvas * c23 = new TCanvas("c23");
 TCanvas * c24 = new TCanvas("c24");
 TCanvas * c25 = new TCanvas("c25");
 TCanvas * c26 = new TCanvas("c26");
 TCanvas * c27 = new TCanvas("c27");
 TCanvas * c28 = new TCanvas("c28");
 TCanvas * c29 = new TCanvas("c29");
 TCanvas * c30 = new TCanvas("c30");
 TCanvas * c31 = new TCanvas("c31");
 cans.push_back(c0);
 cans.push_back(c1);
 cans.push_back(c2);
 cans.push_back(c3);
 cans.push_back(c4);
 cans.push_back(c5);
 cans.push_back(c6);
 cans.push_back(c7);
 cans.push_back(c8);
 cans.push_back(c9);
 cans.push_back(c10);
 cans.push_back(c11);
 cans.push_back(c12);
 cans.push_back(c13);
 cans.push_back(c14);
 cans.push_back(c15);
 cans.push_back(c16);
 cans.push_back(c17);
 cans.push_back(c18);
 cans.push_back(c19);
 cans.push_back(c20);
 cans.push_back(c21);
 cans.push_back(c22);
 cans.push_back(c23);
 cans.push_back(c24);
 cans.push_back(c25);
 cans.push_back(c26);
 cans.push_back(c27);
 cans.push_back(c28);
 cans.push_back(c29);
 cans.push_back(c30);
 cans.push_back(c31);

 vector<TH1D*> histos = {};
 TH1D * h0 = new TH1D("h0", "Pulse area in Ch.", 70, 0, 50000);
 histos.push_back(h0);
 TH1D * h1 = new TH1D("h1", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h1);
 TH1D * h2 = new TH1D("h2", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h2);
 TH1D * h3 = new TH1D("h3", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h3);
 TH1D * h4 = new TH1D("h4", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h4);
 TH1D * h5 = new TH1D("h5", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h5);
 TH1D * h6 = new TH1D("h6", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h6);
 TH1D * h7 = new TH1D("h7", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h7);
 TH1D * h8 = new TH1D("h8", "Pulse Area in Ch.", 70, 0, 30000);
 histos.push_back(h8);
 TH1D * h9 = new TH1D("h9", "Pulse Area in Ch.", 70, 0, 100000);
 histos.push_back(h9);
 TH1D * h10 = new TH1D("h10", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h10);
 TH1D * h11 = new TH1D("h11", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h11);
 TH1D * h12 = new TH1D("h12", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h12);
 TH1D * h13 = new TH1D("h13", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h13);
 TH1D * h14 = new TH1D("h14", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h14);
 TH1D * h15 = new TH1D("h15", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h15);
 TH1D * h16 = new TH1D("h16", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h16);
 TH1D * h17 = new TH1D("h17", "Pulse Area in Ch.", 70, 0, 100000);
 histos.push_back(h17);
 TH1D * h18 = new TH1D("h18", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h18);
  TH1D * h19 = new TH1D("h19", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h19);
 TH1D * h20 = new TH1D("h20", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h20);
 TH1D * h21 = new TH1D("h21", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h21);
 TH1D * h22 = new TH1D("h22", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h22);
 TH1D * h23 = new TH1D("h23", "Pulse Area in Ch.", 70, 0, 10000);
 histos.push_back(h23);
 TH1D * h24 = new TH1D("h24", "Pulse Area in Ch.", 70, 0, 100000);
 histos.push_back(h24);
 TH1D * h25 = new TH1D("h25", "Pulse Area in Ch.", 70, 0, 100000);
 histos.push_back(h25);
 TH1D * h26 = new TH1D("h26", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h26);
 TH1D * h27 = new TH1D("h27", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h27);
 TH1D * h28 = new TH1D("h28", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h28);
 TH1D * h29 = new TH1D("h29", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h29);
 TH1D * h30 = new TH1D("h30", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h30);
 TH1D * h31 = new TH1D("h31", "Pulse Area in Ch.", 70, 0, 50000);
 histos.push_back(h31);


 vector<int> chans(32, -1);

for(int i = 0; i < chain->GetEntries(); ++i){
   chain->GetEntry(i);
   vector<bool> channels(32,false);
   vector<double> areas(32,-1.0);
   
   //fill the channels and area vectors
   for (int j=0; j<(*v_ipulse).size(); ++j){
     if ((*v_ipulse)[j]==0){
           channels[(*v_chan)[j]]=true;
           areas[(*v_chan)[j]]=(*v_area)[j];
     }
   }

   vector<vector<int>> oneSheetsTagChannels;
   vector<int> tagCombination;
   vector<int> vetoCombination;
   
   //test all the combinations & get info if it's passed
   for (int layer(0); layer<LAYERS; layer++){
     vector<vector<vector<int>>> tagChannels = tagChannelsFunction(layer);
     //go throught top sheet
     for (int sheet=1; sheet<2; sheet++){
        oneSheetsTagChannels.clear();
        oneSheetsTagChannels = tagChannels.at(sheet);
        for (int combo=0; combo<oneSheetsTagChannels.size(); combo++){
	  for(int row=0; row < 3; row++){
	    tagCombination.clear();
	    vetoCombination.clear();
	    tagCombination = oneSheetsTagChannels.at(combo);
	      if(combo == 1){
		vetoCombination = oneSheetsTagChannels.at(combo - 1);
	      }
	      else if(combo == 0){
		vetoCombination = oneSheetsTagChannels.at(combo + 1);
	      }
	    
	       if (testChannels(channels, areas, tagCombination) && vetoChannels(channels, areas, vetoCombination, layer)) {
		totals[layer][sheet][combo] += 1;
		histos[channelMap[layer][combo][row]]->Fill(areas[channelMap[layer][combo][row]]);
		if (!channels[sheetMap[layer][sheet]]){ misses[layer][sheet][combo] += 1;}
				}
	  }
	}
     }
     
     
   }
   
   if(i % 500000 == 0){ cout << i << endl; }
 }
 
 printResults(misses, totals);

 for (int q=0; q<histos.size(); q++){
   if(histos[q]->GetEntries() > 0){
   cans[q]->cd();
   cans[q]->SetLogy();

   cout << "Size: " << histos[q]->GetEntries() << endl;
   string title = "Pulse Area in Ch.";
   string channel = tostr(q);
   title += channel;
   histos[q]->SetTitle(title.c_str());
   histos[q]->GetXaxis()->SetTitle("Area (mVns)");
   histos[q]->Draw();
   string saveString = "/data/users/wintering/cosmicCutsToDetermine/";
   saveString += tostr(q);
   saveString += ".jpg";
   cans[q]->SaveAs(saveString.c_str());
   cans[q]->Close();
   }
 }


}
