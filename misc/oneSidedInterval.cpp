#include "treesV16TemplateTest.h"

void oneSidedInterval(){

    double passMean = 3.45;
    double totalMean = 5.5;

    TF1 *f1 = new TF1("f1","TMath::Poisson(x,[0])",0,200);
    f1->SetParameter(0,passMean);

    TF1 *f2 = new TF1("f2","TMath::Poisson(x,[0])",0,200);
    f2->SetParameter(0,totalMean);

    TF1 *f3 = new TF1("f2","f1/f2",0,200);
    TCanvas * c1 = new TCanvas("c1");
    c1->cd();
    // f1->Draw();
    // f2->Draw("same");
    f3->Draw();
}