#include <sparky/parsed_function.h>

int main()
{
    std::vector<ParsedFunction> functions;

    functions.push_back( ParsedFunction( "f1", std::vector<std::string>{ "x", "y", "z" }, "var_x + var_y * var_z" ) );

    functions.push_back( ParsedFunction( "f2", std::vector<std::string>{ "x", "y", "z" }, "func_f1( var_z, var_y, var_x ) + var_x" ) );

    functions.push_back( ParsedFunction( "f3", std::vector<std::string>{ "x", "y", "z" }, "func_f1( var_x^2, var_y, var_z ) + func_f2( func_f1( var_z, var_y, 2 ), 1, 2 )" ) );

    functions[2].expand( functions );

    functions[2].normalize();

    PRINT_INFO( "%s\n", functions[2].expression_.c_str() );

/*
    (( x^2) + ( y) * ( z )) + ((( ( 2 )) + ( ( 1)) * ( ( (( z) + ( y) * ( 2 ))) )) + ( (( z) + ( y) * ( 2 ))))

    (( x^2) + ( y) * ( z )) + ((( ( 2 )) + ( ( 1)) * ( ( (( z) + ( y) * ( 2 ))) )) + ( (( z) + ( y) * ( 2 ))))

    ( x^2 + y * z ) + ( 2 + 1 * ( z + y * 2 ) ) + ( z + y * 2 )
*/
    return 0;
}
