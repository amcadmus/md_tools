#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <queue>
#include <vector>
#include <cmath>
#include <boost/program_options.hpp>

#include "StringSplit.h"
#include "BlockAverage.h"
#include "TableFileLoader.h"

#define MaxLineLength 65536

namespace po = boost::program_options;
using namespace std;

int main(int argc, char * argv[])
{
  std::string ifile1, ifile2;
  unsigned column1,column2, nBlock, every;
  double tail_ratio;
  
  po::options_description desc ("# Allowed options");
  desc.add_options()
      ("help,h", "print this message")
      ("input-1,f", po::value<std::string > (&ifile1)->default_value ("data1"), "the first input file.")
      ("column-1", po::value<unsigned > (&column1)->default_value (1), "the used column in the first input file.")
      ("input-2", po::value<std::string > (&ifile2), "the second input file, by default it is set to the first file")
      ("column-2", po::value<unsigned > (&column2)->default_value (1), "the used column int the second input file.")
      ("tail,t", po::value<double > (&tail_ratio)->default_value (1.0), "ratio of the data used for averge, count from tail. 1 means full data, 0 means no data.")
      ("num-block,n", po::value<unsigned > (&nBlock)->default_value (20), "number of blocks.")
      ("every", po::value<unsigned > (&every)->default_value (1), "avery number of data.");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify (vm);
  if (vm.count("help")){
    std::cout << desc<< "\n";
    return 0;
  }
  if (!vm.count("input-2")){
    ifile2 = ifile1;
  }

  cout << "# using first  input file: " << ifile1
       << " with column: " << column1
       << endl;
  cout << "# using second input file: " << ifile2
       << " with column: " << column2
       << endl;

  vector<vector<double > > deposite;

  if (ifile1 == ifile2){
    TableFileLoader tfl (ifile1.c_str());
    vector<unsigned> vec_cols;
    vec_cols.push_back (column1);
    vec_cols.push_back (column2);
    tfl.setColumns (vec_cols);
    tfl.setEvery (every);
    tfl.loadAll (deposite);
  }
  else {
    {
      TableFileLoader tfl (ifile1.c_str());
      vector<unsigned> vec_cols;
      vec_cols.push_back (column1);
      tfl.setColumns (vec_cols);
      tfl.setEvery (every);
      vector<vector<double > > tmp_deposite;
      tfl.loadAll (tmp_deposite);
      deposite.push_back (tmp_deposite[0]);
    }
    {
      TableFileLoader tfl (ifile2.c_str());
      vector<unsigned> vec_cols;
      vec_cols.push_back (column2);
      tfl.setColumns (vec_cols);
      tfl.setEvery (every);
      vector<vector<double > > tmp_deposite;
      tfl.loadAll (tmp_deposite);
      deposite.push_back (tmp_deposite[0]);
    }
  }

  if (deposite.size() != 2){
    cerr << "the number of data sets is not 2, must be something wrong, do nothing" << endl;
    return 1;
  }
  if (deposite[0].size() != deposite[1].size()){
    cerr << "the number of data in files does not match, do nothing" << endl;
    return 1;
  }

  if (tail_ratio < 1){
    unsigned numb_line = (1. - tail_ratio) * deposite[0].size();
    for (unsigned ii = 0; ii < deposite.size(); ++ii){
      vector<double >::iterator beg_vec = deposite[ii].begin();
      vector<double >::iterator end_vec (beg_vec);
      end_vec += numb_line;
      deposite[ii].erase (beg_vec, end_vec);
    }
  }  

  vector<double > & deposite1 (deposite[0]);
  vector<double > & deposite2 (deposite[1]);
  vector<double > values;
  BlockAverage ba1, ba2;
  ba1.processData (deposite1, nBlock);
  ba2.processData (deposite2, nBlock);
  for (unsigned ii = 0; ii < deposite1.size(); ++ii){
    values.push_back ((deposite1[ii] - ba1.getAvg()) * (deposite2[ii] - ba2.getAvg()));
  }
  BlockAverage ba;
  ba.processData (values, nBlock);
  
  printf ("# num data used: %d with %d blocks. 60 percent confidence level\n", ba.getNumDataUsed(), nBlock);
  printf ("# cvar \t cvar_err\n");
  printf ("%e \t %e\n",
	  ba.getAvg(), ba.getAvgError()
      );
  
  return 0;
}
