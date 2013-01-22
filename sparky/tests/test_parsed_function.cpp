#include <sparky/parsed_function.h>

int main()
{
    std::vector<ParsedFunction> functions;

    functions.push_back( ParsedFunction( "f1", std::vector<std::string>{ "z", "y", "x" }, "var_z * var_x^2 + var_y" ) );

    functions.push_back( ParsedFunction( "f2", std::vector<std::string>{ "y", "z", "x" }, "2 * func_f1( var_y, var_z, var_x ) + var_z" ) );

    functions[1].expand( functions );

    //functions[1].normalize();

    PRINT_INFO( "%s\n", functions[1].expression_.c_str() );

    return 0;
}
