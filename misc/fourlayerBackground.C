#include "treesV16Template.h"

void fourlayerBackground(){
  TChain * chain = new TChain("t");
  vector<TString> directories = {

    "Run1607_Physics",
    "Run1608_Physics",

    "Run1609_Physics",
    "Run1610_Physics4layer",
    "Run1611_Physics4layer",
    "Run1612_Physics4layer",
    "Run1619_Physics4layer",
    "Run1620_Physics4layer",
    "Run1621_Physics4layer",
    "Run1626_Physics4layer",
    "Run1627_Physics4layer",
    "Run1628_Physics4layer",
    "Run1629_Physics4layer",
    "Run1631_Physics4layer",
    "Run1661_Physics4layer",
    "Run1729_Physics4layer",
    "Run1730_Physics4layer",
    /*
    //"Run1732_Physics4layer",
    //"Run1733_Physics4layerUpdatedLayout",
    //"Run1734_Physics4layerUpdatedLayout",
    "Run1736_Physics4layerUpdatedLayout",
    "Run1737_Physics4layerUpdatedLayout",
    "Run1739_Physics4layerUpdatedLayout",
    "Run1740_Physics4layerUpdatedLayout",
    "Run1741_Physics4layerUpdatedLayout",
    "Run1743_Physics4layerUpdatedLayoutBarsOnly",
    "Run1744_Physics4layerUpdatedLayout",
    "Run1746_Physics4layerUpdatedLayoutBarsOnly",
    "Run1747_Physics4layerUpdatedLayout",
    "Run1749_Physics4layerUpdatedLayoutBarsOnly",
    "Run1750_Physics4layerUpdatedLayout",
    "Run1752_Physics4layerUpdatedLayoutBarsOnly",
    "Run1754_Physics4layerUpdatedLayout",
    "Run1770_Physics4layerUpdatedLayoutBarsOnly",
    "Run1771_Physics4layerUpdatedLayout",
    "Run1787_Physics4layerUpdatedLayoutBarsOnly",
    "Run1804_Physics4layerUpdatedLayoutBarsOnly",
    "Run1822_Physics4layerUpdatedLayoutBarsOnly",
    "Run1788_Physics4layerUpdatedLayout",
    "Run1805_Physics4layerUpdatedLayout",
    "Run1806_Physics4layerUpdatedLayout",
    "Run1823_Physics4layerUpdatedLayout"
    */
  };

  for(int iDirectory=0; iDirectory<directories.size(); ++iDirectory) chain->Add("/home/MilliQan/data/trees_v16/" + directories[iDirectory] + "/*.root");
  SetAddresses(chain);
  cout << "Added " << chain->GetEntries() << " entries" << endl;


  double runtime = 1; // insert accurate run runtime
  double triple_rate = 0;
  double four_rate = 0;

  int triples = 0;
  int quads = 0;
  for(int i=0; i<chain->GetEntries(); ++i){
    chain->GetEntry(i);


    bool layer0(false), layer1(false), layer2(false), layer3(false);
    int total_hits = 0;
    for(int iPulse=0; iPulse<(*v_area).size(); ++iPulse){
      if((*v_ipulse)[iPulse] != 0 || (*v_chan)[iPulse] == 15) continue;
      if((*v_layer)[iPulse] == 0) layer0 = true;
      if((*v_layer)[iPulse] == 1) layer1 = true;
      if((*v_layer)[iPulse] == 2) layer2 = true;
      if((*v_layer)[iPulse] == 3) layer3 = true;
      ++total_hits;
    }
    if(layer0 && layer1 && layer2 && layer3 && total_hits == 4) ++quads;
    if(layer1 && layer2 && layer3 && total_hits == 3) ++triples;

  } // end chain loop

  triple_rate = triples / runtime;
  four_rate = quads / runtime;

  cout << endl << "triples: " << triples << endl;
  cout << "fours: " << quads << endl << endl;

  cout << "triple coin. rate: " << triple_rate  << " Hz" << endl;
  cout << "four coin. rate: " << four_rate << " Hz" << endl;
  cout << "\t difference: " << triple_rate - four_rate << " Hz" << endl << endl;


} // end script
