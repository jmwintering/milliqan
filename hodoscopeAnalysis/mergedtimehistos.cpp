#include "/home/bmanley/OSUAnalysis/bmanley/treesV16Template.h"
#include "TMultiGraph.h"
#include "THStack.h"

void mergedtimehistos(){

    TMultiGraph * mg_beam = new TMultiGraph("mg_beam", "mg_beam");
    TMultiGraph * mg_nobeam = new TMultiGraph("mg_nobeam", "mg_nobeam");

    THStack *hTDC_beam = new THStack("hTDC_beam","hTDC_beam");
    THStack *hTime_beam = new THStack("hTime_beam","hTime_beam");
    THStack *hTDC_nobeam = new THStack("hTDC_nobeam","hTDC_nobeam");
    THStack *hTime_nobeam = new THStack("hTime_nobeam","hTime_nobeam");

    TH2D * h_2d_total_nobeam = new TH2D("h2d_total_nobeam", "h2d_total_nobeam", 50, 0, 5, 25, 0, 25);
    TH2D * h_2d_total_beam = new TH2D("h2d_total_beam", "h2d_total_beam", 50, 0, 5, 25, 0, 25);

    for(int i = 1000; i < 1090; ++i){
        //if(i > 1000) continue;
        TFile * f = new TFile("/store/user/bmanley/dTime/timing" + toTstring(tostr(i))+ "_beam.root");
        if(!f->IsOpen()) continue;
        TH1D * h_rateps_nobeam = (TH1D*)f->Get("hSync_nobeam");
        h_rateps_nobeam->SetTitle("Run"+toTstring(tostr(i)));
        h_rateps_nobeam->SetLineColor(i - 999);
        hTDC_nobeam->Add(h_rateps_nobeam);

        TH1D * h_dt_nobeam = (TH1D*)f->Get("hDt_nobeam");
        h_dt_nobeam->SetTitle("Run"+toTstring(tostr(i)));
        h_dt_nobeam->SetLineColor(i-999);
        hTime_nobeam->Add(h_dt_nobeam);

        TH1D * h_rateps_beam = (TH1D*)f->Get("hSync_beam");
        h_rateps_beam->SetTitle("Run"+toTstring(tostr(i)));
        h_rateps_beam->SetLineColor(i-999);
        hTDC_beam->Add(h_rateps_beam);

        TH1D * h_dt_beam = (TH1D*)f->Get("hDt_beam");
        h_dt_beam->SetTitle("Run"+toTstring(tostr(i)));
        h_dt_beam->SetLineColor(i-999);
        hTime_beam->Add(h_dt_beam);

        TGraph * gSync_nobeam = (TGraph*)f->Get("gSync_nobeam");
        gSync_nobeam->SetTitle("Run"+toTstring(tostr(i)));
        gSync_nobeam->SetLineColor(i-999);
        mg_nobeam->Add(gSync_nobeam);

        TGraph * gSync_beam = (TGraph*)f->Get("gSync_beam");
        gSync_beam->SetTitle("Run"+toTstring(tostr(i)));
        gSync_beam->SetLineColor(i-999);
        mg_beam->Add(gSync_beam);

        TH2D * h_2d = (TH2D*)f->Get("h2d");
        h_2d_total_nobeam->Add(h_2d);

        TH2D * h_2d_beam = (TH2D*)f->Get("h2d_beam");
        h_2d_total_beam->Add(h_2d_beam);
    }

    TFile * outputFile = new TFile("timing_merged_cpp.root", "RECREATE");
    TCanvas * c1_nobeam = new TCanvas("c1_nobeam");
    TCanvas * c2_nobeam = new TCanvas("c2_nobeam");
    TCanvas * c3_nobeam = new TCanvas("c3_nobeam");
    TCanvas * c4_nobeam = new TCanvas("c4_nobeam");
    TCanvas * c1_beam = new TCanvas("c1_beam");
    TCanvas * c2_beam = new TCanvas("c2_beam");
    TCanvas * c3_beam = new TCanvas("c3_beam");
    TCanvas * c4_beam = new TCanvas("c4_beam");

    c1_nobeam->cd();
    hTDC_nobeam->Draw("nostack");
    c1_nobeam->BuildLegend();
    c1_nobeam->Write();

    c2_nobeam->cd();
    hTime_nobeam->Draw("nostack");
    c2_nobeam->BuildLegend();
    c2_nobeam->Write();

    c3_nobeam->cd();
    h_2d_total_nobeam->Draw("colz");
    c3_nobeam->Write();

    c4_nobeam->cd();
    mg_nobeam->Draw();
    c4_nobeam->Write();

    c1_beam->cd();
    hTDC_beam->Draw("nostack");
    c1_beam->BuildLegend();
    c1_beam->Write();

    c2_beam->cd();
    hTime_beam->Draw("nostack");
    c2_beam->BuildLegend();
    c2_beam->Write();

    c3_beam->cd();
    h_2d_total_beam->Draw("colz");
    c3_beam->Write();

    c4_beam->cd();
    mg_beam->Draw();
    c4_beam->Write();

    outputFile->Close();
}
