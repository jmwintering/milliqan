#include "treesV16TemplateTest.h"

vector<int> slabList = {18,20,28};
vector<vector<int>> extraPaths = { {2,7,12} , {1,3,5} , {0,7,8} };

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
    for(int j = 0; j < 2; j++){
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


void makeSkim(TString outFilename, TString inFilename) {
  // create output tree using default input tree
  TFile * fInput = new TFile("/data/abcd_data/methodFiles/1376.root", "READ");
  TTree * input = (TTree*)fInput->Get("t");
  cout << endl << inFilename << endl; 
  TFile * fOutput = new TFile("/data/abcd_data/methodFiles/" + outFilename + ".root", "RECREATE");
  TTree * outputTree = input->CloneTree();
  outputTree->Reset();
  SetAddressesTree(outputTree);

  // grab input tree 
  TChain * inputTree = new TChain("t");
  inputTree->Add("/data/mcptest_m1p0_q0"+inFilename+".root");
  // for(int i=1;i<11;++i) inputTree->Add("/data/HS_Run1090/"+toTstring(tostr(i))+"/*.root"); // 1090: triple channel, bars, slabs, panels (physics)
  // for(int i=1;i<11;++i) inputTree->Add("/data/HS_Run1327/"+toTstring(tostr(i))+"/*.root"); // 1327: CD CH0 and CH2, long window
  // for(int i=1;i<11;++i) inputTree->Add("/data/HS_Run1612/"+toTstring(tostr(i))+"/*.root"); // triple channel, bars, slabs, panels (4 layer, cosmics)
  for(int i=1;i<11;++i) inputTree->Add("/data/Run1366*.root"); // triple channel, bars, slabs, panels (4 layer, cosmics)
  SetAddressesChain(inputTree);
  int entries = inputTree->GetEntries();
  if(entries==0){
    cout << "Error: no entries in tree" << endl;
    return;
  }
  else cout << "Entries added: " << entries << endl;

  TH1::SetDefaultSumw2();

  vector<string> cutNames = {"RMS cut", "Single Pulse", "nBars Active not above 3", "one hit per layer"};
  vector<int> nEntries = {0,0,0,0};

  for(int iEvent = 0; iEvent < entries; ++iEvent){
    displayProgress(iEvent, entries);
    int entryRetrieved = inputTree->GetEntry(iEvent);
    if(entryRetrieved == 0) continue;

    //same game group TDC
		// if((*v_groupTDC_b0)[0] != (*v_groupTDC_b1)[0]) continue;

    // max sideband RMS = 1.3
    bool quietRMS = true;
    for(int iPulse = 0; iPulse < (*v_sideband_RMS).size(); iPulse++) {
      if((*v_sideband_RMS)[iPulse] > 1.3) quietRMS = false;
    }
    // if(!quietRMS) continue;
    nEntries[0]++;

    //FIXME: hard veto on the sheets????
    // make sure detector is quiet
    /*
    bool detectorQuiet = true;
    for(int iPulse = 0; iPulse < (*v_ipulse).size(); iPulse++) {
      if( ((*v_type)[iPulse] != kBar) && ((*v_type)[iPulse] != kSlab)) detectorQuiet = false;
    }
    if(!detectorQuiet) continue;
    */

    // Cut variables
    bool allSlabsHit =  false;
    vector<bool> slabsHit(3, false);
    int nBarsActive = 0;
    int nSlabsActive = 0;
    vector<int> nBarsActiveInLayer(3, 0);
    bool singlePulse = true;
    vector<int> slabs = {};
    bool extraPath = false;
    vector<int> chans = {};

    for(int iPulse = 0; iPulse < (*v_ipulse).size(); iPulse++) {
      //one pulse per bar only, no other Pulses
    	if((*v_ipulse)[iPulse] != 0){
          singlePulse = false;
          break;
      }

      vector<int> info = checkType((*v_chan)[iPulse]);
      // count how many bars and bar per layer are active
    	if(info[0] == kBar) {
        nBarsActive++;
        nBarsActiveInLayer[info[1]]++;
      }

      //count how many slabs and which are active
      if(info[0]== kSlab){
        nSlabsActive++;
        slabs.push_back((*v_chan)[iPulse]);
        for(int iSlab = 0; iSlab < slabList.size(); iSlab++) {
          if( (*v_chan)[iPulse]  == slabList[iSlab]) slabsHit[iSlab] = true;
        }
      }

      // store the channels with hits
      chans.push_back((*v_chan)[iPulse]);

    } // iPulse loop end

    // cout << "hi" << endl;

    // check to see if event is an extra path
    chans = sort_vector(chans);
    for(int iList=0; iList < extraPaths.size(); ++iList){
      if(chans == extraPaths[iList]) extraPath = true;
    }
    if(slabsHit[0] && slabsHit[1] && slabsHit[2]) allSlabsHit = true;

    // only 1 pulse per channel
    if(!singlePulse) continue;
    nEntries[1]++;

    if(nBarsActive != 3) continue;
    nEntries[2]++;

    if((nBarsActiveInLayer[0] != 1 || nBarsActiveInLayer[1] != 1 || nBarsActiveInLayer[2] != 1)) continue;
    nEntries[3]++;

    outputTree->Fill();
  } // end event loop

  ofstream fout;
  fout.open("/data/abcd_data/cutEfficiencies_method.txt", ios::app);
  fout << inFilename << endl;
  fout << "initial entries: " << entries << endl;
  for(int iCut=0; iCut < cutNames.size(); ++iCut){
    fout << cutNames[iCut] << ": " << nEntries[iCut] << " (" << nEntries[iCut]*100.0  / entries << "%)" << endl;
    cout << cutNames[iCut] << ": " << nEntries[iCut] << " (" << nEntries[iCut]*100.0  / entries << "%)" << endl;
  }
  fout.close();
  outputTree->Write();
  fOutput->Close();
} // end function



void abcdMethod_injected(TString fileName=""){
  TString filename = "";
  if(fileName != "") filename = fileName;
  else filename = "/data/mcptest*.root"; 

  vector<TString> injectionOuts = {
    "p3", "p2", "p1", "p14", "p07", "p05", "p03", "p02", "p01", "p005"
  };

  gSystem->Exec("rm /data/abcd_data/cutEfficiencies_method.txt");
  gSystem->Exec("touch /data/abcd_data/cutEfficiencies_method.txt");

  for(int i=0; i<injectionOuts.size(); ++i){
    makeSkim("abcdInjectedSkim_"+injectionOuts[i], injectionOuts[i]);
  }

} // end function
