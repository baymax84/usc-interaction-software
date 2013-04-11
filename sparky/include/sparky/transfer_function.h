#ifndef SPARKY_TRANSFERFUNCTION_H_
#define SPARKY_TRANSFERFUNCTION_H_

#include <muParser/muParser.h>
#include <sparky/parsed_function.h>
#include <yaml-cpp/yaml.h>
#include <sparky/macros.h>
#include <stack>

namespace sparky
{

class TransferFunction
{
public:
    mu::Parser parser_;
    sparky::ParsedFunction function_;
    double variable_value_;

    TransferFunction();

    TransferFunction( TransferFunction const & other );

    void init( YAML::Node const & node );

    mu::value_type eval( double const & value );

    void registerFunctionPool( std::vector<TransferFunction> const & function );
};

} // sparky

void operator>>( YAML::Node const & node, sparky::TransferFunction & transfer_function );
void operator>>( YAML::Node const & node, std::vector<sparky::TransferFunction> & function_pool );

#endif // SPARKY_TRANSFERFUNCTION_H_
