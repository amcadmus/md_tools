#include "Histogram.h"
#include <iostream>

Histogram::
Histogram (const unsigned nbin,
	   const double lower,
	   const double upper)
{
  reinit (nbin, lower, upper);
}

void
Histogram::
reinit (const unsigned nbin_,
	const double lower_,
	const double upper_)
{
  setNumbBins (nbin_);
  setBoundary (lower_, upper_);
  
  hh = (upper - lower) / double(n_bin);
  
  value.clear();
  value.resize (n_bin, 0.0);
}

void
Histogram::
setBoundary (const double lower_,
	     const double upper_)
{
  upper = upper_;
  lower = lower_;
  if (lower >  upper) {
    swap (upper, lower);
  }
}

void
Histogram::
processData (const vector<double > & vec) 
{
  if (lower == upper){
    lower = vec[0];
    upper = vec[0];
    for (unsigned ii = 1; ii < vec.size(); ++ii){
      if (vec[ii] < lower) lower = vec[ii];
      if (vec[ii] > upper) upper = vec[ii] + 1e-6 * vec[ii];
    }
    reinit (n_bin, lower, upper);
  }
  
  double sum = 0;
  for (unsigned ii = 0; ii < vec.size(); ++ii){
    int index = (vec[ii] - lower) / hh;
    if (index >= 0 && index < int(n_bin)){
      value[index] += 1.;
      sum += 1;
    }
  }
  if (sum != 0){
    for (unsigned ii = 0; ii < n_bin; ++ii){
      value[ii] = value[ii] / sum / hh;
    }
  }
}

void
Histogram::
getResult (vector<double > & xx,
	   vector<double > & vv)
{
  xx.resize (n_bin);
  vv.resize (n_bin);

  for (unsigned ii = 0; ii < n_bin; ++ii){
    xx[ii] = lower + (ii + 0.5) * hh;
    vv[ii] = value[ii];
  }
}
