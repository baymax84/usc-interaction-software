#include <sparky/transfer_function.h>

namespace sparky
{
    TransferFunction::TransferFunction()
    {
        //
    }

    TransferFunction::TransferFunction( TransferFunction const & other )
    :
        variables_( other.variables_ ),
        definition( other.definition_ ),
        inverse_( other.inverse_ )
    {
        //
    }

    void TransferFunction::init( YAML::Node const & node )
    {
        try
        {
            node["name"] >> name_;

            YAML::Node const & variables = node["variables"];
            variable_values_.resize( variables.size() );
            variable_names_.resize( variables.size() );

            for( size_t i = 0; i < variables.size(); ++i )
            {
                std::string & variable_name = variable_names_[i];
                double & variable_value = variable_values_[i];
                variables[i] >> variable_name;
                parser_.DefineVar( variable_name, &variable_value );
            }

            node["definition"] >> definition_;
            parser_.SetExpr( definition_ );
        }
        catch( YAML::Exception const & e )
        {
            PRINT_WARN( "%s\n", e.what() );
        }
        catch( mu::Parser::exception_type const & e )
        {
            PRINT_WARN( "%s\n", e.GetMsg().c_str() );
        }
    }

    void TransferFunction::exportTo( mu::Parser & parser ) const
    {
        try
        {
            parser.DefineFun( name_, &TransferFunction::eval, this );
        }
        catch( mu::Parser::exception_type const & e )
        {
            PRINT_WARN( "%s\n", e.GetMsg().c_str() );
        }
    }

    mu::value_type TransferFunction::eval( mu::value_type const * values, int num_values )
    {
        for( int i = 0; i < num_values; ++i )
        {
            variable_values_[i] = values[i];
        }

        try
        {
            return parser_.Eval()
        }
        catch( mu::Parser::exception_type const & e )
        {
            PRINT_WARN( "%s\n", e.GetMsg().c_str() );
        }
        return 0;
    }
}

void operator>>( YAML::Node const & node, sparky::TransferFunction & transfer_function )
{
    transfer_function.init( node );
}
