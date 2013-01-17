#ifndef SPARKY_TRANSFERFUNCTION_H_
#define SPARKY_TRANSFERFUNCTION_H_

#include <muParser/muParser.h>
#include <yaml-cpp/yaml.h>
#include <sparky/macros.h>

namespace sparky
{

class TransferFunction
{
protected:
    std::vector<double> variable_values_;
    std::vector<std::string> variable_names_;
    std::string definition_;
    mu::Parser parser_;

public:
    TransferFunction();

    TransferFunction( TransferFunction const & other );

    void exportTo( mu::Parser & parser ) const;

    mu::value_type eval( mu::value_type const * values, int num_values )
};

} // sparky

void operator>>( YAML::Node const & node, sparky::TransferFunction & transfer_function );

#endif // SPARKY_TRANSFERFUNCTION_H_
