#include "treesV19Template.h"

void testing(){

	TChain * chain = new TChain("t");
	chain->Add("/data/trees_v19/HS_Run1000.root");
	SetAddresses(chain);

	int elts = chain->GetEntries();

	for(int i =0; i < elts; ++i){
		chain->GetEntry(i);

		cout << (*v_nPECorr)[27] << endl;
	}
}


