void overlay_plots(TString fileLoV, TString fileHiV, TString fileLonV, TString fileHinV, double timeLo, double timeHi, TString outputName){

TFile * inputLoV = new TFile(fileLoV);
TFile * inputHiV = new TFile(fileHiV);
TFile * inputLonV = new TFile(fileLonV);
TFile * inputHinV = new TFile(fileHinV);

TH1D * h_loV = (TH1D*)inputLoV->Get("heights_allPulses");
TH1D * h_hiV = (TH1D*)inputHiV->Get("heights_allPulses");

TH1D * h_a_loV = (TH1D*)inputLoV->Get("areas_allPulses");
TH1D * h_a_hiV = (TH1D*)inputHiV->Get("areas_allPulses");

TH1D * h_lonV = (TH1D*)inputLonV->Get("heights_allPulses");
TH1D * h_hinV = (TH1D*)inputHinV->Get("heights_allPulses");

TH1D * h_a_lonV = (TH1D*)inputLonV->Get("areas_allPulses");
TH1D * h_a_hinV = (TH1D*)inputHinV->Get("areas_allPulses");

TCanvas * can = new TCanvas("can", "can", 10, 10, 800, 600);
can->SetLogy(true);
can->SetLogx(true);

h_loV->SetLineColor(kBlack);
h_a_loV->SetLineColor(kBlack);
h_hiV->SetLineColor(kRed);
h_a_hiV->SetLineColor(kRed);

h_lonV->SetLineColor(kGreen);
h_a_lonV->SetLineColor(kGreen);
h_hinV->SetLineColor(kBlue);
h_a_hinV->SetLineColor(kBlue);

h_loV->SetLineWidth(2);
h_a_loV->SetLineWidth(2);
h_hiV->SetLineWidth(2);
h_a_hiV->SetLineWidth(2);

h_lonV->SetLineWidth(2);
h_a_lonV->SetLineWidth(2);
h_hinV->SetLineWidth(2);
h_a_hinV->SetLineWidth(2);

h_loV->GetXaxis()->SetTitle("Amplitude [mV]");
h_loV->GetYaxis()->SetTitle("Rate [Hz]");

h_a_loV->GetXaxis()->SetTitle("Area [mV*ns]");
h_a_loV->GetYaxis()->SetTitle("Rate [Hz]");

h_loV->Scale(1. / timeLo);
h_a_loV->Scale(1. / timeLo);
h_hiV->Scale(1. / timeHi);
h_a_hiV->Scale(1. / timeHi);

h_lonV->Scale(1. / timeLo);
h_a_lonV->Scale(1. / timeLo);
h_hinV->Scale(1. / timeHi);
h_a_hinV->Scale(1. / timeHi);


h_loV->Draw();
h_hiV->Draw("same");
h_lonV->Draw("same");
h_hinV->Draw("same");
can->SaveAs("/home/wintering/pdfs/overlaid_unscaled_" + outputName + ".pdf");


h_a_loV->Draw();
h_a_hiV->Draw("same");
h_a_lonV->Draw("same");
h_a_hinV->Draw("same");
can->SaveAs("/home/wintering/pdfs/overlaid_unscaled_areas" + outputName + ".pdf");

double sfV = h_hiV->Integral(h_hiV->FindBin(110), -1) / h_loV->Integral(h_loV->FindBin(110), -1);
h_loV->Scale(sfV);
h_a_loV->Scale(sfV);

double sfnV = h_hinV->Integral(h_hinV->FindBin(110), -1) / h_lonV->Integral(h_lonV->FindBin(110), -1);
h_lonV->Scale(sfnV);
h_a_lonV->Scale(sfnV);

h_loV->Draw();
h_hiV->Draw("same");
h_lonV->Draw("same");
h_hinV->Draw("same");
can->SaveAs("/home/wintering/pdfs/overlaid_scaled_" + outputName + ".pdf");

h_a_loV->Draw();
h_a_hiV->Draw("same");
h_a_lonV->Draw("same");
h_a_hinV->Draw("same");
can->SaveAs("/home/wintering/pdfs/overlaid_scaled_area_" + outputName + ".pdf");

TH1D * h_rateV = (TH1D*)h_loV->Clone("rate");
for(int i = 0; i < h_loV->GetNbinsX(); i++) {
    h_rateV->SetBinContent(i+1, h_loV->Integral(i+1, -1));
}

TH1D * h_a_rateV = (TH1D*)h_a_loV->Clone("rate_area");
for(int i = 0; i < h_a_loV->GetNbinsX(); i++) {
    h_a_rateV->SetBinContent(i+1, h_a_loV->Integral(i+1, -1));
}

TH1D * h_ratenV = (TH1D*)h_lonV->Clone("rate");
for(int i = 0; i < h_lonV->GetNbinsX(); i++) {
    h_ratenV->SetBinContent(i+1, h_lonV->Integral(i+1, -1));
}

TH1D * h_a_ratenV = (TH1D*)h_a_lonV->Clone("rate_area");
for(int i = 0; i < h_a_lonV->GetNbinsX(); i++) {
    h_a_ratenV->SetBinContent(i+1, h_a_lonV->Integral(i+1, -1));
}

h_rateV->GetXaxis()->SetTitle("Threshold [mV]");
h_rateV->GetYaxis()->SetTitle("Rate above threshold [Hz]");

h_rateV->Draw();
h_ratenV->Draw("same");

h_ratenV->SetLineColor(kRed);

can->SetGridx(true);
can->SetGridy(true);

can->SaveAs("/home/wintering/pdfs/overlaid_rate_" + outputName + ".pdf");

h_a_rateV->GetXaxis()->SetTitle("Threshold [mV*ns]");
h_a_rateV->GetYaxis()->SetTitle("Rate above threshold [Hz]");

h_a_rateV->Draw();
h_a_ratenV->Draw("same");
h_a_ratenV->SetLineColor(kRed);

can->SaveAs("/home/wintering/pdfs/overlaid_rate_area_" + outputName + ".pdf");


}

void overlaidPlots(){

	overlay_plots("run836V.root", "run837V.root", "run836nV.root", "run837nV.root", 321.329, 413.097, "ch24");
	overlay_plots("run838V.root", "run839V.root", "run838nV.root", "run839nV.root", 313.924, 734.536, "ch16");
	overlay_plots("run840V.root", "run841V.root", "run840nV.root", "run841nV.root", 282.505, 2366.83, "ch22");


}
