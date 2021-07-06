#include "treesV16Template.h"

void compileRun(){
        TString run = "Run1329_Physics";

        TChain * chain = new TChain("t");
        SetAddressesChain(chain);

        //  int total_entries = chain->GetEntries();

        chain->Add("/data/trees_v16/" + run + "/*.root");

        cout << "Added " << chain->GetEntries() << " entries" << endl;
        cout << "Added trees to chain." << endl;

        chain->Merge("/data/mergedChains/" + run + "_merged.root");
        cout << "Chain merged." << endl;
}
