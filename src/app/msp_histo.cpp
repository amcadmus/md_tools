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
#include "Histogram.h"
#include "TableFileLoader.h"

#define MaxLineLength 65536

namespace po = boost::program_options;
using namespace std;

int main(int argc, char * argv[])
{
  string ifile, ofile;
  unsigned column, every;
  string str_cols;
  unsigned numb_bin;
  double upper, lower;
  double tail_ratio;
      
  po::options_description desc ("# Allowed options");
  desc.add_options()
      ("help,h", "print this message")
      ("input,f", po::value<std::string > (&ifile)->default_value ("data"), "the input file.")
      ("column,c", po::value<unsigned > (&column)->default_value (1), "the column used.")
      ("range-up,u", po::value<double > (&upper)->default_value (0), "the upper bond of the range.")
      ("range-low,l", po::value<double > (&lower)->default_value (0), "the lower bond of the range. If set the same as the upper bond, then both the upper and lower bonds will be calculated by the max and min value of the data.")
      ("numb-bin,b", po::value<unsigned > (&numb_bin)->default_value (10), "the number of bins.")
      ("tail,t", po::value<double > (&tail_ratio)->default_value (1.0), "ratio of the data used for averge, count from tail. 1 means full data, 0 means no data.")
      // multi cols turned off
      // ("multi-cols,m", po::value<string > (&str_cols), "Multi-columns. Should be a sequence of indexes seperated by comma, if set then --column is inactive")
      ("every", po::value<unsigned > (&every)->default_value (1), "avery number of data.")
      ("output,o", po::value<string > (&ofile)->default_value ("histo.out"), "the input file.");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify (vm);
  if (vm.count("help")){
    std::cout << desc<< "\n";
    return 0;
  }

  TableFileLoader tfl (ifile.c_str());
  cout << "# analyze file: " << ifile << endl;
  vector<unsigned > vec_cols;
  vector<double > vec_upper;
  vector<double > vec_lower;
  if (vm.count("multi-cols")){
    vector<string> words;
    StringOperation::split (str_cols, ",", words);
    for (unsigned ii = 0; ii < words.size(); ++ii){
      vec_cols.push_back (atoi(words[ii].c_str()));
      vec_upper.push_back (0);
      vec_lower.push_back (0);
    }
  }
  else{
    vec_cols.push_back (column);
    vec_upper.push_back (upper);
    vec_lower.push_back (lower);
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
    Histogram histo (numb_bin, vec_lower[ii], vec_upper[ii]);
    histo.processData (deposite[ii]);
    ofstream myofs;
    myofs.open (ofile.c_str());
    vector<double> xx, vv;
    histo.getResult (xx, vv);
    for (unsigned ii = 0; ii < xx.size(); ++ii){
      myofs << xx[ii] << "\t" << vv[ii] << endl;
    }
  }
  
  return 0;
}
