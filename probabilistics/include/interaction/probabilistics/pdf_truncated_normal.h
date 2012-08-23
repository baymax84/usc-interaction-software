#ifndef TRUNCATEDNORMAL_PDFTRUNCATEDNORMAL_H_
#define TRUNCATEDNORMAL_PDFTRUNCATEDNORMAL_H_

#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
#include <math.h>

double pdf_truncated_normal( double const & mean, double const & variance, double const & a, double const & b, double const & value );

#endif // TRUNCATEDNORMAL_PDFTRUNCATEDNORMAL_H_
