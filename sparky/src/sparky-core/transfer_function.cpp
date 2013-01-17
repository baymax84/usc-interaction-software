#include <sparky/transfer_function.h>
//#include <functional>

namespace sparky
{
    TransferFunction * TransferFunction_last_caller;

    double testFunction( double const * values, int num_values )
    {
        return 0;
    }

    TransferFunction::TransferFunction()
    {
        //
    }

    TransferFunction::TransferFunction( TransferFunction const & other )
    :
        variable_values_( other.variable_values_ ),
        variable_names_( other.variable_names_ ),
        definition_( other.definition_ ),
        name_( other.name_ ),
        parser_( other.parser_ )
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
            //parser.DefineFun( name_, testFunction );
            //parser.DefineFun( name_, std::bind( &TransferFunction::eval, this, std::placeholders::_1, std::placeholders::_2 ) );
            TransferFunction_last_caller = const_cast<TransferFunction*>( this );
            parser.DefineFun( name_, TransferFunctionEvalWrapper );
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
            return parser_.Eval();
        }
        catch( mu::Parser::exception_type const & e )
        {
            PRINT_WARN( "%s\n", e.GetMsg().c_str() );
        }
        return 0;
    }

    double TransferFunctionEvalWrapper( double const * values, int num_values )
    {
        return TransferFunction_last_caller->eval( values, num_values );
    }
}

void operator>>( YAML::Node const & node, sparky::TransferFunction & transfer_function )
{
    transfer_function.init( node );
}
