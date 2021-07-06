double TurnOn_f1(double *x, Double_t *par){
    auto halfpoint = par[0];
    auto slope = par[1];
    auto plateau = par[2];
    auto offset = 0.;
    auto pt = TMath::Max(x[0],0.000001);
    auto arg = 0;
    arg = (pt - halfpoint)/(TMath::Sqrt(2)*slope);
    auto fitval = offset+0.5*plateau*(1+TMath::Erf(arg));

    // 0.5*par[2]*(1+TMath::Erf((TMath::Max(x[0],0.000001) - par[0])/(TMath::Sqrt(2)*par[1])))
    return fitval;
}

vector<TF1*> fitFuncs(32, NULL);

void readFitFuncs(){

    TFile * f1 = new TFile("fitFunctions.root", "read");

    for(auto key: TRangeDynCast<TKey>( f1->GetListOfKeys()) ){
        TString keyName = key->GetName();
        if(!keyName.Contains("16") && !keyName.Contains("17") && !keyName.Contains("TC")){ continue; }
        auto raw_f = dynamic_cast<TF1*>(key->ReadObj());
        TF1 * f = new TF1(raw_f->GetName(), TurnOn_f1, raw_f->GetXmin(), raw_f->GetXmax(), 3);
        f->SetParameters( raw_f->GetParameter(0), raw_f->GetParameter(1), 1); //raw_f.GetParameter(2) )
        // auto chan = dynamic_cast<int>(keyName.split("_")[1].replace("CH",""));

        TObjArray * splitName = keyName.Tokenize("_");
        auto chan = ((TObjString *)(splitName->At(1)))->String();
        chan =  chan.Replace(0,2,"");
        auto chanNum = chan.Atoi();
        fitFuncs[chanNum] = f;
    }
}