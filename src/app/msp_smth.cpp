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
#include "TableFileLoader.h"

namespace po = boost::program_options;
using namespace std;

void average_vec (vector<double > & out,
		  const vector<double > & in,
		  const unsigned & rr)
{
  unsigned size_in = in.size();
  out.resize (size_in);
  for (unsigned ii = 0; ii < size_in; ++ii){
    int lower = ii - rr;
    int upper = ii + rr + 1;
    if (upper > int(size_in)) upper = size_in;
    if (lower < 0) lower = 0;
    int nn = upper - lower;
    double avg = 0;
    for (int jj = lower ; jj < upper; ++jj){
      avg += in[jj];
    }
    avg /= double (nn);
    out[ii] = avg;
  }
}

int main(int argc, char * argv[])
{
  string ifile;
  string str_cols;
  unsigned support;

  po::options_description desc ("# Allowed options");
  desc.add_options()
      ("help,h", "print this message")
      ("input,f", po::value<std::string > (&ifile)->default_value ("data"), "the input file containing data.")
      ("multi-cols,m", po::value<string > (&str_cols), "Use data on multi-columns. Should be a sequence of indexes seperated by comma, no spacing in between, if set then option --column is inactive")
      ("support-radius,r", po::value<unsigned > (&support)->default_value (1), "radius of averaging support. 1 means 3 data points are averaged.");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify (vm);
  if (vm.count("help")){
    std::cout << desc<< "\n";
    return 0;
  }

  vector<unsigned > vec_cols;
  if (vm.count("multi-cols")){
    vector<string> words;
    StringOperation::split (str_cols, ",", words);
    for (unsigned ii = 0; ii < words.size(); ++ii){
      vec_cols.push_back (atoi(words[ii].c_str()));
    }
  }

  cout << "# analyze file: " << ifile << endl;
  TableFileLoader tfl (ifile.c_str());
  vector<unsigned > vec_cols_all;
  unsigned numb_cols = tfl.getNumbColumns();
  for (unsigned ii = 0; ii < numb_cols; ++ii){
    vec_cols_all.push_back (ii+1);
  }
  tfl.setColumns (vec_cols_all);
  tfl.setEvery (1);
  vector<vector<double > > deposite;
  tfl.loadAll (deposite);
  if (deposite.size() == 0) return 0;

  for (unsigned ii = 0; ii < vec_cols.size(); ++ii){
    vector<double > out ;
    average_vec (out, deposite[vec_cols[ii]-1], support);
    deposite[vec_cols[ii]-1] = out;
  }

  for (unsigned ii = 0; ii < deposite[0].size(); ++ii){
    for (unsigned jj = 0; jj < deposite.size(); ++jj){
      printf ("%16f", deposite[jj][ii]);
    }
    cout << endl;
  }
  
  return 0;
}
