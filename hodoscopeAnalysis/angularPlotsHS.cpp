#include "treesV16Template.h"

struct hit {
        int type;
        int column;
        int row;
        int layer;
        double height;
        double area;
        double nPE;
        int iPulse;
        int channel;
        double time;
};

void angularPlotsHS(){

  const vector<int> layer1_bars = {0,1,24,25,8,9};
  const vector<int> layer2_bars = {6,7,16,17,12,13};
  const vector<int> layer3_bars = {2,3,22,23,4,5};
  const vector<int> slabs = {18, 20, 28, 21};

        TChain * chain = new TChain("t");
        chain->Add("/data/methodFiles/barEfficiency/*.root");
        SetAddressesChainHS(chain);
        cout << "Update" << endl;

        TH1D *angularDistX[6];
        for(int i=0; i<6; i++) {
                TString name = "Path " + toTstring(tostr(i)) + ": " + toTstring(tostr(layer1_bars[i])) +  " " + toTstring(tostr(layer2_bars[i]))+  " "  + toTstring(tostr(layer3_bars[i])) + " X ; track angle (deg)";
                TString title = name;
                angularDistX[i] = new TH1D(name,title,20,-6,6);
        }

        TH1D *angularDistY[6];
        for(int i=0; i<6; i++) {
                TString name = "Path " + toTstring(tostr(i)) + ": " + toTstring(tostr(layer1_bars[i])) +  " " + toTstring(tostr(layer2_bars[i]))+  " "  + toTstring(tostr(layer3_bars[i])) + " Y ; track angle (deg)";
                TString title = name;
                angularDistY[i] = new TH1D(name,title,20,-6,6);
        }

        TH1D * totalAngularX = new TH1D("totalX", "totalX; track angle (deg)", 20, -6,6);
        TH1D * totalAngularY = new TH1D("totalY", "totalY; track angle (deg)", 20, -6,6);

        TH1D *angularDistX_notStraight[6];
        for(int i=0; i<6; i++) {
                TString name = "Path " + toTstring(tostr(i)) + ": " + toTstring(tostr(layer1_bars[i])) +  " " + toTstring(tostr(layer2_bars[i]))+  " "  + toTstring(tostr(layer3_bars[i])) + " X not straight ; track angle (deg)";
                TString title = name;
                angularDistX_notStraight[i] = new TH1D(name,title,20,-6,6);
        }

        TH1D *angularDistY_notStraight[6];
        for(int i=0; i<6; i++) {
                TString name = "Path " + toTstring(tostr(i)) + ": " + toTstring(tostr(layer1_bars[i])) +  " " + toTstring(tostr(layer2_bars[i]))+  " "  + toTstring(tostr(layer3_bars[i])) + " Y not straight ; track angle (deg)";
                TString title = name;
                angularDistY_notStraight[i] = new TH1D(name,title,20,-6,6);
        }

        TH1D * totalAngularX_notStraight = new TH1D("totalX not straight", "totalX not straight; track angle (deg)", 20, -6,6);
        TH1D * totalAngularY_notStraight = new TH1D("totalY not straight", "totalY not straight; track angle (deg)", 20, -6,6);

        TH2D * tb = new TH2D("tb", "tb; nHits in top HS; nHits in bottom HS", 10, 0, 10, 10, 0,10);
        TH2D * tb_notStraight = new TH2D("tb not straight", "tb not straight; nHits in top HS; nHits in bottom HS", 10, 0, 10, 10, 0,10);

        TH1D * testCoords = new TH1D("testCoords", "testCoords", 200, -10,10);

        TH1D * hsHits = new TH1D("hsHits", "hsHits; HS hits", 20, 0, 20);

        int straightBarEvents = 0;
        int straightHSevents = 0;
        int straightBarAndHSevents = 0;
        int weak_straightBarAndHSevents = 0;
        double minCut = 0;

        int entries = chain->GetEntries();
        cout << "added " << entries << " entries" << endl;

        for(int iE=0; iE<entries; ++iE) {
                if(iE%100000 == 0) cout << iE << endl;
                chain->GetEntry(iE);

                if(!beam) continue;

                //check for straight path through bars
                bool isStraightPath = false;
                bool isStraightHS = false;
                bool matched = false;
                bool weak_matched = false;
                bool otherBarsQuiet = false;
                vector<bool> straightPaths(6,false);
                vector<bool> straightPathsHS(6,false);
                vector<bool> layersHit = {false, false, false, false};
                vector<bool> slabsHit = {false, false, false, false};

                double sheetNPE = 0;
                double slabNPE = 0;
                double otherBarsNPE = 0;
                vector<hit> barHits;
                vector<int> chanHS;
                vector<double> path_length;

                //consider only beam on events, since we are looking for muons
                //if(!beam) continue;

                //consider only events with HS data
                bool skipEvent = false;
                for(unsigned int j = 0; j < (*v_hs).size(); j++) {
                        if((*v_hs)[j] == -1) skipEvent = true;
                }
                if(skipEvent) continue;

                for(unsigned int ipulse=0; ipulse < (*v_ipulse).size(); ++ipulse) {
                        if((*v_ipulse)[ipulse]!=0) continue;
                        if((*v_type)[ipulse] == 1) {
                                slabNPE += (*v_nPE)[ipulse];
                                for(unsigned int iSlab = 0; iSlab < slabs.size(); iSlab++) {
                                        if(slabs[iSlab] == (*v_chan)[ipulse]) slabsHit[iSlab] = true;
                                }
                        }
                        if((*v_type)[ipulse] == 2) sheetNPE += (*v_nPE)[ipulse];

                        if((*v_type)[ipulse]== 0 && (*v_nPE)[ipulse]>200) {

                                hit thisHit;
                                thisHit.type = (*v_type)[ipulse];
                                thisHit.column = (*v_column)[ipulse];
                                thisHit.row = (*v_row)[ipulse];
                                thisHit.layer = (*v_layer)[ipulse];
                                thisHit.height = (*v_height)[ipulse];
                                thisHit.area = (*v_height)[ipulse];
                                thisHit.nPE = (*v_nPE)[ipulse];
                                thisHit.iPulse = (*v_ipulse)[ipulse];
                                thisHit.channel = (*v_chan)[ipulse];
                                thisHit.time = (*v_time_module_calibrated)[ipulse];
                                barHits.push_back(thisHit);

                                layersHit[(*v_layer)[ipulse]] = true;
                        }

                        if((*v_type)[ipulse]== 0 && (*v_nPE)[ipulse]< 200 && (*v_nPE)[ipulse] > 10) {
                                otherBarsNPE += (*v_nPE)[ipulse];
                                otherBarsQuiet = true;
                        }

                }

                //selections
                //if(sheetNPE > 10) continue;
                //if(slabNPE < 1000) continue;
                if(!otherBarsQuiet) continue;
                //if(!slabsHit[0] || !slabsHit[1] || !slabsHit[2] || !slabsHit[3]) continue;
                if(!layersHit.at(3) || !layersHit.at(1) || !layersHit.at(2)) continue;
                if(barHits.size() < 3) continue;

              //  cout << "here" << endl;
                // check for straight paths in bars
                for(const auto firstHit : barHits) {
                        for(const auto secondHit : barHits) {
                                for(const auto thirdHit : barHits) {
                                        if(firstHit.layer != secondHit.layer && secondHit.layer != thirdHit.layer && firstHit.layer != thirdHit.layer) {
                                                isStraightPath = firstHit.row - secondHit.row == 0 && secondHit.row - thirdHit.row == 0 && firstHit.column - secondHit.column == 0 && secondHit.column - thirdHit.column ==0;

                                                for(int path=0; path<6; ++path) {
                                                        if(firstHit.channel == layer1_bars[path] && secondHit.channel == layer2_bars[path] && thirdHit.channel == layer3_bars[path]) straightPaths[path] = true;
                                                }                                                                                                                                                // paths
                                        }                                                                                                                         // if
                                }                                                                                                 // 3rd hit
                        }                                                                         //2nd hit
                }                                                 // 1st hit
                if(isStraightPath) ++straightBarEvents;

                if((*v_fit_xz).size() >= 2  && (*v_fit_yz).size() >=  2) {

                        //fill chanHS with channels hit by HS by the line fit
                        vector<double> parFit;

                        parFit.push_back((*v_fit_xz)[0]);
                        parFit.push_back((*v_fit_xz)[1]);
                        parFit.push_back((*v_fit_yz)[0]);
                        parFit.push_back((*v_fit_yz)[1]);

                        getPathLength(parFit, chanHS, path_length);
                        parFit.clear();
                }

                // check for straight bar AND hs events
                if(!isStraightPath) continue;
                int pulses_matched = 0;
                for(const auto h : barHits) {
                      for(unsigned int ihs = 0; ihs < chanHS.size(); ihs++) {
                              if(h.channel == chanHS.at(ihs)) pulses_matched++;
                      }
                }

                // check to see which pulses match
                for(unsigned int chan1 = 0; chan1 < chanHS.size(); ++chan1) {
                        for(unsigned int chan2 = 0; chan2 < chanHS.size(); ++chan2) {
                                for(unsigned int chan3 = 0; chan3 < chanHS.size(); ++chan3) {
                                        for(int path=0; path<6; ++path) {
                                                if(chanHS[chan1] == layer1_bars[path] && chanHS[chan2] == layer2_bars[path] && chanHS[chan3] == layer3_bars[path]) {
                                                        straightPathsHS[path] = true;
                                                }
                                        }
                                }
                        }
                }


                if(pulses_matched >= 3) {
                        ++straightBarAndHSevents;
                        matched = true;
                }

                //for straight events that were not able to matched due to
                //not enough hits in the HS, look to see where the HS was acutally hit,
                //and look if it was in the same row, col as the bars that were hit

                // physical coordinates of hits
                int nTop = 0;
                int nBottom =0;
                vector<float> hs_phys_coord_xTop = {};
                vector<float> hs_phys_coord_xBot = {};
                vector<float> hs_phys_coord_yTop = {};
                vector<float> hs_phys_coord_yBot = {};
                for(unsigned int j = 0; j < (*v_hs).size(); j++) {
                        vector<int> coord_index = convertRawToPhysCh((*v_hs)[j],true);
                        if(coord_index.size() < 3) continue;

                        if((*v_hs)[j] >= 48 && (*v_hs)[j] <= 63){
                          if(isStraightPath) ++nTop;
                          if(coord_index.at(0) != 0) hs_phys_coord_xTop.push_back(convertPhysChToCoord(coord_index, true)[0]);
                        }
                        if((*v_hs)[j] >= 0 && (*v_hs)[j] <= 15){
                            if(isStraightPath) ++nBottom;
                          if(coord_index.at(0) != 0) hs_phys_coord_xBot.push_back(convertPhysChToCoord(coord_index, true)[0]);
                        }
                        if((*v_hs)[j] >= 32 && (*v_hs)[j] <= 47){
                            if(isStraightPath) ++nTop;
                          if(coord_index.at(1) != 0) hs_phys_coord_yTop.push_back(convertPhysChToCoord(coord_index, true)[1]);
                        }
                        if((*v_hs)[j] >= 16 && (*v_hs)[j] <= 31){
                            if(isStraightPath) ++nBottom;
                          if(coord_index.at(1) != 0) hs_phys_coord_yBot.push_back(convertPhysChToCoord(coord_index, true)[1]);
                        }
                }
                tb->Fill(nBottom,nTop);

                // temp sanity check
                if(isStraightPath){

              }



                // convert bars to physical coords
                int bar_coord_x = 0;
                int bar_coord_y = 0;
                if(barHits.at(0).column == 1) bar_coord_x = 2.5;
                if(barHits.at(0).column == 2) bar_coord_x = -2.5;
                if(barHits.at(0).row == 1) bar_coord_y = 1;
                if(barHits.at(0).row == 2) bar_coord_y = 7;
                if(barHits.at(0).row == 3) bar_coord_y = 13;

                bool printChans = false;
                if((hs_phys_coord_xTop.size() > 2 || hs_phys_coord_yTop.size() > 2) && printChans) {
                        cout << "HS: ";
                        for(unsigned int iH=0; iH< (*v_hs).size(); ++iH) {
                                cout << " " << (*v_hs)[iH];
                        }
                        cout << endl << "Bar: ";
                        for(const auto h : barHits) {
                                cout << " " << h.channel;
                        }
                        cout << endl;
                }

                double distanceBetweenHorPacks = 180;
                double distanceBetweenVerPacks = 180;

                double pi = 3.1415;

                // Fill x plots
                if(hs_phys_coord_xBot.size() == 0 || hs_phys_coord_xTop.size() == 0) goto YCOORDS;


                if(hs_phys_coord_xBot.size() == hs_phys_coord_xTop.size() && hs_phys_coord_xBot.size() == 2){
                  if(hs_phys_coord_xBot[0] == hs_phys_coord_xBot[1]){
                    for(int ip=0; ip<6;++ip){
                      if(straightPaths[ip]){
                        angularDistX[ip]->Fill(atan((hs_phys_coord_xTop[0] - hs_phys_coord_xBot[0])/distanceBetweenHorPacks)*(180/pi));
                        cout << "1: " << hs_phys_coord_xTop[0] - hs_phys_coord_xBot[0] << " ";
                        cout << atan((hs_phys_coord_xTop[0] - hs_phys_coord_xBot[0])/distanceBetweenHorPacks)*(180/pi) << "             " << runNum << " " << fileNum << " " << event << endl;
                        //angularDistX[ip]->Fill();
                      }
                      angularDistX_notStraight[ip]->Fill(atan((hs_phys_coord_xTop[0] - hs_phys_coord_xBot[0])/distanceBetweenHorPacks)*(180/pi));
                    }


                    for(unsigned int ic=0;ic<hs_phys_coord_xBot.size(); ++ic) testCoords->Fill(hs_phys_coord_xBot[ic]);
                    for(unsigned int ic=0;ic<hs_phys_coord_xTop.size(); ++ic) testCoords->Fill(hs_phys_coord_xTop[ic]);

                  }
                }
                else if(hs_phys_coord_xBot.size() > hs_phys_coord_xTop.size() && hs_phys_coord_xBot.size() == 2){

                  if(hs_phys_coord_xBot[0] == hs_phys_coord_xBot[1]){
                    for(int ip=0; ip<6;++ip){
                      if(straightPaths[ip]){
                        angularDistX[ip]->Fill(atan((hs_phys_coord_xTop[0] - hs_phys_coord_xBot[0])/distanceBetweenHorPacks)*(180/pi));
                        cout << "2: " << hs_phys_coord_xTop[0] - hs_phys_coord_xBot[0] << " ";
                        cout << atan((hs_phys_coord_xTop[0] - hs_phys_coord_xBot[0])/distanceBetweenHorPacks)*(180/pi) << "               " << runNum << " " << fileNum << " " << event << endl;
                        //angularDistX[ip]->Fill();
                      }
                      angularDistX_notStraight[ip]->Fill(atan((hs_phys_coord_xTop[0] - hs_phys_coord_xBot[0])/distanceBetweenHorPacks)*(180/pi));
                    }



                    for(unsigned int ic=0;ic<hs_phys_coord_xBot.size(); ++ic) testCoords->Fill(hs_phys_coord_xBot[ic]);
                            for(unsigned int ic=0;ic<hs_phys_coord_xTop.size(); ++ic) testCoords->Fill(hs_phys_coord_xTop[ic]);
                            //for(unsigned int ic=0;ic<hs_phys_coord_yBot.size(); ++ic) testCoords->Fill(hs_phys_coord_yBot[ic]);
                            //for(unsigned int ic=0;ic<hs_phys_coord_yTop.size(); ++ic) testCoords->Fill(hs_phys_coord_yTop[ic]);
                 }
                }
                else if(hs_phys_coord_xBot.size() < hs_phys_coord_xTop.size() && hs_phys_coord_xTop.size() == 2){

                  if(hs_phys_coord_xTop[1] == hs_phys_coord_xTop[0]){
                    for(int ip=0; ip<6;++ip){
                      if(straightPaths[ip]){
                        angularDistX[ip]->Fill(atan((hs_phys_coord_xTop[0] - hs_phys_coord_xBot[0])/distanceBetweenHorPacks)*(180/pi));
                        cout << "3: " << hs_phys_coord_xTop[0] - hs_phys_coord_xBot[0] << " ";
                        cout << atan((hs_phys_coord_xTop[0] - hs_phys_coord_xBot[0])/distanceBetweenHorPacks)*(180/pi) << "             " << runNum << " " << fileNum << " " << event << endl;
                        //angularDistX[ip]->Fill();
                      }
                      angularDistX_notStraight[ip]->Fill(atan((hs_phys_coord_xTop[0] - hs_phys_coord_xBot[0])/distanceBetweenHorPacks)*(180/pi));
                    }



                    for(unsigned int ic=0;ic<hs_phys_coord_xBot.size(); ++ic) testCoords->Fill(hs_phys_coord_xBot[ic]);
                            for(unsigned int ic=0;ic<hs_phys_coord_xTop.size(); ++ic) testCoords->Fill(hs_phys_coord_xTop[ic]);
                            //for(unsigned int ic=0;ic<hs_phys_coord_yBot.size(); ++ic) testCoords->Fill(hs_phys_coord_yBot[ic]);
                            //for(unsigned int ic=0;ic<hs_phys_coord_yTop.size(); ++ic) testCoords->Fill(hs_phys_coord_yTop[ic]);
                 }
                }
                else if(hs_phys_coord_xBot.size() == hs_phys_coord_xTop.size() && hs_phys_coord_xTop.size() == 1){
                  for(int ip=0; ip<6;++ip){
                    if(straightPaths[ip]){
                      angularDistX[ip]->Fill(atan((hs_phys_coord_xTop[0] - hs_phys_coord_xBot[0])/distanceBetweenHorPacks)*(180/pi));
                      cout << "4: " << hs_phys_coord_xTop[0] - hs_phys_coord_xBot[0] << " ";
                      cout << atan((hs_phys_coord_xTop[0] - hs_phys_coord_xBot[0])/distanceBetweenHorPacks)*(180/pi) << "                " << runNum << " " << fileNum << " " << event << endl;
                      //angularDistX[ip]->Fill();
                    }
                    angularDistX_notStraight[ip]->Fill(atan((hs_phys_coord_xTop[0] - hs_phys_coord_xBot[0])/distanceBetweenHorPacks)*(180/pi));
                  }



                  for(unsigned int ic=0;ic<hs_phys_coord_xBot.size(); ++ic) testCoords->Fill(hs_phys_coord_xBot[ic]);
                          for(unsigned int ic=0;ic<hs_phys_coord_xTop.size(); ++ic) testCoords->Fill(hs_phys_coord_xTop[ic]);
                          //for(unsigned int ic=0;ic<hs_phys_coord_yBot.size(); ++ic) testCoords->Fill(hs_phys_coord_yBot[ic]);
                          //for(unsigned int ic=0;ic<hs_phys_coord_yTop.size(); ++ic) testCoords->Fill(hs_phys_coord_yTop[ic]);
                }


                // Fill y plots
                YCOORDS: if(hs_phys_coord_yBot.size() == 0 || hs_phys_coord_yTop.size() == 0) goto END;

                if(hs_phys_coord_yBot.size() == hs_phys_coord_yTop.size() && hs_phys_coord_yBot.size() == 2){
                  if(hs_phys_coord_yBot[0] == hs_phys_coord_yBot[1]){
                    for(int ip=0; ip<6;++ip){
                      if(straightPaths[ip]) angularDistY[ip]->Fill(atan((hs_phys_coord_yTop[0] - hs_phys_coord_yBot[0])/distanceBetweenVerPacks)*(180/pi));
                      angularDistY_notStraight[ip]->Fill(atan((hs_phys_coord_yTop[0] - hs_phys_coord_yBot[0])/distanceBetweenVerPacks)*(180/pi));
                    }
                  }
                }
                else if(hs_phys_coord_yBot.size() > hs_phys_coord_yTop.size() && hs_phys_coord_yBot.size() == 2){

                  if(hs_phys_coord_yBot[0] == hs_phys_coord_yBot[1]){
                    for(int ip=0; ip<6;++ip){
                      if(straightPaths[ip]) angularDistY[ip]->Fill(atan((hs_phys_coord_yTop[0] - hs_phys_coord_yBot[0])/distanceBetweenVerPacks)*(180/pi));
                      angularDistY_notStraight[ip]->Fill(atan((hs_phys_coord_yTop[0] - hs_phys_coord_yBot[0])/distanceBetweenVerPacks)*(180/pi));
                    }
                 }
                }
                else if(hs_phys_coord_yBot.size() < hs_phys_coord_yTop.size() && hs_phys_coord_yTop.size() == 2){

                  if(hs_phys_coord_yTop[1] == hs_phys_coord_yTop[0]){
                    for(int ip=0; ip<6;++ip){
                      if(straightPaths[ip]) angularDistY[ip]->Fill(atan((hs_phys_coord_yTop[0] - hs_phys_coord_yBot[0])/distanceBetweenVerPacks)*(180/pi));
                      angularDistY_notStraight[ip]->Fill(atan((hs_phys_coord_yTop[0] - hs_phys_coord_yBot[0])/distanceBetweenVerPacks)*(180/pi));
                    }
                 }
                }
                else if(hs_phys_coord_yBot.size() == hs_phys_coord_yTop.size() && hs_phys_coord_yTop.size() == 1){
                  for(int ip=0; ip<6;++ip){
                    if(straightPaths[ip]) angularDistY[ip]->Fill(atan((hs_phys_coord_yTop[0] - hs_phys_coord_yBot[0])/distanceBetweenVerPacks)*(180/pi));
                    angularDistY_notStraight[ip]->Fill(atan((hs_phys_coord_yTop[0] - hs_phys_coord_yBot[0])/distanceBetweenVerPacks)*(180/pi));
                  }
                }

                END: ;
                hs_phys_coord_xTop.clear();
                hs_phys_coord_xBot.clear();
                hs_phys_coord_yTop.clear();
                hs_phys_coord_yBot.clear();
                barHits.clear();
                chanHS.clear();
                path_length.clear();

                hsHits->Fill((*v_hs).size());

        } // end chain loop

        cout << "Bar: " << straightBarEvents << endl;
        cout << "HS: " << straightHSevents << endl;
        cout << "Bar & HS: " << straightBarAndHSevents << endl;
        cout << "Weak Bar & HS: " << weak_straightBarAndHSevents << endl;
        cout << "Weak Efficiency: " << (weak_straightBarAndHSevents+straightBarAndHSevents*1.0 ) / straightBarEvents;
        cout << " +- " << sqrt( ((weak_straightBarAndHSevents+straightBarAndHSevents)/(pow(straightBarEvents,2)) ) + ( (pow(weak_straightBarAndHSevents+straightBarAndHSevents,2)) / (pow(straightBarEvents,3)))) << endl;

        gStyle->SetOptStat(0);
        gStyle->SetPalette(56); // kDarkBodyRadiator palette

        TFile* outputFile = new TFile("angPlot_beam.root", "RECREATE");
        for(int i = 0; i < 6; i++) {
                angularDistX[i]->SetOption("E");
                angularDistX[i]->Write();
                angularDistY[i]->SetOption("E");
                angularDistY[i]->Write();
                totalAngularX->Add(angularDistX[i]);
                totalAngularY->Add(angularDistY[i]);

                angularDistX_notStraight[i]->SetOption("E");
                angularDistX_notStraight[i]->Write();
                angularDistY_notStraight[i]->SetOption("E");
                angularDistY_notStraight[i]->Write();
                totalAngularX_notStraight->Add(angularDistX_notStraight[i]);
                totalAngularY_notStraight->Add(angularDistY_notStraight[i]);
        }
        totalAngularX->SetOption("E");
        totalAngularX->Write();
        totalAngularY->SetOption("E");
        totalAngularY->Write();

        totalAngularX_notStraight->SetOption("E");
        totalAngularX_notStraight->Write();
        totalAngularY_notStraight->SetOption("E");
        totalAngularY_notStraight->Write();


        tb->SetOption("colz");
        tb->Write();


        testCoords->SetOption("e");
        testCoords->Write();


        hsHits->Write();

        outputFile->Close();



        TCanvas * c1 = new TCanvas("c1");
        c1->cd();
        tb->Draw("colz");
        tb->SaveAs("tb2.pdf");

} // end function
