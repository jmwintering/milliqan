/*
    Author: Brandon Manley and Luca Lavezzo
    Date: November 2019
    Plot data of cutflow efficiency of signal and background analysis
    of injected signal trees. 
*/

#include "treesV16TemplateTest.h"
void plotCutflow(){

    vector<double> dataset = {1,34,3,2};

    vector<TString> selections = {
        "oneSlab", "noSlab"
    };
    vector<TString> plotTitles = {
        "03", "02", "01", "014", "007", "005", "003", "002", "001", "0005"
    };
    
    vector<vector<double>> oneSlabData = { 

{ 4539653 , 3699626 , 329316 , 134014 , 6603 , 6427 , 6439 , 383 , 7 }, 

{ 4569842 , 3729436 , 358857 , 135221 , 7653 , 6430 , 6442 , 386 , 9 }, 

{ 4632016 , 3791614 , 420370 , 138876 , 11230 , 10514 , 10549 , 3594 , 2624 }, 

{ 4610979 , 3770535 , 399593 , 137366 , 9666 , 7993 , 8022 , 1605 , 1204 }
, 

{ 4637677 , 3797281 , 425816 , 139587 , 12089 , 11816 , 11839 , 4908 , 1745 }, 

{ 4641182 , 3800808 , 429102 , 139929 , 12504 , 12373 , 12388 , 5728 , 1082 }, 

{ 4641804 , 3801404 , 430170 , 140392 , 12999 , 12937 , 12951 , 6585 , 1623 }, 

{ 4643453 , 3803068 , 432165 , 140753 , 13375 , 13342 , 13354 , 7130 , 3519 }, 

{ 4644703 , 3804299 , 432016 , 139618 , 12254 , 12235 , 12247 , 6137 , 4834 }, 

{ 4644877 , 3804477 , 433095 , 139301 , 11936 , 11919 , 11931 , 5865 , 5018 }, 

};

    vector<vector<double>> noSlabData = { 

{ 4539653 , 3699626 , 329316 , 134014 , 6603 , 0 , 0 , 0 , 0 }, 

{ 4569842 , 3729436 , 358857 , 135221 , 7653 , 2 , 3 , 2 , 2 }, 

{ 4632016 , 3791614 , 420370 , 138876 , 11230 , 4022 , 4110 , 3165 , 2594 }, 

{ 4610979 , 3770535 , 399593 , 137366 , 9666 , 1525 , 1583 , 1184 , 1174 }, 

{ 4637677 , 3797281 , 425816 , 139587 , 12089 , 5327 , 5400 , 4488 , 1727 }, 

{ 4641182 , 3800808 , 429102 , 139929 , 12504 , 5907 , 5949 , 5315 , 1074 }, 

{ 4641804 , 3801404 , 430170 , 140392 , 12999 , 6485 , 6512 , 6187 , 1614 }, 

{ 4643453 , 3803068 , 432165 , 140753 , 13375 , 6903 , 6915 , 6737 , 3510 }, 

{ 4644703 , 3804299 , 432016 , 139618 , 12254 , 5800 , 5808 , 5746 , 4826 }, 

{ 4644877 , 3804477 , 433095 , 139301 , 11936 , 5492 , 5492 , 5482 , 5011 }, 

};

    // vector<vector<double>> noSlabData = {
    //     {5393, 4966,4889 ,4794 ,159 ,0,0,0,0},
    //     {35582,34776 ,34430 ,33992 ,1209 ,0,0,0,0},
    //     {76719,75875,75166,74501,3222,0,0,0,0},
    //     {97756,96954,95943 ,95472, 4786,0,0,0,0},
    //     {103417,102621, 101389,101084,5645,0,0,0,0},
    //     {106922,106148,104675,104500,6060,6,6,6,5},
    //     {107544,106744,105743 ,105664, 6555,264 ,264 ,264,264},
    //     {109193,108408 ,107738 ,107706 ,6931,1577 ,1577 ,1567,1566},
    //     {110443,109639 , 107589 ,107568,5810 ,3802 ,3802 ,3791,3777},
    //     {110617,109817 ,108668 ,108656,5492 ,4915,4915,4909,4651} 
    //     };

    int datapoints = 8;
    // vector<TH1D*> oneSlabPlots;
    // vector<TH1D*> noSlabPlots;

    TH1D *noSlabPlots[10];
    TH1D *oneSlabPlots[10];

     cout << "hi" << endl;
    for(int i=0;i<10;++i){
        oneSlabPlots[i] = new TH1D(selections[0]+plotTitles[i],selections[0]+plotTitles[i], datapoints, 0, datapoints);
        noSlabPlots[i] = new TH1D(selections[1]+plotTitles[i],selections[1]+plotTitles[i], datapoints, 0, datapoints);
    
    }
       
    
    for(int h=0; h<10; ++h){
        for(int i=0;i<datapoints; ++i){
            for(int j=0; j<oneSlabData[h][i]; ++j){
                oneSlabPlots[h]->Fill(i);
            }
            for(int j=0; j<noSlabData[h][i]; ++j){
                noSlabPlots[h]->Fill(i);
            }
        }
    }
    
    gStyle->SetOptStat(0);
    TCanvas*c1= new TCanvas("c1");
    TCanvas*c2= new TCanvas("c2");
    c1->cd();
    c1->SetLogy();

    vector<TString> cutNames = {"Start", "RMS cut", "Single Pulse", "nBars Active not above 3", "one hit per layer", "no sheet", "slab cut", "straight path", "delta T < 15 ns"};
    
    for(int iLabel = 1; iLabel <10; iLabel++){
        for(int h=0;h<10;++h){
            noSlabPlots[h]->GetXaxis()->SetBinLabel(iLabel, cutNames[iLabel-1]); 
        }
        for(int h=0;h<10;++h){
            noSlabPlots[h]->SetLineWidth(5);
            oneSlabPlots[h]->SetLineWidth(5);
            noSlabPlots[h]->GetYaxis()->SetRangeUser(1,10e5);
            oneSlabPlots[h]->GetYaxis()->SetRangeUser(1,10e5);
            noSlabPlots[h]->GetYaxis()->SetTitle("Events");
            oneSlabPlots[h]->GetYaxis()->SetTitle("Events");
            oneSlabPlots[h]->GetXaxis()->SetBinLabel(iLabel, cutNames[iLabel-1]); 
        }
    }

    auto legend = new TLegend(0.1,0.7,0.48,0.9);
    legend->SetHeader("Charge","C"); 
    for(int h=0;h<10;++h){
        legend->AddEntry(oneSlabPlots[h], "Q: 0."+plotTitles[h],"l");
    }

    noSlabPlots[9]->Draw("P L");
    oneSlabPlots[9]->SetMarkerStyle(8); 
    oneSlabPlots[9]->SetMarkerSize(2);
    noSlabPlots[9]->SetLineColor(11);
    noSlabPlots[9]->SetMarkerColor(11);
    noSlabPlots[9]->SetTitle("Cut Flow for No Slab (> 100 nPE) Selection");
    for(int k=0; k<10; ++k){
        noSlabPlots[k]->SetMarkerStyle(8); 
        noSlabPlots[k]->SetMarkerSize(2);
        if(k!=9){noSlabPlots[k]->Draw("P same L");
        noSlabPlots[k]->SetMarkerColor(k+1);
        noSlabPlots[k]->SetLineColor(k+1);
        }
    }
    legend->Draw();
    
    c2->cd();
    c2->SetLogy();
    oneSlabPlots[9]->Draw("P L");
    oneSlabPlots[9]->SetMarkerStyle(8); 
    oneSlabPlots[9]->SetMarkerSize(2);
    oneSlabPlots[9]->SetMarkerColor(11);
    oneSlabPlots[9]->SetLineColor(11);
    oneSlabPlots[9]->SetTitle("Cut Flow for One Slab (< 100 nPE) Selection");
    for(int k=0; k<10; ++k){
        oneSlabPlots[k]->SetMarkerStyle(8); 
        oneSlabPlots[k]->SetMarkerSize(2);
        if(k!=9){oneSlabPlots[k]->Draw("P same L");
        oneSlabPlots[k]->SetMarkerColor(k+1);
        oneSlabPlots[k]->SetLineColor(k+1);
        }
    }
    legend->Draw();

    cout << "All done!" << endl; 
}