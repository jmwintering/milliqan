#include "treesV16Template.h"

void comparePlots(){
    TFile * mattInput = new TFile("/data/abcd_data/closurePlots/plotFiles/outputFileABCDbeamBlindsignalInjectionStudies191204V16_noPrePulseAddPanelTEST_15ns.root");

    TString config = "";
    TString mattConfig = "";
    for(int i=0; i<2; ++i){
        for(int j=0; j<2; ++j){
            if(i == 0){ config = "noOtherPulse_"; mattConfig = "/NoOtherPulse/max/NPEMin/All"; }
            else {config = "noPrePulse_"; mattConfig = "/NoPrePulse/max/NPEMin/All"; }
            if(j == 0){ config += "noSlabs"; }
            else {config += "atLeastOneSlab"; mattConfig += "PlusAnySlab"; }
            
            TFile * osuInput = new TFile("/data/abcd_data/closurePlots/plotFiles/plotsProjected_15ns_"+config+".root");
            
            
            // grab the observed plots
            TH1D * hA_OSU = (TH1D*)osuInput->Get("A");
            TH1D * hA_Matt = (TH1D*)mattInput->Get(mattConfig+"/A");

            // grab prediction plots
            TH1D * hAP_OSU = (TH1D*)osuInput->Get("APred");
            TH1D * hAP_Matt = (TH1D*)mattInput->Get(mattConfig+"/APred");

            cout << "a osu: " << hA_OSU->GetEntries() << endl;
            cout << "apred osu: " << hAP_OSU->GetEntries() << endl;
            cout << "a matt: " << hA_Matt->GetEntries() << endl;
            cout << "apred matt: " << hAP_Matt->GetEntries() << endl;

            // now plot both on separate canvases (and 1 together)
            TCanvas * c1= new TCanvas("c1", "c1", 700*8.33,500*8.33);
            TCanvas * c2= new TCanvas("c2", "c2", 700*8.33,500*8.33);
            TCanvas * c3= new TCanvas("c3", "c3", 700*8.33,500*8.33);
            c1->SetLogy();
            c1->SetLogx();
            c2->SetLogy();
            c2->SetLogx();
            c3->SetLogy();
            c3->SetLogx();

            hA_OSU->SetLineColor(kBlue);
            hA_Matt->SetLineColor(kRed);
            hAP_OSU->SetLineColor(kGreen);
            hAP_Matt->SetLineColor(kBlack);

            hA_OSU->SetMarkerStyle(4);
            hA_Matt->SetMarkerStyle(5);
            hAP_OSU->SetMarkerStyle(4);
            hAP_Matt->SetMarkerStyle(5);

            hA_OSU->SetTitle(config);
            hAP_OSU->SetTitle(config);

            hA_OSU->GetXaxis()->SetTitle("min NPE");
            hA_Matt->GetXaxis()->SetTitle("min NPE");
            hAP_OSU->GetXaxis()->SetTitle("min NPE");
            hAP_Matt->GetXaxis()->SetTitle("min NPE");
            hA_OSU->GetYaxis()->SetTitle("Event count");
            hA_Matt->GetYaxis()->SetTitle("Event count");
            hAP_OSU->GetYaxis()->SetTitle("Event count");
            hAP_Matt->GetYaxis()->SetTitle("Event count");

            auto legend = new TLegend(0.9,0.8,1-0.48,0.9);
            auto legend2 = new TLegend(0.9,0.8,1-0.48,0.9);
            auto legend3 = new TLegend(0.9,0.65, 1-0.3,0.9);
            legend->AddEntry(hA_OSU, "OSU Observed", "lp");
            legend->AddEntry(hA_Matt, "UCSB Observed", "lp");
            legend2->AddEntry(hAP_OSU, "OSU Predicted", "lp");
            legend2->AddEntry(hAP_Matt, "UCSB Predicted", "lp");
            legend3->AddEntry(hA_OSU, "OSU Observed", "lp");
            legend3->AddEntry(hAP_OSU, "OSU Predicted", "lp");
            legend3->AddEntry(hA_Matt, "Matt Observed", "lp");
            legend3->AddEntry(hAP_Matt, "Matt Predicted", "lp");


    
            c1->cd();
            hA_OSU->Draw();
            hA_Matt->Draw("same");
            legend->Draw();
            c1->SaveAs("/data/abcd_data/closurePlots/compPlots/comp_A"+config+".png");

            c2->cd();
            hAP_OSU->Draw();
            hAP_Matt->Draw("same");
            legend2->Draw();
            c2->SaveAs("/data/abcd_data/closurePlots/compPlots/comp_APred"+config+".png");

            c3->cd();
            hAP_OSU->Draw();
            hAP_Matt->Draw("same");
            hA_Matt->Draw("same");
            hA_OSU->Draw("same");
            legend3->Draw();
            c3->SaveAs("/data/abcd_data/closurePlots/compPlots/comp_all"+config+".png");  
        }
    }
}