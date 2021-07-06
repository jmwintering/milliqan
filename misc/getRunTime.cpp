#include <iostream>
#include <fstream>

void getRunTime(){
  ifstream input;
  input.open("/data/test_abcd/run_timesV16.txt");


  string line;
  while (getline(input, line))
  {
    std::size_t pos1 = line.find("on: ");      // position of _ in str
    if (pos1!=std::string::npos){
      std::string str1 = line.substr(pos1 + 4);     // get from on: to the end
      cout << str1 << endl;
    }

    std::size_t pos2 = line.find("off: ");      // position of _ in str
    if (pos2!=std::string::npos){
      std::string str2 = line.substr(pos2 + 4);     // get from off: to the end
      cout << str2 << endl;
    }
    }
  }



}
