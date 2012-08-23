#include <interaction/probabilistics/von_mises.h>
#include <interaction/probabilistics/wrapped_normal.h>

void test_cdf_von_mises( double const & mean, double const & k )
{
    std::cout << "x : cdf_von_mises( " << mean << ", " << k << " )" << std::endl;
    {
        double const min = -2 * M_PI;
        double const max = 2 * M_PI;
        double const size = 100;

        for( size_t i = 0; i < size; ++i )
        {
            double const x = min + i * ( max - min ) / ( size - 1 );
            std::cout << x << " : " << cdf_von_mises( mean, k, x ) << std::endl;
        }
    }
    std::cout << std::endl;
}

void test_pdf_von_mises( double const & mean, double const & k )
{
    std::cout << "x : pdf_von_mises( " << mean << ", " << k << " )" << std::endl;
    {
        double const min = -2 * M_PI;
        double const max = 2 * M_PI;
        double const size = 100;

        for( size_t i = 0; i < size; ++i )
        {
            double const x = min + i * ( max - min ) / ( size - 1 );
            std::cout << x << " : " << pdf_von_mises( mean, k, x ) << std::endl;
        }
    }
    std::cout << std::endl;
}

void test_cdf_wrapped_normal( double const & mean, double const & variance )
{
    std::cout << "x : cdf_wrapped_normal( " << mean << ", " << variance << " )" << std::endl;
    {
        double const min = -2 * M_PI;
        double const max = 2 * M_PI;
        double const size = 100;

        for( size_t i = 0; i < size; ++i )
        {
            double const x = min + i * ( max - min ) / ( size - 1 );
            std::cout << x << " : " << cdf_wrapped_normal( mean, variance, x ) << std::endl;
        }
    }
    std::cout << std::endl;
}

void test_pdf_wrapped_normal( double const & mean, double const & variance )
{
    std::cout << "x : pdf_wrapped_normal( " << mean << ", " << variance << " )" << std::endl;
    {
        double const min = -2 * M_PI;
        double const max = 2 * M_PI;
        double const size = 100;

        for( size_t i = 0; i < size; ++i )
        {
            double const x = min + i * ( max - min ) / ( size - 1 );
            std::cout << x << " : " << pdf_wrapped_normal( mean, variance, x ) << std::endl;
        }
    }
    std::cout << std::endl;
}

void test_von_mises( double const & mean, double const & k )
{
    test_cdf_von_mises( mean, k );
    test_pdf_von_mises( mean, k );
}

void test_wrapped_normal( double const & mean, double const & variance )
{
    test_cdf_wrapped_normal( mean, variance );
    test_pdf_wrapped_normal( mean, variance );
}

int main()
{
    test_von_mises( 0, 5 );
    test_von_mises( 0, 2 );
    test_von_mises( 0, 1 );
    test_von_mises( 0, 0.5 );
    test_von_mises( 0, 0.2 );

    test_von_mises( -M_PI, 1 );
    test_von_mises( -M_PI_2, 1 );
    test_von_mises( 0, 1 );
    test_von_mises( M_PI_2, 1 );
    test_von_mises( M_PI, 1 );

    test_wrapped_normal( 0, 5 );
    test_wrapped_normal( 0, 2 );
    test_wrapped_normal( 0, 1 );
    test_wrapped_normal( 0, 0.5 );
    test_wrapped_normal( 0, 0.2 );

    test_wrapped_normal( -M_PI, 1 );
    test_wrapped_normal( -M_PI_2, 1 );
    test_wrapped_normal( 0, 1 );
    test_wrapped_normal( M_PI, 1 );
    test_wrapped_normal( M_PI_2, 1 );
}
