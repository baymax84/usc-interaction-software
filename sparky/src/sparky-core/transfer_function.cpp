#include <sparky/transfer_function.h>
//#include <functional>

namespace sparky
{
    TransferFunction::TransferFunction()
    {
        //
    }

    TransferFunction::TransferFunction( TransferFunction const & other )
    :
        parser_( other.parser_ ),
        function_( other.function_ )
    {
        //
    }

    void TransferFunction::init( YAML::Node const & node )
    {
        try
        {
            node >> function_;
            parser_.DefineVar( "input", &variable_value_ );
//            auto const & variables = function_.parameters_;
//            variable_values_.resize( variables.size() );
//
//            for( size_t i = 0; i < variables.size(); ++i )
//            {
//                std::string const & variable_name = variables[i];
//                double & variable_value = variable_values_[i];
//                parser_.DefineVar( variable_name, &variable_value );
//            }
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

    mu::value_type TransferFunction::eval( double const & value )
    {
        variable_value_ = value;

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

    void TransferFunction::registerFunctionPool( std::vector<TransferFunction> const & functions )
    {
        std::vector<sparky::ParsedFunction> function_pool( functions.size() );

        for( size_t i = 0; i < functions.size(); ++i )
        {
            function_pool[i] = functions[i].function_;
        }
        function_.expand( function_pool );
        function_.normalize();
        parser_.SetExpr( function_.expression_ );
    }
}

void operator>>( YAML::Node const & node, sparky::TransferFunction & transfer_function )
{
    transfer_function.init( node );
}

void operator>>( YAML::Node const & node, std::vector<sparky::TransferFunction> & function_pool )
{
    function_pool.resize( node.size() );
    try
    {
        for( size_t i = 0; i < node.size(); ++i )
        {
            node[i] >> function_pool[i];
        }
        for( auto function_pool_it = function_pool.begin(); function_pool_it != function_pool.end(); ++function_pool_it )
        {
            function_pool_it->registerFunctionPool( function_pool );
        }
    }
    catch ( YAML::Exception const & e )
    {
        PRINT_WARN( "%s\n", e.what() );
    }
}
