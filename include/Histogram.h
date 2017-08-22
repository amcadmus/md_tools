#ifndef __Tool_Histogram_h_wanghan__
#define __Tool_Histogram_h_wanghan__

#include <vector>

using namespace std;

class Histogram
{
public:
  Histogram		(const unsigned nbin = 10,
			 const double lower = 0.,
			 const double upper = 0.);
  void reinit		(const unsigned nbin = 10,
			 const double lower = 0.,
			 const double upper = 0.);
public:
  void processData	(const vector<double > & vec) ;
  void getResult	(vector<double > & xx,
			 vector<double > & vv);
private:
  void setBoundary	(const double lower_,
			 const double upper_);
  void setNumbBins	(const unsigned nbin)			{n_bin = nbin;}  
  // void setNumbBlocks	(const unsigned nblock)			{n_block = nblock;}
  double lower;
  double upper;
  unsigned n_bin;
  double hh;
  vector<double > value;
}
    ;

#endif


