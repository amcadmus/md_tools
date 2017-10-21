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
  string scolumn, snbin, supper, slower;
  unsigned every;
  double tail_ratio;
      
  po::options_description desc ("# Allowed options");
  desc.add_options()
      ("help,h", "print this message")
      ("input,f", po::value<std::string > (&ifile)->default_value ("data"), "the input file.")
      ("column,c", po::value<string > (&scolumn)->default_value ("1,2"), "the column used, two numbers seperated by comma, no space in between.")
      ("numb-bin,b", po::value<string > (&snbin)->default_value ("10,10"), "the number of bins, two numbers seperated by comma, no space in between.")
      ("range-up,u", po::value<string > (&supper)->default_value ("0.,0."), "the upper bond of the range, two numbers seperated by comma, no space in between.")
      ("range-low,l", po::value<string > (&slower)->default_value ("0.,0."), "the lower bond of the range, two numbers seperated by comma, no space in between. If set the same as the upper bond, then both the upper and lower bonds will be calculated by the max and min value of the data.")
      ("tail,t", po::value<double > (&tail_ratio)->default_value (1.0), "ratio of the data used for averge, count from tail. 1 means full data, 0 means no data.")
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
  vector<unsigned > vec_nbins;

  vector<string> words;
  StringOperation::split (scolumn, ",", words);
  assert (words.size() == 2);
  vec_cols.push_back (atoi(words[0].c_str()));
  vec_cols.push_back (atoi(words[1].c_str()));
  StringOperation::split (snbin, ",", words);
  assert (words.size() == 2);
  vec_nbins.push_back (atoi(words[0].c_str()));
  vec_nbins.push_back (atoi(words[1].c_str()));
  StringOperation::split (slower, ",", words);
  assert (words.size() == 2);
  vec_lower.push_back (atof(words[0].c_str()));
  vec_lower.push_back (atof(words[1].c_str()));
  StringOperation::split (supper, ",", words);
  assert (words.size() == 2);
  vec_upper.push_back (atof(words[0].c_str()));
  vec_upper.push_back (atof(words[1].c_str()));
  cout << "# with nbins: " << vec_nbins[0] << " " << vec_nbins[1] << endl;
  
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

  assert (deposite.size() == 2);
  
  Histogram2D histo (vec_nbins[0], vec_nbins[1], vec_lower[0], vec_lower[1], vec_upper[0], vec_upper[1]);
  histo.processData (deposite[0], deposite[1]);
  
  vector<double > xx, yy;
  vector<vector<double > > vv;  
  histo.getResult (xx, yy, vv);

  ofstream myofs;
  myofs.open (ofile.c_str());
  for (unsigned ii = 0; ii < xx.size(); ++ii){
    for (unsigned jj = 0; jj < yy.size(); ++jj){
      myofs << xx[ii] << " " 
	    << yy[jj] << " \t"
	    << vv[ii][jj] 
	    << endl;
    }
    myofs << endl;
  }
  
  return 0;
}
