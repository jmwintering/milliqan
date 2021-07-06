#include "treesV16TemplateTest.h"

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

void slabEfficiencyMethod(string fileToAdd = "", string runToAdd =""){
  cout << "Running slabEfficiencyMethod.cpp..." << endl;
  fileToAdd = "/data/barEfficiency_4hshits/*.root";
  runToAdd = "All";
  string tag = "slabs";
  bool weighted = false;
  cout << "WEIGHTING: " << weighted << endl;
  

  vector<string> badRuns_updated = {
    " 1066 " ,  " 1080 " ,  " 1100 " ,  " 1101 " ,  " 1286 " ,  " 1378 " ,  " 1379 " ,  " 1477 " ,  " 1478" ,  " 1479 " ,  " 1482 " ,  " 1485 " ,  " 1488 " ,  " 1491 " ,  " 1494 " ,  " 1511 " ,  " 1518 " ,  " 1526 " ,  " 1533 " ,  " 1540 " ,  " 1547 " ,  " 1550",
    " 1551 " ,  " 1552 " ,  " 1553 " ,  " 1557 " ,  " 1560 " ,  " 1563 " ,  " 1566 " ,  " 1569 " ,  " 1572 " ,  " 1579 " ,  " 1586 " ,  " 1593 " ,  " 1600" ,  " 1602 " ,  " 1604 " ,  " 1625 " ,  " 1336 " ,  " 1369 " ,  " 1370 " ,  " 1428 " ,  " 1429 " ,  " 1451 " ,  " 1452 " ,  " 1546 " ,  " 1578 " ,  " 1757 " ,  " 1770 " ,  " 1813 " ,
    " 1072 " ,  " 1077 "  ,  " 1361 " ,  " 1458 " ,  " 1460 " ,  " 1473 " ,  " 1475 " ,  " 1497 " ,  " 1501 " ,  " 1516 " ,  " 1531 " ,  " 1556 " ,  " 1559 " ,  " 1565 " ,  " 1568 " ,  " 1596 " ,  " 1718 " ,  " 1729 " ,  " 1752 " ,  " 1779 " ,  " 1784 " ,  " 1787 " ,  " 1800 " ,  " 1818 " ,  " 1117 " ,  " 1288 " ,  " 1303 " ,  " 1398 " ,  " 1422 " ,  " 1424 " ,  " 958 " ,  " 961 " ,
    " 970 " ,  " 979 " ,  " 986 " ,  " 993 " ,  " 994 " 
	};

	TChain * chain = new TChain("t");
	if(fileToAdd == "" || runToAdd == "") {cerr<< "Error please enter file to add " << endl; return;}
	chain->Add(toTstring(fileToAdd));
    //chain->Add("/data/users/wintering/methodFiles/slabskims/*.root");
	SetAddressesChainHS(chain);
   
	vector<int> layer1_bars = {0,1,24,25,8,9};
	vector<int> layer2_bars = {6,7,16,17,12,13};
	vector<int> layer3_bars = {2,3,22,23,4,5};
	fillBars();

	vector<int> slabs = {18, 20, 28, 21};
    vector<double> muonBarCuts(32, 400);
    vector<double> cosmicBarCuts(32, 0);
    cosmicBarCuts[0] = 2900;
    cosmicBarCuts[1] = 2700;
    cosmicBarCuts[2] = 2500;
    cosmicBarCuts[3] = 2500;
    cosmicBarCuts[4] = 2800;
    cosmicBarCuts[5] = 900;
    cosmicBarCuts[6] = 3400;
    cosmicBarCuts[7] =  2800;
    cosmicBarCuts[8] = 2500;
    cosmicBarCuts[9] = 1400;
    cosmicBarCuts[12] = 2900;
    cosmicBarCuts[13] = 2000;
    cosmicBarCuts[16] = 5700;
    cosmicBarCuts[17] = 2000;
    cosmicBarCuts[22] = 650;
    cosmicBarCuts[23] = 3000;
    cosmicBarCuts[24] = 2000;
    cosmicBarCuts[25] = 3600;
            
    
    //18, 20, 28, 21
    vector<double> slabCuts(4,0);
	slabCuts[0] = 300;
	slabCuts[1] = 350;
	slabCuts[2] = 150;
	slabCuts[3] = 350;
            


	int entries = chain->GetEntries();
	cout << "added " << entries << " entries" << endl;

	for(int iE=0; iE<entries; ++iE) {
        //if(iE > 5) continue;
		chain->GetEntry(iE);
        displayProgress(iE,entries);

		// bool badRun = false;

		// for(unsigned ibad=0; ibad<badRuns_updated.size(); ++ibad){
        //     if(tostr(runNum) == badRuns_updated[ibad]){
        //         cout << "woah bad run" << endl;
        //         badRun = true;
        //     }
		// }

        //beam
        //if(!beam) continue;
        vector<double> barCuts(32,0);
        if(beam) barCuts = muonBarCuts;
        else barCuts = cosmicBarCuts; 

		//consider only events with HS data
        bool skipEvent = false;
        for(unsigned int j = 0; j < (*v_hs).size(); j++) {
            if((*v_hs)[j] == -1) skipEvent = true;
        }
        if(skipEvent) continue;

        if(runNum > 1605){
            continue;
        }

        vector<float> hs_phys_coord_xTop = {};
		vector<float> hs_phys_coord_xBot = {};
		vector<float> hs_phys_coord_yTop = {};
		vector<float> hs_phys_coord_yBot = {};
		for(unsigned int j = 0; j < (*v_hs).size(); j++) {
			vector<int> coord_index = convertRawToPhysCh((*v_hs)[j],true);
			if(coord_index.size() < 3) continue;
			if((*v_hs)[j] >= 48 && (*v_hs)[j] <= 63){ 
				if(coord_index.at(0) != 0) hs_phys_coord_xTop.push_back(convertPhysChToCoord(coord_index, true)[0]);
			}
			if((*v_hs)[j] >= 0 && (*v_hs)[j] <= 15){ 
			    if(coord_index.at(0) != 0) hs_phys_coord_xBot.push_back(convertPhysChToCoord(coord_index, true)[0]);
			}
			if((*v_hs)[j] >= 32 && (*v_hs)[j] <= 47){ 
				if(coord_index.at(1) != 0) hs_phys_coord_yTop.push_back(convertPhysChToCoord(coord_index, true)[1]);
			}
			if((*v_hs)[j] >= 16 && (*v_hs)[j] <= 31){ 
			    if(coord_index.at(1) != 0) hs_phys_coord_yBot.push_back(convertPhysChToCoord(coord_index, true)[1]);
			}
		}
	
        bool clustered = true;
        // for(auto h1:hs_coord_x){
        //     for(auto h2: hs_coord_x){
        //         if(abs(h2-h1) > 3) clustered = false;
        //     }
        // }
        //  for(auto h1:hs_coord_y){
        //     for(auto h2: hs_coord_y){
        //         if(abs(h2-h1) > 3) clustered = false;
        //     }
        // }
        

        double sheetNPE = 0;
        vector<int> slabsHit = {};
        for(int ipulse=0; ipulse<(*v_ipulse).size(); ++ipulse){
            if((*v_ipulse)[ipulse]!=0) continue;
            if((*v_type)[ipulse] == 2) sheetNPE += (*v_nPE)[ipulse];
            if((*v_type)[ipulse] == 1) {
                for(unsigned int j = 0; j < slabs.size(); j++) {
                    if(slabs[j] == (*v_chan)[ipulse] && (*v_nPE)[ipulse] > slabCuts[j]) slabsHit.push_back((*v_chan)[ipulse]);
                }
            }
           
        }
        /* CUTS  */
        if(slabsHit.size()<3) continue;
        if(sheetNPE > 100) continue;
        if(hs_phys_coord_xBot.size() !=2 || hs_phys_coord_xTop.size() !=2 || hs_phys_coord_yBot.size() !=2 || hs_phys_coord_yTop.size() !=2) continue;


        vector<int> chanHS = {};
        vector<double> path_length= {};
        vector<double> parFit= {};
        bool mathematicaRequired = false;

        parFit = doCircleFitInverted(v_hs);
        getCirclePathLengthInverted(parFit, chanHS, path_length, runNum, fileNum, event, "ellipFiles/"+tag+runToAdd, mathematicaRequired, weighted);
        //getCirclePathLengthInverted(parFit, chanHS, path_length); 

        // if(runNum ==  1336  && fileNum == 488 && event == 822){
        //     cout  << endl << "par ";
        //     for(const auto p:parFit){
        //         cout << p << " ,";
        //     }
        //     cout << endl;
        //     cout << "chan ";
        //     for(const auto c:(*v_chan)) cout << c << " ,";
        //     cout << endl;

        //     cout << "hs ";
        //     for(const auto c:(*v_hs)) cout << c << " ,";
        //     cout << endl;
        // }
        parFit.clear();
	}
	bars.clear();
    cout << "Wrote results to " <<  "ellipFiles/" << tag << runToAdd << ".txt, " << "ellipFiles/" << tag <<  runToAdd << "noMath.txt " << endl;

} // end function
