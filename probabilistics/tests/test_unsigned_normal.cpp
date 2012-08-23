#include <interaction/probabilistics/unsigned_normal.h>
#include <iostream>

int main()
{
    std::cout << "x : cdf_unsigned_normal" << std::endl;
    {
        double const min = 0;
        double const max = 10;
        double const size = 100;

        for( size_t i = 0; i < size; ++i )
        {
            double const x = min + i * max / ( size - 1 );
            std::cout << x << " : " << cdf_unsigned_normal( 2, 2, x ) << std::endl;
        }
    }
    std::cout << std::endl;

    std::cout << "x : pdf_unsigned_normal" << std::endl;
    {
        double const min = 0;
        double const max = 10;
        double const size = 100;

        for( size_t i = 0; i < size; ++i )
        {
            double const x = min + i * max / ( size - 1 );
            std::cout << x << " : " << pdf_unsigned_normal( 2, 2, x ) << std::endl;
        }
    }
    std::cout << std::endl;

    std::cout << "x : cdf_unsigned_normal" << std::endl;
    {
        double const min = 0;
        double const max = 10;
        double const size = 100;

        for( size_t i = 0; i < size; ++i )
        {
            double const x = min + i * max / ( size - 1 );
            std::cout << x << " : " << cdf_unsigned_normal( 5, 2, x ) << std::endl;
        }
    }
    std::cout << std::endl;

    std::cout << "x : pdf_unsigned_normal" << std::endl;
    {
        double const min = 0;
        double const max = 10;
        double const size = 100;

        for( size_t i = 0; i < size; ++i )
        {
            double const x = min + i * max / ( size - 1 );
            std::cout << x << " : " << pdf_unsigned_normal( 5, 2, x ) << std::endl;
        }
    }
    std::cout << std::endl;

    std::cout << "x : cdf_unsigned_normal" << std::endl;
    {
        double const min = 0;
        double const max = 10;
        double const size = 100;

        for( size_t i = 0; i < size; ++i )
        {
            double const x = min + i * max / ( size - 1 );
            std::cout << x << " : " << cdf_unsigned_normal( 5, 10, x ) << std::endl;
        }
    }
    std::cout << std::endl;

    std::cout << "x : pdf_unsigned_normal" << std::endl;
    {
        double const min = 0;
        double const max = 10;
        double const size = 100;

        for( size_t i = 0; i < size; ++i )
        {
            double const x = min + i * max / ( size - 1 );
            std::cout << x << " : " << pdf_unsigned_normal( 5, 10, x ) << std::endl;
        }
    }
    std::cout << std::endl;
}
