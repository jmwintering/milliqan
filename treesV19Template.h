#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <string>
#include <cmath>
#include <utility>
#include <map>
#include <stdio.h>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <typeinfo>

#include "TROOT.h"
#include "TSystem.h"
#include "TRandom.h"
#include "TRandom3.h"
#include "TF1.h"
#include "TFile.h"
#include "TVirtualFFT.h"
#include "TTree.h"
#include "TChain.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TBranch.h"
#include "TString.h"
#include "TStyle.h"
#include "TPaveStats.h"
#include "TLatex.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TGaxis.h"
#include "TLegend.h"
#include "TColor.h"
#include "TComplex.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraphAsymmErrors.h"

#include "utilities.h"
#include "corrections.h"
#include "msl.hpp"

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

using namespace std;

// Set addresses of ROOT tree branches

int nRollOvers=0;
int runNumOrig=0;
int event = 0;
int fileNum=0;
int runNum=0;
int fileID = 0;
Long64_t event_time_b0=0;
Long64_t event_time_b1=0;
double event_time_fromTDC=0;
bool present_b0;
bool present_b1;
int t_since_fill_start=0;
int t_since_fill_end=0;
int t_until_next_fill=0;
string * event_t_string;
Long64_t event_trigger_time_tag_b0;
Long64_t event_trigger_time_tag_b1;
int fillNum = 0;
float fillAvgLumi=0;
float fillTotalLumi=0;
bool beam;
bool hardNoBeam;
float chan_muDist[32];
int chan_trueNPE[32];
float scale1fb=0;
int orig_evt=0;
bool mcTruth_threeBarLine;
bool mcTruth_fourSlab;

std::vector<int> * v_npulses = new std::vector<int>();
std::vector<int> * v_ipulse = new std::vector<int>();
std::vector<int> * v_chan = new std::vector<int>();
std::vector<int> * v_layer = new std::vector<int>();
std::vector<int> * v_row = new std::vector<int>();
std::vector<int> * v_column = new std::vector<int>();
std::vector<int> * v_type = new std::vector<int>();
std::vector<float> * v_height = new std::vector<float>();
std::vector<float> * v_time = new std::vector<float>();
std::vector<float> * v_triggerCandidates = new std::vector<float>();
std::vector<float> * v_triggerCandidatesEnd = new std::vector<float>();
std::vector<int> * v_triggerCandidatesChannel = new std::vector<int>();
std::vector<float> * v_time_module_calibrated = new std::vector<float>();
std::vector<float> * v_delay = new std::vector<float>();
std::vector<float> * v_area = new std::vector<float>();
std::vector<float> * v_nPE = new std::vector<float>();
std::vector<float> * v_duration =  new std::vector<float>();
std::vector<bool> * v_quiet = new std::vector<bool>();
std::vector<float> * v_presample_mean = new std::vector<float>();
std::vector<float> * v_presample_RMS = new std::vector<float>();
std::vector<float> * v_sideband_mean = new std::vector<float>();
std::vector<float> * v_sideband_RMS = new std::vector<float>();
std::vector<Long64_t> * v_groupTDC_b0 = new std::vector<Long64_t>();
std::vector<Long64_t> * v_groupTDC_b1 = new std::vector<Long64_t>();
std::vector<float> * v_bx = new std::vector<float>();
std::vector<float> * v_by = new std::vector<float>();
std::vector<float> * v_bz = new std::vector<float>();
std::vector<float> * v_max = new std::vector<float>();
std::vector<float> * v_min = new std::vector<float>();
std::vector<float> * v_triggerBand_mean = new std::vector<float>();
std::vector<float> * v_triggerBand_max = new std::vector<float>();
std::vector<float> * v_triggerBand_maxTime = new std::vector<float>();
std::vector<float> * v_triggerBand_RMS = new std::vector<float>();
std::vector<float> * v_sideband_mean_calib = new std::vector<float>();
std::vector<float> * v_sideband_RMS_calib = new std::vector<float>();
std::vector<float> * v_maxAfterFilter = new std::vector<float>();
std::vector<float> * v_maxThreeConsec = new std::vector<float>();
std::vector<float> * v_minAfterFilter = new std::vector<float>();
std::vector<float> * v_sideband_mean_perFile = new std::vector<float>();
std::vector<float> * v_triggerThreshold = new std::vector<float>();
std::vector<bool> * v_triggerEnable = new std::vector<bool>();
std::vector<int> * v_triggerMajority = new std::vector<int>();
std::vector<int> * v_triggerLogic = new std::vector<int>();

// nPE corrections directly from Matthew
std::vector<float> * v_nPECorr = new std::vector<float> ();

// hodoscope data
int extrg;
double hs_time;
std::vector<int> * v_hs = new std::vector<int>();
std::vector<int> * v_tp = new std::vector<int>();
std::vector<double> * v_fit_xz = new std::vector<double>();
std::vector<double> * v_fit_yz = new std::vector<double>();

// function to set branch addresses for everything above
template <class T> 
int SetAddresses(T *& tree){

	bool hodoscopeDataPresent = false; 
	bool simDataPresent = false; 
	auto hodoscopeBranchesFound = (tree->T::FindBranch("hs"))? true:false;
	auto correctionBranchFound = (tree->T::FindBranch("nPECorr"))? true:false;
	auto simBranchesFound = (tree->T::FindBranch("scale1fb"))? true:false;


	tree->SetBranchAddress("triggerBand_mean", &v_triggerBand_mean);
	tree->SetBranchAddress("triggerBand_max", &v_triggerBand_max);
	tree->SetBranchAddress("triggerBand_maxTime", &v_triggerBand_maxTime);
	tree->SetBranchAddress("triggerBand_RMS", &v_triggerBand_RMS);
	tree->SetBranchAddress("sideband_mean_calib", &v_sideband_mean_calib);
	tree->SetBranchAddress("sideband_RMS_calib", &v_sideband_RMS_calib);
	tree->SetBranchAddress("max", &v_max);
	tree->SetBranchAddress("min", &v_min);
	tree->SetBranchAddress("maxAfterFilter", &v_maxAfterFilter);
	tree->SetBranchAddress("minAfterFilter", &v_minAfterFilter);
	tree->SetBranchAddress("maxThreeConsec", &v_maxThreeConsec);
	tree->SetBranchAddress("hardNoBeam", &hardNoBeam);
	tree->SetBranchAddress("t_until_next_fill", &t_until_next_fill);
	tree->SetBranchAddress("event_t_string", &event_t_string);
	tree->SetBranchAddress("event", &event);
	tree->SetBranchAddress("run", &runNum);
	tree->SetBranchAddress("file", &fileNum);
	tree->SetBranchAddress("nRollOvers", &nRollOvers);
	tree->SetBranchAddress("beam", &beam);
	tree->SetBranchAddress("fillAvgLumi", &fillAvgLumi);
	tree->SetBranchAddress("fillTotalLumi", &fillTotalLumi);
	tree->SetBranchAddress("event_time_b0", &event_time_b0);
	tree->SetBranchAddress("event_time_b1", &event_time_b1);
	tree->SetBranchAddress("t_since_fill_start", &t_since_fill_start);
	tree->SetBranchAddress("t_since_fill_end", &t_since_fill_end);
	tree->SetBranchAddress("file", &fileNum);
	tree->SetBranchAddress("fill", &fillNum);
	tree->SetBranchAddress("event_trigger_time_tag_b0", &event_trigger_time_tag_b0);
	tree->SetBranchAddress("event_trigger_time_tag_b1", &event_trigger_time_tag_b1);
	tree->SetBranchAddress("present_b0", &present_b0);
	tree->SetBranchAddress("present_b1", &present_b1);
	tree->SetBranchAddress("triggerCandidates", &v_triggerCandidates);
	tree->SetBranchAddress("triggerCandidatesEnd", &v_triggerCandidatesEnd);
	tree->SetBranchAddress("triggerCandidatesChannel", &v_triggerCandidatesChannel);
	tree->SetBranchAddress("chan", &v_chan);
	tree->SetBranchAddress("layer", &v_layer);
	tree->SetBranchAddress("row", &v_row);
	tree->SetBranchAddress("column", &v_column);
	tree->SetBranchAddress("type", &v_type);
	tree->SetBranchAddress("height", &v_height);
	tree->SetBranchAddress("time_module_calibrated", &v_time_module_calibrated);
	tree->SetBranchAddress("time", &v_time);
	tree->SetBranchAddress("delay", &v_delay);
	tree->SetBranchAddress("area", &v_area);
	tree->SetBranchAddress("nPE", &v_nPE);
	tree->SetBranchAddress("ipulse", &v_ipulse);
	tree->SetBranchAddress("npulses", &v_npulses);
	tree->SetBranchAddress("duration", &v_duration);
	tree->SetBranchAddress("quiet", &v_quiet);
	tree->SetBranchAddress("presample_mean", &v_presample_mean);
	tree->SetBranchAddress("presample_RMS", &v_presample_RMS);
	tree->SetBranchAddress("sideband_mean", &v_sideband_mean);
	tree->SetBranchAddress("sideband_RMS", &v_sideband_RMS);
	tree->SetBranchAddress("groupTDC_b0", &v_groupTDC_b0);
	tree->SetBranchAddress("groupTDC_b1", &v_groupTDC_b1);
	tree->SetBranchAddress("event_time_fromTDC", &event_time_fromTDC);
	tree->SetBranchAddress("bx", &v_bx);
	tree->SetBranchAddress("by", &v_by);
	tree->SetBranchAddress("bz", &v_bz);

	// simulation branches
	if(simBranchesFound){
		tree->SetBranchAddress("sideband_mean_perFile",&v_sideband_mean_perFile);
		tree->SetBranchAddress("triggerThreshold",&v_triggerThreshold);
		tree->SetBranchAddress("triggerEnable",&v_triggerEnable);
		tree->SetBranchAddress("triggerMajority",&v_triggerMajority);
		tree->SetBranchAddress("triggerLogic",&v_triggerLogic);
		tree->SetBranchAddress("chan_muDist",&chan_muDist);
		tree->SetBranchAddress("chan_trueNPE",&chan_trueNPE);
		tree->SetBranchAddress("scale1fb",&scale1fb);
		tree->SetBranchAddress("orig_evt",&orig_evt);
		tree->SetBranchAddress("mcTruth_threeBarLine", &mcTruth_threeBarLine);
		tree->SetBranchAddress("mcTruth_fourSlab", &mcTruth_fourSlab);
	}

	// hodoscope data (including nPE corrections branch
	if(hodoscopeBranchesFound){
		tree->SetBranchAddress("extrg", &extrg);
		tree->SetBranchAddress("hs_time", &hs_time);
		tree->SetBranchAddress("hs", &v_hs);
		tree->SetBranchAddress("tp", &v_tp);
		tree->SetBranchAddress("nPECorr", &v_nPECorr);
	}

	return tree->T::GetEntries();
}


void ClearVectors() {

	v_max->clear();
	v_maxAfterFilter->clear();
	v_maxThreeConsec->clear();
	v_min->clear();
	v_minAfterFilter->clear();
	v_chan->clear();
	v_triggerCandidates->clear();
	v_triggerCandidatesEnd->clear();
	v_triggerCandidatesChannel->clear();
	v_layer->clear();
	v_row->clear();
	v_column->clear();
	v_type->clear();
	v_height->clear();
	v_time->clear();
	v_time_module_calibrated->clear();
	v_area->clear();
	v_nPE->clear();
	v_ipulse->clear();
	v_npulses->clear();
	v_duration->clear();
	v_delay->clear();
	v_sideband_mean->clear();
	v_sideband_RMS->clear();
	v_triggerBand_mean->clear();
	v_triggerBand_max->clear();
	v_triggerBand_maxTime->clear();
	v_triggerBand_RMS->clear();
	v_sideband_mean_calib->clear();
	v_sideband_RMS_calib->clear();
	v_quiet->clear();
	v_presample_mean->clear();
	v_presample_RMS->clear();
	v_groupTDC_b0->clear();
	v_groupTDC_b1->clear();
	v_bx->clear();
	v_by->clear();
	v_bz->clear();
	v_sideband_mean_perFile->clear();
	v_triggerThreshold->clear();
	v_triggerEnable->clear();
	v_triggerMajority->clear();
	v_triggerLogic->clear();

	// hodoscope
	v_hs->clear();
	v_tp->clear();
	v_fit_xz->clear();
	v_fit_yz->clear();
}


//========================================================================================================================
// USEFUL FUNCTIONS
//========================================================================================================================

int LAYERS=3;
int HEIGHT=3;
int WIDTH=2;
int LEFT=0;
int RIGHT=1;
int BOTTOM=0;
int MIDDLE=1;
int TOP=2;
int NUM_CHAN = 32;
enum typeList {kBar = 0, kSlab = 1, kSheet = 2};

std::vector<int> slabs = {18,20,28,21};
std::vector<int> allBars = {0,1,24,25,8,9,6,7,16,17,12,13,2,3,22,23,4,5};

//first index is layer = 0,1,2
//second index is L=0,T=1,R=2
int sheetMap[3][3] = {
	{ 27, 10, 29 },
	{ 11, 30, 19 },
	{ 31, 14, 26 }
};

//second index is F=0, B=1       "Front" is defined to be toward the IP
int slabMap[3][2] = {
	{ 18, 20 },
	{ 20, 28 },
	{ 28, 21 }
};

int channelMap[3][2][3] = {
  { {8, 24,0}, {9, 25, 1} },
  { {12, 16, 6}, {13, 17, 7} },
  { {4, 22, 2}, {5, 23, 3} }
};

std::vector<string> tubeTypes = {
	"878","878","878","878",            
	"878","7725","878","878",       
	"878","ET","878","878", 
	"878","878","878","878",   
	"878","ET","878","878",
	"878","878","7725","878",   
	"ET","ET","878","878",  
	"878","878","878","878"
};

std::vector<int> getInfo(int ch, bool fourLayer=false){
	
	int type = -1;
	int layer = -1;
	int column = -1;
	int row = -1;

	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 2; j++){
			for(int k = 0; k < 3; k++){
				if(ch == channelMap[i][j][k]){
					type = 0;
					layer = i+1;
					column = j;
					row = k;
				}
			}
		}
	}

	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			if(ch == sheetMap[i][j]){
				type = 2;
				layer = i+1;
			}
		}
	}

	for(int i = 0; i < 4; i++){
		if(ch == slabs[i]){
			type = 1;
			layer = i;
		}
	}
	
	if(fourLayer){
		if(ch == 28 || ch == 20 || ch == 15){
			type = kBar;
			layer = 4;	
		}
	}

	std::vector<int> info(4,0);
	info[0] = type;
	info[1] = layer;
	info[2] = column;
	info[3] = row;

	return info;
}


int tubeSpecies(int const ch){

	string tube = tubeTypes[ch];

	if(tube == "878"){ return 0; }
	else if(tube == "ET"){ return 1; }
	else if(tube == "7725"){ return 2; }
	else{ return -1; }
}

	
int percentDone = 0;
auto firstTime = chrono::system_clock::now();
bool isFirstTime = true; 
void displayProgress(int i, int entries){

	int updateFreqPercent = 1;
	if(isFirstTime) {
		firstTime = chrono::system_clock::now();
		isFirstTime = false;
	}

	std::chrono::duration<double> timeTaken;
	double timeLeft = -1;

	int currentPercent = static_cast<int>(((i*1.0)/(entries-1))*100);
	
	if(currentPercent > percentDone){

		if(percentDone % updateFreqPercent == 0){
			
			auto currentTime = chrono::system_clock::now();
			timeTaken = currentTime - firstTime;

			timeLeft = (timeTaken.count()*(100-percentDone)*1.0)/percentDone;
		}

		if(percentDone < updateFreqPercent) cout << BOLDGREEN << "\r[" << currentPercent << "% ]" << RESET << std::flush;
		else cout << BOLDGREEN << "\r[" << currentPercent << "% (" << i << "/" << entries << ")]" << " - ETA: " << timeLeft << "s" << RESET << std::flush;
	}

	percentDone = currentPercent;
}

Long64_t GetTreeIndex(TChain *tree, int runVar, int fileVar, int eventVar, int entries){
	auto nentries = entries;
	TString draw_selection = TString::Format("run==%d && file==%d && event==%d",runVar,fileVar, eventVar);
	TString draw_command = TString::Format("Entry$ >> hist(%d,0,%d)",nentries,nentries);
	tree->Draw(draw_command,draw_selection,"goff");
	TH1I *hist = (TH1I*)gDirectory->Get("hist");
	Long64_t iEntry = hist->GetBinLowEdge(hist->FindFirstBinAbove(0));
	delete hist;
	return iEntry;
}

std::vector<double> findTimeExtrema(std::vector<double> const time, bool slabs, std::vector<int> layer={}){

	double maxDeltaT = -1;
	double minDeltaT = 1e9;

	std::vector<double> deltaTimes = {};
	if(!slabs){
		for(int iTime=0u; iTime < time.size(); ++iTime){
			for(int jTime = 0u; jTime < time.size(); ++jTime){
				if(iTime == jTime) continue;

				if(layer[iTime] > layer[jTime]){
					deltaTimes.push_back(time[iTime] - time[jTime]) ;
				}
				else if(layer[iTime] < layer[jTime]){
					deltaTimes.push_back(time[jTime] - time[iTime]) ;
				}

			}
		}
	}
	else{
		for(int iTime= 0u; iTime < time.size(); ++iTime){
			for(int jTime = 0u; jTime < time.size(); ++jTime){
				if(iTime == jTime) continue;
				deltaTimes.push_back(time[iTime] - time[jTime]) ;
			}
		}
	}

	std::vector<double> deltaTimeAbs = {};
	for(int j = 0; j < deltaTimes.size(); j++){
		deltaTimeAbs.push_back(fabs(deltaTimes[j]));
	}

	maxDeltaT = msl::maxElement<double>(deltaTimeAbs);
	minDeltaT = msl::minElement<double>(deltaTimeAbs);
	
	int maxIndex = -1;
	int minIndex =-1;
	for(int j = 0u; j < deltaTimeAbs.size(); j++){
		if(deltaTimeAbs[j] == maxDeltaT) maxIndex = j;
		if(deltaTimeAbs[j] == minDeltaT) minIndex = j;
	}

	maxDeltaT = deltaTimes[maxIndex];
	minDeltaT = deltaTimes[minIndex];

	return  {minDeltaT, maxDeltaT};
}



double correctTime(double const oldTime, double const area, int const tubeSpecies, bool signal){
	if(tubeSpecies < 0){ return oldTime; }

	TRandom3 * smear  = new TRandom3(0);
	
	std::vector<float> t_corrections = data_t_corrs[tubeSpecies];
	if(signal) std::vector<float> t_corrections = mc_t_corrs[tubeSpecies];
	
	double time = oldTime;

	for(int iBin = 0u; iBin < area_bins.size(); iBin++){

		if(iBin == area_bins.size()-1){
			if(area >= area_bins[iBin]){
				time += t_corrections[iBin];
				if(signal) time += smear->Gaus(0, smear_corrs[tubeSpecies][iBin]);
			}
		}
		else{
			if(area >= area_bins[iBin] && area < area_bins[iBin+1]){
				time += t_corrections[iBin];
				if(signal) time += smear->Gaus(0, smear_corrs[tubeSpecies][iBin]);
			}
		}

		if(iBin == 0){
			if(area < area_bins[iBin]){
				time += t_corrections[iBin];
				if(signal) time += smear->Gaus(0, smear_corrs[tubeSpecies][iBin]);
			}
		}
	} 

	delete smear;
	return time;
}


double correctTimeFourthLayer(double const oldTime, int const channel){

	double time = -1;
	if(channel == 20) time = oldTime + 8.125;
	else if(channel == 28) time = oldTime + 19.375;
	else if(channel == 15) time = oldTime + 30.0;
	
	return time;
}


//========================================================================================================================
// PLOTTING
//========================================================================================================================

std::vector<TH1*> findHigherHistogram(std::vector<TH1*> histos){
    auto maxBinHeight = histos[0]->GetBinContent(1);
    int maxHistoIndex = 0;

    for(int iBin = 1; iBin <= histos[0]->GetNbinsX(); ++iBin){
        if(histos[0]->GetBinContent(iBin) > maxBinHeight) maxBinHeight = histos[0]->GetBinContent(iBin);
    }

    for(int iH=1; iH < histos.size(); ++iH){
		for(int iBin = 1; iBin <= histos[iH]->GetNbinsX(); ++iBin){
        	if(histos[iH]->GetBinContent(iBin) > maxBinHeight){
				maxHistoIndex = iH;
				break;
			}
   		}
	}

    std::vector<TH1*> reorderedHistos = {};
	reorderedHistos.push_back(histos[maxHistoIndex]);
	for(int iH=0; iH < histos.size(); ++iH){
		if(iH == maxHistoIndex) continue;
		reorderedHistos.push_back(histos[iH]);
	}

	return reorderedHistos;
}


void plot1DHistograms(std::vector<TH1*> histos, TString plotName="",
					string plotOptions = "", TLegend * userLegend=NULL,
					TString fitType = "expo", TString fitOptions = "", std::vector<double> fitRange={0,0},
					int colorPalette=55){

	bool drawLegend(false), logx(false), logy(false), logz(false), showMean(false), dotted(false), errorBars(false);
	drawLegend = plotOptions.find("l") != std::string::npos ? true:false;
	logx = plotOptions.find("x") != std::string::npos ? true:false;
	logy = plotOptions.find("y") != std::string::npos ? true:false;
	logz = plotOptions.find("z") != std::string::npos ? true:false;
	showMean = plotOptions.find("m") != std::string::npos ? true:false;
	dotted = plotOptions.find("d") != std::string::npos ? true:false;
	errorBars = plotOptions.find("e") != std::string::npos ? true:false;

	bool fitHisto = fitOptions != "";
	TF1 * f1 = new TF1("f1", fitType, fitRange[0], fitRange[1]);
	if(fitHisto) f1->SetLineWidth(4);

	bool styleByPalette = histos.size() >= 7;
	std::vector<int> lineColors = {4, 2, 1, 8, 38, 617};

	gStyle->SetOptStat(1);
	gStyle->SetPalette(colorPalette);
	TString drawOption = "";
	if(errorBars) drawOption = "e";
	std::vector<TH1*> rHistos = histos;

	if(dotted) rHistos[1]->SetLineStyle(2);

	for(int i=0; i < rHistos.size(); ++i){
		if(!styleByPalette) rHistos[i]->SetLineColor(lineColors[i]);
	}

	if(histos.size() > 1){
		if(styleByPalette) drawOption += " PLC PMC";
		rHistos = findHigherHistogram(histos);
	}

	TCanvas * cPlot = new TCanvas("cPlot");
	cPlot->cd();
	if(logx) cPlot->SetLogx();
	if(logy) cPlot->SetLogy();
	if(logz) cPlot->SetLogz();

	cPlot->SetCanvasSize(2100,1500);

	double offsetx = 0.25;
	double offsety = 0.07;
	auto * legend = new TLegend(0.35-offsetx, 0.75-offsety, 0.35+offsetx, 0.75+offsety);
	legend->SetBorderSize(0);

	if(userLegend != NULL){
		legend = userLegend;
	}
	else if(drawLegend){
		for(auto h: rHistos){
			TString entryLabel = h->GetTitle();
			if(showMean) entryLabel += msl::tstring(std::to_string(h->GetMean()));
			legend->AddEntry(h, entryLabel, "l");
		}
	}

	if(fitHisto) rHistos[0]->Fit("f1", fitOptions);
	for(int i=0; i < rHistos.size(); ++i){
		rHistos[i]->SetLineWidth(4);
		if(!logy) rHistos[i]->SetMinimum(0);

		if(i == 0) rHistos[i]->Draw(drawOption);
		else rHistos[i]->Draw("same hist "+drawOption);
	}

	if(drawLegend) legend->Draw();
	if(fitHisto) cPlot->Update();

	cPlot->SaveAs(plotName);
	delete cPlot;
}


void plot2DHistograms(std::vector<TH2*> histos, TString plotName="",
					string options = "", TLegend * userLegend=NULL,
					int colorPalette=55){
	bool drawLegend(false), logx(false), logy(false), logz(false), showMean(false);
	drawLegend = options.find("l") != std::string::npos ? true:false;
	logx = options.find("x") != std::string::npos ? true:false;
	logy = options.find("y") != std::string::npos ? true:false;
	logz = options.find("z") != std::string::npos ? true:false;
	showMean = options.find("m") != std::string::npos ? true:false;

	bool styleByPalette = histos.size() >= 7;
	std::vector<int> lineColors = {4, 2, 1, 8, 38, 617};

	gStyle->SetPalette(colorPalette);
	gStyle->SetOptStat(0);
	TString drawOption = "";
	std::vector<TH2*> rHistos = histos;

	if(histos.size() == 1) drawLegend = 0;
	if(histos[0]->GetDimension() != 1) drawOption += "colz";

	TCanvas * cPlot = new TCanvas("cPlot");
	cPlot->cd();
	if(logx) cPlot->SetLogx();
	if(logy) cPlot->SetLogy();
	if(logz) cPlot->SetLogz();

	cPlot->SetCanvasSize(2100,1500);

	double offsetx = 0.25;
	double offsety = 0.07;
	auto * legend = new TLegend(0.35-offsetx, 0.75-offsety, 0.35+offsetx, 0.75+offsety);
	legend->SetBorderSize(0);

	if(userLegend != NULL){
		legend = userLegend;
	}
	else if(drawLegend){
		for(auto h: rHistos){
			TString entryLabel = h->GetTitle();
			if(showMean) entryLabel += msl::tstring(std::to_string(h->GetMean()));
			legend->AddEntry(h, entryLabel, "l");
		}
	}

	for(int i=0; i < rHistos.size(); ++i){
		if(!styleByPalette) rHistos[i]->SetLineColor(lineColors[i]);
		if(i == 0) rHistos[i]->Draw(drawOption);
		else rHistos[i]->Draw("same hist "+drawOption);
	}

	if(drawLegend) legend->Draw();

	cPlot->SaveAs(plotName);
	delete cPlot;
}