#include "treesV16TemplateTest.h"

vector<TString> injectionOuts = {
    "p3", "p2", "p1", "p14", "p07", "p05", "p03", "p02", "p01", "p005"
  };

vector<int> checkType(int ch){
  
  int type = -1;
  int layer = -1;
  int column = -1;
  int row = -1;

  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 2; j++){
      for(int k = 0; k < 3; k++){
        if(ch == channelMap[i][j][k]){
          type = 0;
          layer = i;
          column = j;
          row = k;
        }
      }
    }
  }

  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      if(ch == sheetMap[i][j]){
        type = 1;
        layer = i;
      }
    }
  }

  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      if(ch == slabMap[i][j]){
        type = 2;
        layer = i;
      }
    }
  }

  vector<int> info(4,0);
  info[0] = type;
  info[1] = layer;
  info[2] = column;
  info[3] = row;

  return info;
}


vector<double> findTimeExtrema(vector<double> time, bool slabs, vector<int> layer={}){
  double maxDeltaT = -1;
  double minDeltaT = 1e9;

  vector<double> deltaTimes = {};
  if(!slabs){
    for(int iTime=0; iTime < time.size(); ++iTime){
      for(int jTime=0; jTime < time.size(); ++jTime){
        if(iTime != jTime && (layer[iTime] > layer[jTime])) deltaTimes.push_back(time[iTime] - time[jTime]) ;
      }
    }
  }
  else{
    for(int iTime=0; iTime < time.size(); ++iTime){
      for(int jTime=0; jTime < time.size(); ++jTime){
        if(iTime != jTime) deltaTimes.push_back(time[iTime] - time[jTime]) ;
      }
    }
  }

  vector<double> deltaTimeAbs = {};
  for(int j = 0; j < deltaTimes.size(); j++){
    deltaTimeAbs.push_back(abs(deltaTimes[j]));
  }

  maxDeltaT = maxElement(deltaTimeAbs);
  minDeltaT = minElement(deltaTimeAbs);
  
  // int maxIndex = -1;
  // int minIndex =-1;
  // for(int j = 0; j < deltaTimeAbs.size(); j++){
  //   if(deltaTimeAbs[j] == maxDeltaT) maxIndex = j;
  //   if(deltaTimeAbs[j] == minDeltaT) minIndex = j;
  // }

  // maxDeltaT = deltaTimes[maxIndex];
  // minDeltaT = deltaTimes[minIndex];

  return  {minDeltaT, maxDeltaT};
}


void abcdPrepare_injected(){

  TString selection = "noSlab";
  gSystem->Exec("rm /data/abcd_data/cutEfficiencies_"+selection+".txt");
  gSystem->Exec("touch /data/abcd_data/cutEfficiencies_"+selection+".txt");

  for(int iFile = 0; iFile < injectionOuts.size(); iFile++){
    cout << endl << injectionOuts[iFile] << endl;
    TChain * chain = new TChain("t");
    int entries = 0;
    chain->Add("/data/abcd_data/methodFiles/abcdInjectedSkim_"+injectionOuts[iFile]+".root");
    entries = chain->GetEntries();

    SetAddressesChain(chain);
    cout << "Added " << entries << " entries" << endl;
    TTree * outputTree = new TTree("outputTree", "outputTree");

    bool isStraightPath = false;
    bool beam_on;
    double minNPE = 1e9;
    double maxNPE = -1;
    double minSlabNPE = 1e9;
    double maxSlabNPE = -1;
    double minBarNPE = 1e9;
    double maxBarNPE = -1;
    double maxDeltaT = -1;
    double minDeltaT = 1e9;
    double maxSlabDeltaT = -1;
    double minSlabDeltaT = 1e9;
    bool sameDigi  = false;
    int run = -1;

    outputTree->Branch("isStraightPath", &isStraightPath);
    outputTree->Branch("beam_on", &beam_on);
    outputTree->Branch("minNPE", &minNPE);
    outputTree->Branch("maxNPE", &maxNPE);
    outputTree->Branch("maxSlabNPE", &maxSlabNPE);
    outputTree->Branch("minSlabNPE", &minSlabNPE);
    outputTree->Branch("minBarNPE", &minBarNPE);
    outputTree->Branch("maxBarNPE", &maxBarNPE);
    outputTree->Branch("maxDeltaT", &maxDeltaT);
    outputTree->Branch("minDeltaT", &minDeltaT);
    outputTree->Branch("maxSlabDeltaT", &maxSlabDeltaT);
    outputTree->Branch("minSlabDeltaT", &minSlabDeltaT);
    outputTree->Branch("sameDigi", &sameDigi);
    outputTree->Branch("run", &run);

    enum typeList {kBar = 0, kSlab = 1, kSheet = 2};
    vector<int> badChans = {6,4};

   
    vector<string> cutNames = {"Single Pulse", "one hit per layer", "no sheet", "one slab"};
    vector<int> nEntries = {0,0,0,0};

    TH1D * hSheet = new TH1D(injectionOuts[iFile], injectionOuts[iFile], 200, 0 ,1000);

    for(int i = 0; i < entries; i++) {
        displayProgress(i, entries);
        chain->GetEntry(i);

        beam_on = beam;
        run = runNum;

        vector<int> channel = {};
        vector<int> type = {};
        vector<double> time = {};
        vector<double> slabTime = {};
        vector<int> row = {};
        vector<int> col = {};
        vector<int> barRow = {};
        vector<int> barCol = {};
        vector<double> nPE = {};
        vector<int> bars = {};
        vector<int> layer = {};

        bool singlePulse = true;

        for(int j = 0; j < (*v_ipulse).size(); j++) {
          //single pulse everything (in the slabs, bars already ruled out in method)
          if((*v_ipulse)[j] != 0){
            singlePulse = false;
            break;
          }

          // store event info in simpler vectors
          channel.push_back((*v_chan)[j]);
          if((*v_type)[j] == kBar) time.push_back((*v_time_module_calibrated)[j]);
    
          nPE.push_back((*v_nPE)[j]);

          vector<int> info = checkType((*v_chan)[j]);
          type.push_back(info[0]);
          row.push_back(info[3]);
          col.push_back(info[2]);
          layer.push_back(info[1]);

          if(type[j] == kSlab){
            slabTime.push_back((*v_time_module_calibrated)[j]);
          }

          if(type[j] == kBar){
            barRow.push_back(info[3]);
            barCol.push_back(info[2]);
            // cout << info[3] << " " << info[2] << endl;
            bars.push_back((*v_chan)[j]);
          }
        } // end ipulse loop

        // only want one pulse
        if(!singlePulse) continue;
        nEntries[0]++;

        // re initialize desired variables
        minNPE = 1e9;
        maxNPE = -1;
        minSlabNPE = 1e9;
        maxSlabNPE = -1;
        maxBarNPE = -1;
        minBarNPE = 1e9;
        minSlabNPE = 1e9;
        isStraightPath = false;
        minDeltaT = 1e9;
        maxDeltaT = -1;
        minSlabDeltaT = 1e9;
        maxSlabDeltaT = -1;
        sameDigi = false;


        // find max and min times
        minDeltaT = findTimeExtrema(time, false, layer)[0];
        maxDeltaT = findTimeExtrema(time, false, layer)[1];

        if(slabTime.size() >= 2){
          minSlabDeltaT = findTimeExtrema(slabTime, true)[0];
          maxSlabDeltaT = findTimeExtrema(slabTime, true)[1];
        }
        
      
        int slabsActive = 0;
        int sheetsActive = 0;
        bool badChan = false;
        //min and max NPE and slab/bar NPE
        for(int j = 0; j < channel.size(); j++){

          vector<int> info = checkType(channel[j]);
          if(nPE[j] < minNPE) minNPE = nPE[j];
          if(nPE[j] > maxNPE) maxNPE = nPE[j];
          if(info[0] == kSlab && nPE[j] < minSlabNPE) minSlabNPE = nPE[j];
          if(info[0] == kSlab && nPE[j] > maxSlabNPE) maxSlabNPE = nPE[j];
          if(info[0] == kBar && nPE[j] < minBarNPE) minBarNPE = nPE[j];
          if(info[0] == kBar && nPE[j] > maxBarNPE) maxBarNPE = nPE[j];
          if(info[0] == kSlab) {
            if(nPE[j] > 100) slabsActive++;
          }
          if(info[0]==kSheet){
            if(nPE[j] > 50) sheetsActive++;
            hSheet->Fill(nPE[j]);
          };
        }

        //one bar hit per layer
        if(bars.size() != 3) continue;
        nEntries[1]++;

        //no sheet, f noise
        if(sheetsActive>0) continue;
        nEntries[2]++;
        //if(find(type.begin(), type.end(), kSlab) != type.end()) continue;
        if(slabsActive==1) continue;
        nEntries[3]++;

        //path
        int row_var = abs(barRow[0] - barRow[1]) + abs(barRow[1] - barRow[2]) + abs(barRow[0] - barRow[2]);
        int col_var = abs(barCol[0] - barCol[1]) + abs(barCol[1] - barCol[2]) + abs(barCol[0] - barCol[2]);
        if(row_var + col_var == 0) isStraightPath = true;
        else {
          isStraightPath = false;
          //cout << "NOT STRAIGHT" << endl;
        }

        if(bars[0]/16  == bars[1] /16 &&  bars[2]/16 == bars[0]/16) sameDigi = true;

        outputTree->Fill();

    } // end event loop

    ofstream fout;
    fout.open("/data/abcd_data/cutEfficiencies_"+selection+".txt", ios::app);
    fout << endl << injectionOuts[iFile] << endl;
    for(int iCut=0; iCut < cutNames.size(); ++iCut){
      fout << cutNames[iCut] << ": " << nEntries[iCut] << " (" << nEntries[iCut]*100.0  / entries << "%)" << endl;
      cout << cutNames[iCut] << ": " << nEntries[iCut] << " (" << nEntries[iCut]*100.0  / entries << "%)" << endl;
    }
    fout.close();

    cout << endl << "Output tree has " << outputTree->GetEntries() << " entries." << endl;
    TFile * fOutput = new TFile("/data/abcd_data/methodFiles/abcdInjectedPrepared_"+injectionOuts[iFile]+"_"+selection+"hit.root", "RECREATE");
    cout << "Writing to file..." << endl;
    outputTree->Write();
    cout << "Finished writing " << endl;
    fOutput->Close();

    TCanvas * c1 = new TCanvas("c1");
    c1->cd();
    hSheet->Draw();
    c1->SaveAs("/data/abcd_data/sheetNpePlots/"+injectionOuts[iFile]+".pdf");
  }

} // end function
