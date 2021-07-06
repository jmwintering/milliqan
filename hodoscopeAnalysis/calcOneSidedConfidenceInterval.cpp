#include "treesV16TemplateTest.h"

vector<double> calcOneSidedConfidenceInterval(int passArg=-1, int totalArg=-1){
    // testing
    double pass =  1; 
    double total =  2;

    vector<double> errorBounds = {-1,-1};

    if(passArg != -1 and totalArg != -1){
        pass = passArg;
        total = totalArg;
    }
    else{
        cerr << "Warning in <ROOT>: using default values 1:2, Enter nonnegative arguments to silence this warning" << endl;
        return errorBounds;
    }
    
    TH1F::SetDefaultSumw2();

    TH1F  * hPass = new TH1F("hPass", "hPass", 1, 0, 1);
    TH1F  * hTotal = new TH1F("hTotal", "hTotal", 1, 0, 1);

    hPass->Fill(0.0, pass);
    hTotal->Fill(0.0,total);

    TGraphAsymmErrors * effGraph = new TGraphAsymmErrors(hPass, hTotal);
    errorBounds = {effGraph->GetErrorYlow(0),effGraph->GetErrorYhigh(0)};
    
    return errorBounds;
} // end function 