#include "/data/treesV16Template.h"


vector<double> timeRun(TString dirT, string dirS, int vNum) {
  double totalRunTime = 0;
  double totalRunTimeBeamOff = 0;

  TString trees;
  if(vNum == 14) trees = "14";
  if(vNum == 16) trees = "16";

  TChain * chain = new TChain("t");

  // number of files in directory of run
  int count = numberFiles(dirT);

  std::size_t pos = dirS.find("R");                     // position of "R" in str
  std::string runName = dirS.substr(pos);               //returns "RunNNNN_runDescription/"
  std::size_t pos2 = runName.find("_");                 // position of "R" in str
  std::string runTitle = runName.substr(0,pos2);        //returns run title, e.g. "RunNNNN"
  std::string runDescription = runName.substr(pos2);    //returns run description, e.g. "_Physics"

  //add the files in order, 1 to count, since they are filled in chronological order
  for(int  iFileNum = 1; iFileNum < count; ++iFileNum){
    chain->Add(dirT + Form("/UX5MilliQan_" + runTitle + ".%d" + runDescription + "_v" + trees + ".root", iFileNum));
  }

  SetAddressesChain(chain, vNum);

  // timing variables
  bool previousBeam = true;
  double beamOnTime = 0;
  double beamOffTime = 0;
  double minTempTime = 0;
  double maxTempTime = 0;
  double previousTime = 0;
  int currentRun = 0;
  int previousRun = 0;

  int iter = chain->GetEntries();
  for(int iEvent = 0; iEvent < iter; ++iEvent){
            chain->GetEntry(iEvent);
            if(iEvent % 1000000 == 0) {cout << "Working on Event " << iEvent << endl;}

            // keep track of livetime with respect to beam
            if(beam == !previousBeam ||
              iEvent == 0 ||
              iEvent == chain->GetEntries()-1) {

                      maxTempTime = previousTime;

                      if(previousBeam == true){
                        beamOnTime = maxTempTime - minTempTime;
                        totalRunTime += beamOnTime;
                      }
                      else{
                        beamOffTime = maxTempTime - minTempTime;
                        totalRunTimeBeamOff += beamOffTime;
                      }

                      minTempTime = event_time_fromTDC;

            } // end change flag


              //if(event_time_fromTDC < minTempTime) cout << "TEST 1" << endl;

          previousTime = event_time_fromTDC;
          previousBeam = beam;

    } // end chain loop

  cout << "Total run time beam ON (" << runName << "): "  << totalRunTime << endl;
  cout << "Total run time beam OFF (" << runName << "): " << totalRunTimeBeamOff << endl;

  vector<double> runTimes = {};
  runTimes.push_back(totalRunTime);
  runTimes.push_back(totalRunTimeBeamOff);

  return runTimes;
} // end function


void run_timing(int run=-1, int tv=16){

  //add runs you want to calculate timing for, run script with -1
  //grab tree strings from treesV16Template.h
  vector<string> runs = { " Run1000_tripleChannelJun25BarsPanelsSlabs10mV " ,
 " Run1001_DC " ,
 " Run1003_tripleChannelJun25BarsPanelsSlabs10mV " ,
 " Run1012_tripleChannelJun25BarsPanelsSlabs10mV " ,
 " Run1013_tripleChannelJun25BarsPanelsSlabs10mV " ,
 " Run1018_tripleChannelJun25BarsPanelsSlabs10mV " ,
 " Run1022_tripleChannelJun25BarsPanelsSlabs10mV " ,
 " Run1026_tripleChannelJun25BarsPanelsSlabs10mV " ,
 " Run1040_tripleChannelJul4BarsPanelsSlabs10mV " ,
 " Run1059_tripleChannelJul4BarsPanelsSlabs10mV " ,
 " Run1060_tripleChannelJul4BarsPanelsSlabs10mV " ,
 " Run1062_tripleChannelJul4BarsPanelsSlabs10mV " ,
 " Run1063_tripleChannelJul4BarsPanelsSlabs10mV " ,
 " Run1065_tripleChannelJul4BarsPanelsSlabs10mV " ,
 " Run1068_tripleChannelJul4BarsPanelsSlabs10mV " ,
 " Run1069_Physics " ,
 " Run1070_Physics " ,
 " Run1071_Physics " ,
 " Run1072_Physics " ,
 " Run1073_Physics " ,
 " Run1074_Physics " ,
 " Run1076_Physics " ,
 " Run1077_Physics " ,
 " Run1078_Physics " ,
 " Run1079_Physics " ,
 " Run1081_Physics " ,
 " Run1082_Physics " ,
 " Run1083_Physics " ,
 " Run1084_Physics " ,
 " Run1085_Physics " ,
 " Run1086_Physics " ,
 " Run1087_Physics " ,
 " Run1088_Physics " ,
 " Run1089_Physics " ,
 " Run1090_Physics " ,
 " Run1110_Physics " ,
 " Run1123_Physics " ,
 " Run1127_Physics " ,
 " Run1324_Physics " ,
 " Run1325_Physics " ,
 " Run1326_Physics " ,
 " Run1329_Physics " ,
 " Run1334_Physics " ,
 " Run1336_Physics " ,
 " Run1340_Physics " ,
 " Run1341_Physics " ,
 " Run1343_Physics " ,
 " Run1344_Physics " ,
 " Run1346_Physics " ,
 " Run1349_Physics " ,
 " Run1351_Physics " ,
 " Run1354_Physics " ,
 " Run1355_Physics " ,
 " Run1356_Physics " ,
 " Run1358_Physics " ,
 " Run1359_Physics " ,
 " Run1360_Physics " ,
 " Run1361_Physics " ,
 " Run1362_Physics " ,
 " Run1363_Physics " ,
 " Run1364_Physics " ,
 " Run1366_Physics " ,
 " Run1368_Physics " ,
 " Run1369_Physics " ,
 " Run1370_Physics " ,
 " Run1371_Physics " ,
 " Run1373_Physics " ,
 " Run1374_Physics " ,
 " Run1375_Physics " ,
 " Run1376_Physics " ,
 " Run1378_Physics " ,
 " Run1426_Physics " ,
 " Run1428_Physics " ,
 " Run1429_Physics " ,
 " Run1431_Physics " ,
 " Run1433_Physics " ,
 " Run1434_Physics " ,
 " Run1435_Physics " ,
 " Run1436_Physics " ,
 " Run1437_Physics " ,
 " Run1439_Physics " ,
 " Run1441_Physics " ,
 " Run1442_Physics " ,
 " Run1443_Physics " ,
 " Run1444_Physics " ,
 " Run1445_Physics " ,
 " Run1446_Physics " ,
 " Run1447_Physics " ,
 " Run1448_Physics " ,
 " Run1449_software " ,
 " Run1450_Physics " ,
 " Run1451_Physics " ,
 " Run1452_Physics " ,
 " Run1453_Physics " ,
 " Run1454_Physics " ,
 " Run1455_Physics " ,
 " Run1456_software " ,
 " Run1457_Physics " ,
 " Run1458_Physics " ,
 " Run1459_software " ,
 " Run1460_Physics " ,
 " Run1461_Physics " ,
 " Run1462_Physics " ,
 " Run1463_software " ,
 " Run1464_Physics " ,
 " Run1465_Physics " ,
 " Run1466_Physics " ,
 " Run1467_Physics " ,
 " Run1468_Physics " ,
 " Run1469_software " ,
 " Run1470_Physics " ,
 " Run1471_Physics " ,
 " Run1472_Physics " ,
 " Run1473_Physics " ,
 " Run1474_software " ,
 " Run1475_Physics " ,
 " Run1476_software " ,
 " Run1477_Physics " ,
 " Run1478_Physics " ,
 " Run1479_Physics " ,
 " Run1480_software " ,
 " Run1481_Physics " ,
 " Run1482_Physics " ,
 " Run1483_software " ,
 " Run1484_Physics " ,
 " Run1485_Physics " ,
 " Run1486_software " ,
 " Run1487_Physics " ,
 " Run1488_Physics " ,
 " Run1489_software " ,
 " Run1490_Physics " ,
 " Run1491_Physics " ,
 " Run1492_software " ,
 " Run1493_Physics " ,
 " Run1494_Physics " ,
 " Run1495_Physics " ,
 " Run1496_singleChannelAllChan10mV " ,
 " Run1497_singleChannelAllChan20mV " ,
 " Run1498_singleChannelAllChan50mV " ,
 " Run1499_singleChannelAllChan100mV " ,
 " Run1500_singleChannelAllChan200mV " ,
 " Run1501_singleChannelAllChan500mV " ,
 " Run1502_PhysicsDC " ,
 " Run1503_singleChannelAllChan10mV " ,
 " Run1504_singleChannelAllChan20mV " ,
 " Run1505_singleChannelAllChan50mV " ,
 " Run1506_singleChannelAllChan100mV " ,
 " Run1507_singleChannelAllChan200mV " ,
 " Run1508_singleChannelAllChan300mV " ,
 " Run1509_singleChannelAllChan500mV " ,
 " Run1510_PhysicsDC " ,
 " Run1511_PhysicsDC " ,
 " Run1512_PhysicsDC " ,
 " Run1513_singleChannelAllChan5mV " ,
 " Run1514_singleChannelAllChan10mV " ,
 " Run1515_singleChannelAllChan20mV " ,
 " Run1516_singleChannelAllChan50mV " ,
 " Run1517_PhysicsDC " ,
 " Run1518_PhysicsDC " ,
 " Run1519_PhysicsDC " ,
 " Run1520_singleChannelAllChan5mV " ,
 " Run1521_singleChannelAllChan10mV " ,
 " Run1522_singleChannelAllChan20mV " ,
 " Run1523_singleChannelAllChan50mV " ,
 " Run1524_PhysicsDC " ,
 " Run1525_singleChannelAllChan50mV " ,
 " Run1526_PhysicsDC " ,
 " Run1527_PhysicsDC " ,
 " Run1528_singleChannelAllChan5mV " ,
 " Run1529_singleChannelAllChan10mV " ,
 " Run1530_singleChannelAllChan20mV " ,
 " Run1531_singleChannelAllChan50mV " ,
 " Run1532_PhysicsDC " ,
 " Run1533_PhysicsDC " ,
 " Run1534_PhysicsDC " ,
 " Run1535_singleChannelAllChan5mV " ,
 " Run1536_singleChannelAllChan10mV " ,
 " Run1537_singleChannelAllChan30mV " ,
 " Run1538_singleChannelAllChan60mV " ,
 " Run1539_PhysicsDC " ,
 " Run1540_PhysicsDC " ,
 " Run1541_PhysicsDC " ,
 " Run1542_singleChannelAllChan5mV " ,
 " Run1543_singleChannelAllChan10mV " ,
 " Run1544_singleChannelAllChan30mV " ,
 " Run1545_singleChannelAllChan60mV " ,
 " Run1546_PhysicsDC " ,
 " Run1547_PhysicsDC " ,
 " Run1548_Physics " ,
 " Run1549_Physics " ,
 " Run1550_Physics " ,
 " Run1551_software " ,
 " Run1552_Physics " ,
 " Run1553_Physics " ,
 " Run1554_Physics " ,
 " Run1555_software " ,
 " Run1556_Physics " ,
 " Run1557_Physics " ,
 " Run1558_software " ,
 " Run1559_Physics " ,
 " Run1560_Physics " ,
 " Run1561_software " ,
 " Run1562_Physics " ,
 " Run1563_Physics " ,
 " Run1564_software " ,
 " Run1565_Physics " ,
 " Run1566_Physics " ,
 " Run1567_software " ,
 " Run1568_Physics " ,
 " Run1569_Physics " ,
 " Run1570_software " ,
 " Run1571_Physics " ,
 " Run1572_Physics " ,
 " Run1573_singleChannelAllChan5mV " ,
 " Run1574_singleChannelAllChan10mV " ,
 " Run1575_singleChannelAllChan30mV " ,
 " Run1576_singleChannelAllChan60mV " ,
 " Run1577_PhysicsDC " ,
 " Run1578_Physics " ,
 " Run1579_Physics " ,
 " Run1580_singleChannelAllChan5mV " ,
 " Run1581_singleChannelAllChan10mV " ,
 " Run1582_singleChannelAllChan30mV " ,
 " Run1583_singleChannelAllChan60mV " ,
 " Run1584_PhysicsDC " ,
 " Run1585_Physics " ,
 " Run1586_Physics " ,
 " Run1587_singleChannelAllChan5mV " ,
 " Run1588_singleChannelAllChan10mV " ,
 " Run1589_singleChannelAllChan30mV " ,
 " Run1590_singleChannelAllChan60mV " ,
 " Run1591_PhysicsDC " ,
 " Run1592_Physics " ,
 " Run1593_Physics " ,
 " Run1594_singleChannelAllChan5mV " ,
 " Run1595_singleChannelAllChan10mV " ,
 " Run1596_singleChannelAllChan30mV " ,
 " Run1597_singleChannelAllChan60mV " ,
 " Run1598_Physics " ,
 " Run1599_PhysicsDC " ,
 " Run1600_Physicslow " ,
 " Run1601_Physicslow " ,
 " Run1602_Physicslow " ,
 " Run1603_Physicslow " ,
 " Run1604_Physics " ,
 " Run1605_Physics " ,
 " Run1606_Physics " ,
 " Run1607_Physics " ,
 " Run1608_Physics " ,
 " Run1609_Physics " ,
 " Run1610_Physics4layer " ,
 " Run1611_Physics4layer " ,
 " Run1612_Physics4layer " ,
 " Run1613_Physics4layer " ,
 " Run1614_Physics4layer " ,
 " Run1615_Physics4layer " ,
 " Run1616_Physics4layer " ,
 " Run1617_Physics4layer " ,
 " Run1618_Physics4layer " ,
 " Run1619_Physics4layer " ,
 " Run1620_Physics4layer " ,
 " Run1621_Physics4layer " ,
 " Run1622_singleChannel200mVCH0 " ,
 " Run1623_test " ,
 " Run1624_external " ,
 " Run1625_external " ,
 " Run1626_Physics4layer " ,
 " Run1627_Physics4layer " ,
 " Run1628_Physics4layer " ,
 " Run1629_Physics4layer " ,
 " Run1630_Physics4layer " ,
 " Run1631_Physics4layer " ,
 " Run1632_RadiationStudy " ,
 " Run1633_RadiationStudy " ,
 " Run1634_doubleChannel7mVCHs2 " ,
 " Run1635_doubleChannel7mVCHs2 " ,
 " Run1636_RadiationStudy " ,
 " Run1637_RadiationStudy " ,
 " Run1638_RadiationStudy " ,
 " Run1639_RadiationStudy " ,
 " Run1640_RadiationStudy " ,
 " Run1641_RadiationStudy " ,
 " Run1642_RadiationStudy " ,
 " Run1643_RadiationStudy " ,
 " Run1644_RadiationStudy " ,
 " Run1645_RadiationStudy " ,
 " Run1646_RadiationStudy " ,
 " Run1647_RadiationStudy " ,
 " Run1648_RadiationStudy " ,
 " Run1649_RadiationStudy " ,
 " Run1650_RadiationStudy " ,
 " Run1651_RadiationStudy " ,
 " Run1652_RadiationStudy " ,
 " Run1653_RadiationStudy " ,
 " Run1654_RadiationStudy " ,
 " Run1655_RadiationStudy " ,
 " Run1656_RadiationStudy " ,
 " Run1657_RadiationStudy " ,
 " Run1658_RadiationStudy " ,
 " Run1659_RadiationStudy " ,
 " Run1660_RadiationStudy " ,
 " Run1661_Physics4layer " ,
 " Run1662_RadiationStudy " ,
 " Run1663_RadiationStudy " ,
 " Run1664_RadiationStudy " ,
 " Run1665_RadiationStudy " ,
 " Run1666_RadiationStudy " ,
 " Run1667_RadiationStudy " ,
 " Run1668_RadiationStudy " ,
 " Run1669_RadiationStudy " ,
 " Run1670_RadiationStudy " ,
 " Run1671_RadiationStudy " ,
 " Run1672_RadiationStudy " ,
 " Run1673_RadiationStudy " ,
 " Run1674_RadiationStudy " ,
 " Run1675_RadiationStudy " ,
 " Run1676_RadiationStudy " ,
 " Run1677_RadiationStudy " ,
 " Run1678_RadiationStudy " ,
 " Run1679_RadiationStudy " ,
 " Run1680_RadiationStudy " ,
 " Run1735_software " ,
 " Run1681_RadiationStudy " ,
 " Run1682_RadiationStudy " ,
 " Run1683_RadiationStudy " ,
 " Run1684_RadiationStudy " ,
 " Run1685_RadiationStudy " ,
 " Run1686_RadiationStudy " ,
 " Run1687_RadiationStudy " ,
 " Run1688_RadiationStudy " ,
 " Run1689_RadiationStudy " ,
 " Run1690_RadiationStudy " ,
 " Run1691_RadiationStudy " ,
 " Run1692_RadiationStudy " ,
 " Run1693_RadiationStudy " ,
 " Run1694_RadiationStudy " ,
 " Run1695_RadiationStudy " ,
 " Run1696_RadiationStudy " ,
 " Run1697_RadiationStudy " ,
 " Run1698_RadiationStudy " ,
 " Run1699_RadiationStudy " ,
 " Run1700_RadiationStudy " ,
 " Run1701_RadiationStudy " ,
 " Run1702_RadiationStudy " ,
 " Run1703_singleChannel100mVCH27 " ,
 " Run1704_singleChannel100mVCH27 " ,
 " Run1705_singleChannel100mVCH27 " ,
 " Run1706_singleChannel100mVCH29 " ,
 " Run1707_singleChannel100mVCH29 " ,
 " Run1708_doubleChannelAllBars7mV " ,
 " Run1709_singleChannel100mVAnyChannel " ,
 " Run1710_singleChannel100mVAnyChannel " ,
 " Run1711_singleChannel100mVAnyChannel " ,
 " Run1712_singleChannel100mVAnyChannel " ,
 " Run1713_singleChannel100mVAnyChannel " ,
 " Run1714_singleChannel100mVAnyChannel " ,
 " Run1715_singleChannel100mVCH27 " ,
 " Run1716_singleChannel100mVCH10 " ,
 " Run1717_singleChannel100mVCH29 " ,
 " Run1718_singleChannel100mVCH29 " ,
 " Run1719_singleChannel100mVCH29 " ,
 " Run1720_singleChannel100mVCH20 " ,
 " Run1721_singleChannel100mVCH18 " ,
 " Run1722_singleChannel100mVCH18 " ,
 " Run1723_singleChannel100mVCH21 " ,
 " Run1724_singleChannel100mVCH21 " ,
 " Run1725_singleChannel100mVCH20 " ,
 " Run1726_singleChannel100mVCH28 " ,
 " Run1727_RadiationStudyVert " ,
 " Run1728_RadiationStudyVert " ,
 " Run1729_Physics4layer " ,
 " Run1730_Physics4layer " ,
 " Run1731_singleChannel100mVCH21 " ,
 " Run1732_Physics4layer " ,
 " Run1733_Physics4layerUpdatedLayout " ,
 " Run1734_Physics4layerUpdatedLayout " ,
 " Run1736_Physics4layerUpdatedLayout " ,
 " Run1737_Physics4layerUpdatedLayout " ,
 " Run1738_software " ,
 " Run1739_Physics4layerUpdatedLayout " ,
 " Run1740_Physics4layerUpdatedLayout " ,
 " Run1741_Physics4layerUpdatedLayout " ,
 " Run1742_software " ,
 " Run1743_Physics4layerUpdatedLayoutBarsOnly " ,
 " Run1744_Physics4layerUpdatedLayout " ,
 " Run1745_software " ,
 " Run1746_Physics4layerUpdatedLayoutBarsOnly " ,
 " Run1747_Physics4layerUpdatedLayout " ,
 " Run1748_software " ,
 " Run1749_Physics4layerUpdatedLayoutBarsOnly " ,
 " Run1750_Physics4layerUpdatedLayout " ,
 " Run1751_software " ,
 " Run1752_Physics4layerUpdatedLayoutBarsOnly " ,
 " Run1753_Physics4layerUpdatedLayout " ,
 " Run1754_Physics4layerUpdatedLayout " ,
 " Run1755_DCAllbars3layer " ,
 " Run1756_DCAllbars4layerUpdatedLayout " ,
 " Run1757_DCCH01CH23 " ,
 " Run1758_DCCH1617CH20 " ,
 " Run1759_DCCH23CH45 " ,
 " Run1760_DCCH67CH1213 " ,
 " Run1761_DCCH01CH45 " ,
 " Run1762_DCCH1617CH28 " ,
 " Run1763_DCCH2425CH20 " ,
 " Run1764_DCCH89CH23 " ,
 " Run1765_DCCH01CH89 " ,
 " Run1766_DCCH2028 " ,
 " Run1767_DCCH2425CH28 " ,
 " Run1768_DCCH89CH45 " ,
 " Run1769_software " ,
 " Run1770_Physics4layerUpdatedLayoutBarsOnly " ,
 " Run1771_Physics4layerUpdatedLayout " ,
 " Run1772_DCAllbars3layer " ,
 " Run1773_DCAllbars4layerUpdatedLayout " ,
 " Run1774_DCCH01CH23 " ,
 " Run1775_DCCH1617CH20 " ,
 " Run1776_DCCH23CH45 " ,
 " Run1777_DCCH67CH1213 " ,
 " Run1778_DCCH01CH45 " ,
 " Run1779_DCCH1617CH28 " ,
 " Run1780_DCCH2425CH20 " ,
 " Run1781_DCCH89CH23 " ,
 " Run1782_DCCH01CH89 " ,
 " Run1783_DCCH2028 " ,
 " Run1784_DCCH2425CH28 " ,
 " Run1785_DCCH89CH45 " ,
 " Run1786_software " ,
 " Run1787_Physics4layerUpdatedLayoutBarsOnly " ,
 " Run1788_Physics4layerUpdatedLayout " ,
 " Run1789_DCAllbars3layer " ,
 " Run1790_DCAllbars4layerUpdatedLayout " ,
 " Run1791_DCCH01CH23 " ,
 " Run1792_DCCH1617CH20 " ,
 " Run1793_DCCH23CH45 " ,
 " Run1794_DCCH67CH1213 " ,
 " Run1795_DCCH01CH45 " ,
 " Run1796_DCCH1617CH28 " ,
 " Run1797_DCCH2425CH20 " ,
 " Run1798_DCCH89CH23 " ,
 " Run1799_DCCH01CH89 " ,
 " Run1800_DCCH2028 " ,
 " Run1801_DCCH2425CH28 " ,
 " Run1802_DCCH89CH45 " ,
 " Run1803_software " ,
 " Run1804_Physics4layerUpdatedLayoutBarsOnly " ,
 " Run1805_Physics4layerUpdatedLayout " ,
 " Run1806_Physics4layerUpdatedLayout " ,
 " Run1807_DCAllbars3layer " ,
 " Run1808_DCAllbars4layerUpdatedLayout " ,
 " Run1809_DCCH01CH23 " ,
 " Run1810_DCCH1617CH20 " ,
 " Run1811_DCCH23CH45 " ,
 " Run1812_DCCH67CH1213 " ,
 " Run1813_DCCH01CH45 " ,
 " Run1814_DCCH1617CH28 " ,
 " Run1815_DCCH2425CH20 " ,
 " Run1816_DCCH89CH23 " ,
 " Run1817_DCCH01CH89 " ,
 " Run1818_DCCH2028 " ,
 " Run1819_DCCH2425CH28 " ,
 " Run1820_DCCH89CH45 " ,
 " Run1821_software " ,
 " Run1822_Physics4layerUpdatedLayoutBarsOnly " ,
 " Run1823_Physics4layerUpdatedLayout "
};

  vector<string> runs_used = {};

  vector<string> badWords = {};
  vector<string> goodWords = {};

  if(run == -1){

    for(int iDirectory = 0; iDirectory < runs.size(); iDirectory++) {
        string dir = runs[iDirectory];

      //exlcude runs with bad words in it
      if(badWords.size() != 0){
        bool skip = false;
        for(int i = 0; i < badWords.size(); i++){
          if(dir.find(badWords[i]) != string::npos) skip = true;
        }
        if(skip) continue;
      }

      //include only runs with good words in it
      if(goodWords.size() != 0) {
        for(int i = 0; i < goodWords.size(); i++){
          if(dir.find(goodWords[i]) != string::npos) runs_used.push_back(runs[iDirectory]);
        }
      }
      else runs_used.push_back(runs[iDirectory]);

    } //end directory loop

  }

  double totalBeamTime = 0;
  double totalBeamTimeOff = 0;

  if(run==-1){

    cout << "Running over vector of runs..." << endl;
    ofstream fout;
    fout.open("run_timesAll.txt", ios::out);

    // Go calculate actual run times for selected runs
    for(int i = 0; i < runs_used.size(); i++){

      vector<double> tempRunTimes = {0,0};

      std::size_t pos = runs_used[i].find("_");                   //position of "_"
      std::string runNumberString = runs_used[i].substr(3,pos);   //run number as a string, e.g. "NNNN"
      int runNumber = toint(runNumberString);                     //run number as an int

      TString pathT = runDirectory(runNumber, tv);
      string pathS = runDirectory(runNumber,tv);

      tempRunTimes = timeRun(pathT, pathS, tv);
      totalBeamTime += tempRunTimes[0];
      totalBeamTimeOff += tempRunTimes[1];

      fout << "Run: " << runs_used[i] << endl;
      fout << "Beam on: " << tempRunTimes[0] << endl;
      fout << "Beam off: " << tempRunTimes[1] << endl << endl;
      // fout.close();
      cout << "Wrote " << runs_used[i] << " to disk." << endl;
    }


    fout << "Total beam on time: " << totalBeamTime << endl;
    fout << "Total beam off time: " << totalBeamTimeOff << endl;

    fout.close();
    cout << "Finished writing all selected runs to disk." << endl;

  }

  //calculate run time for one run
  else{
      ofstream fout;
      fout.open("run_time_Run" + tostr(run) + ".txt", ios::out);

      TString pathT = runDirectory(run, tv);

      string pathS = runDirectory(run,tv);

      vector<double> tempRunTimes = {0,0};
      tempRunTimes = timeRun(pathT, pathS, tv);
      totalBeamTime += tempRunTimes[0];
      totalBeamTimeOff += tempRunTimes[1];

      fout << "Run: " << run << endl;
      fout << "Beam on: " << tempRunTimes[0] << endl;
      fout << "Beam off: " << tempRunTimes[1] << endl << endl;
      fout.close();
      cout << "Wrote " << run << " to disk." << endl;
  }

} // end function
