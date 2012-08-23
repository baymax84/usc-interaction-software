#include <interaction/probabilistics/truncated_normal.h>
#include <limits>
#include <iostream>

void test_cdf_truncated_normal( double const & mean, double const & variance, double const & a, double const & b, double const & min, double const & max )
{
    std::cout << "x : cdf_truncated_normal( " << mean << ", " << variance << ", " << a << ", " << b << " )" << std::endl;
    {
        double const size = 100;

        for( size_t i = 0; i < size; ++i )
        {
            double const x = min + i * ( max - min ) / ( size - 1 );
            std::cout << x << " : " << cdf_truncated_normal( mean, variance, a, b, x ) << std::endl;
        }
    }
    std::cout << std::endl;
}

void test_pdf_truncated_normal( double const & mean, double const & variance, double const & a, double const & b, double const & min, double const & max )
{
    std::cout << "x : pdf_truncated_normal( " << mean << ", " << variance << ", " << a << ", " << b << " )" << std::endl;
    {
        double const size = 100;

        for( size_t i = 0; i < size; ++i )
        {
            double const x = min + i * ( max - min ) / ( size - 1 );
            std::cout << x << " : " << pdf_truncated_normal( mean, variance, a, b, x ) << std::endl;
        }
    }
    std::cout << std::endl;
}

void test_truncated_normal( double const & mean, double const & variance, double const & a, double const & b, double const & min, double const & max )
{
    test_cdf_truncated_normal( mean, variance, a, b, min, max );
    test_pdf_truncated_normal( mean, variance, a, b, min, max );
}

void test_truncated_normal( double const & mean, double const & variance, double const & a, double const & b )
{
    double const min = a - ( ( b - a ) / 4 );
    double const max = b + ( ( b - a ) / 4 );
    test_truncated_normal( mean, variance, a, b, min, max );
}

int main()
{
    test_truncated_normal( 0, 1, -std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), -15, 15 );
    test_truncated_normal( 0, 1, -10, 10 );
    test_truncated_normal( 0, 1, -5, 5 );
    test_truncated_normal( 0, 1, -2, 2 );
    test_truncated_normal( 0, 1, -1, 1 );

    test_truncated_normal( -8, 2, -10, 10 );
    test_truncated_normal( 0, 2, -10, 10 );
    test_truncated_normal( 9, 10, -10, 10 );
    test_truncated_normal( 0, 10, -10, 10 );
    test_truncated_normal( 0, 1, 0, std::numeric_limits<double>::max(), -5, 15 );
}
