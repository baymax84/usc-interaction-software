#include <interaction/probabilistics/cdf_truncated_normal.h>

double cdf_truncated_normal( double const & mean, double const & variance, double const & a, double const & b, double const & value )
{
    // CDF( x ) - CDF( a )
    // -------------------
    // CDF( b ) - CDF( a )

    double const cdf_alpha = gsl_cdf_gaussian_P( a - mean, variance );

    return ( gsl_cdf_gaussian_P( value - mean, variance ) - cdf_alpha ) / ( gsl_cdf_gaussian_P( b - mean, variance ) - cdf_alpha );
}
