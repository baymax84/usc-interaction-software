#include <interaction/probabilistics/pdf_truncated_normal.h>

double pdf_truncated_normal( double const & mean, double const & variance, double const & a, double const & b, double const & value )
{
    //      PDF( x )
    // -------------------
    // CDF( b ) - CDF( a )

    return gsl_ran_gaussian_pdf( value - mean, variance ) / ( gsl_cdf_gaussian_P( b - mean, variance ) - gsl_cdf_gaussian_P( a - mean, variance ) );
}
