#ifndef TRUNCATEDNORMAL_CDFTRUNCATEDNORMAL_H_
#define TRUNCATEDNORMAL_CDFTRUNCATEDNORMAL_H_

#include <gsl/gsl_cdf.h>
#include <math.h>

double cdf_truncated_normal( double const & mean, double const & variance, double const & a, double const & b, double const & value );

#endif // TRUNCATEDNORMAL_CDFTRUNCATEDNORMAL_H_
