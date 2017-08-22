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

namespace po = boost::program_options;
using namespace std;

int main(int argc, char * argv[])
{
  string ifile;
  unsigned column, nBlock, every;
  double avg_corr;
  double tail_ratio;
  string str_cols;

  po::options_description desc ("# Allowed options");
  desc.add_options()
      ("help,h", "print this message")
      ("input,f", po::value<std::string > (&ifile)->default_value ("data"), "the input file containing data.")
      ("column,c", po::value<unsigned > (&column)->default_value (1), "the column used.")
      ("multi-cols,m", po::value<string > (&str_cols), "Use data on multi-columns. Should be a sequence of indexes seperated by comma, no spacing in between, if set then option --column is inactive")
      ("tail,t", po::value<double > (&tail_ratio)->default_value (1.0), "ratio of the data used for averge, count from tail. 1 means full data, 0 means no data.")
      ("num-block,n", po::value<unsigned > (&nBlock)->default_value (20), "number of blocks.")
      ("every", po::value<unsigned > (&every)->default_value (1), "avery number of data.")
      ("avg-corr", po::value<double > (&avg_corr)->default_value (0.0), "a constant correction to the average.");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify (vm);
  if (vm.count("help")){
    std::cout << desc<< "\n";
    return 0;
  }
  if (tail_ratio <= 0) return 0;

  TableFileLoader tfl (ifile.c_str());
  cout << "# analyze file: " << ifile << endl;
  vector<unsigned > vec_cols;
  if (vm.count("multi-cols")){
    vector<string> words;
    StringOperation::split (str_cols, ",", words);
    for (unsigned ii = 0; ii < words.size(); ++ii){
      vec_cols.push_back (atoi(words[ii].c_str()));
    }
  }
  else{
    vec_cols.push_back (column);
  }

  tfl.setColumns (vec_cols);
  tfl.setEvery (every);
  vector<vector<double > > deposite;
  tfl.loadAll (deposite);

  if (tail_ratio < 1){
    unsigned numb_line = (1. - tail_ratio) * deposite[0].size();
    for (unsigned ii = 0; ii < deposite.size(); ++ii){
      vector<double >::iterator beg_vec = deposite[ii].begin();
      vector<double >::iterator end_vec (beg_vec);
      end_vec += numb_line;
      deposite[ii].erase (beg_vec, end_vec);
    }
  }  

  for (unsigned ii = 0; ii < deposite.size(); ++ii){
    BlockAverage ba;
    unsigned nBlock_used (nBlock);
    if (nBlock_used > deposite[ii].size()) nBlock_used = deposite[ii].size();
    ba.processData (deposite[ii], nBlock_used);
    double average = ba.getAvg();
    for (unsigned jj = 0; jj < deposite[ii].size(); ++jj){
      deposite[ii][jj] -= average;
    }
    ba.processData (deposite[ii], nBlock_used);
  
    cout << "#" << endl;
    cout << "# col: " << vec_cols[ii] << endl;
    printf ("# num data used: %d with %d blocks. 60 percent confidence level\n", ba.getNumDataUsed(), nBlock_used);
    printf ("#\t avg \t avg_err \t var \t var_err\n");
    printf ("%.10e \t %.10e \t %.10e \t %.10e\n",
	    average + avg_corr, ba.getAvgError(),
	    ba.getVar(), ba.getVarError()
	);
  }
  
  return 0;
}
