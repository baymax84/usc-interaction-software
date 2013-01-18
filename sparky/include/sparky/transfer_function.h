#ifndef SPARKY_TRANSFERFUNCTION_H_
#define SPARKY_TRANSFERFUNCTION_H_

#include <muParser/muParser.h>
#include <yaml-cpp/yaml.h>
#include <sparky/macros.h>
#include <stack>

namespace sparky
{

class TransferFunction
{
public:
    static std::stack<TransferFunction *> EVAL_CALLERS_;

    std::vector<double> variable_values_;
    std::vector<std::string> variable_names_;
    std::string definition_;
    std::string name_;
    mu::Parser parser_;

    TransferFunction();

    TransferFunction( TransferFunction const & other );

    void init( YAML::Node const & node );

    void exportTo( mu::Parser & parser ) const;

    mu::value_type eval( mu::value_type const * values, int num_values );

    static double evalWrapper( double const * values, int num_values );
};

} // sparky

void operator>>( YAML::Node const & node, sparky::TransferFunction & transfer_function );

#endif // SPARKY_TRANSFERFUNCTION_H_
