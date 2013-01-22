/*
 * - name: f1_fwd
 *   parameters: [ "var_y" ]
 *   definition: "2 * var_y"
 *
 *   expand()
 *     expression_ = "2 * var_y"
 *     name_ = "f1_fwd"
 *     parameters_ = { "var_y" }
 *
 * - name: f2_fwd
 *   parameters: [ "x" ]
 *   definition: "2 * func_f1_fwd( var_x + 1 )"
 *
 *   expand()
 *     2 * expand( f1_fwd( expand( x + 1 ) ) )
 *     2 * expand( f1_fwd( x + 1 ) )
 *     2 * 2 * ( x + 1 )
 */

#ifndef SPARKY_PARSEDFUNCTION_H_
#define SPARKY_PARSEDFUNCTION_H_

#include <vector>
#include <string>
#include <map>
#include <sparky/macros.h>

class ParsedFunction
{
public:
    typedef std::map<std::string, ParsedFunction> _FunctionMap;
    std::string definition_;
    std::string expression_;
    std::string name_;
    std::vector<std::string> parameters_;

    static std::string const FUNC_PREFIX_;

    ParsedFunction( std::string const & name = "", std::vector<std::string> parameters = std::vector<std::string>(), std::string const & definition = "" )
    :
        name_( name ),
        parameters_( parameters ),
        definition_( definition ),
        expression_( definition_ )
    {
        PRINT_DEBUG( "Creating new parsed function with name [ %s ] definition [ %s ] and variables:\n", name_.c_str(), definition_.c_str() );
        for( auto variable_it = parameters_.cbegin(); variable_it != parameters_.cend(); ++variable_it )
        {
            PRINT_DEBUG( "> [ %s ]\n", variable_it->c_str() );
        }
    }

    ParsedFunction( ParsedFunction const & other )
    :
        name_( other.name_ ),
        parameters_( other.parameters_ ),
        definition_( other.definition_ ),
        expression_( other.expression_ )
    {
        PRINT_DEBUG( "Copying parsed function with name [ %s ] definition [ %s ] and variables:\n", name_.c_str(), definition_.c_str() );
        for( auto variable_it = parameters_.cbegin(); variable_it != parameters_.cend(); ++variable_it )
        {
            PRINT_DEBUG( "> [ %s ]\n", variable_it->c_str() );
        }
    }

    bool empty()
    {
        PRINT_DEBUG( "ParsedFunction with expression [ %s ] empty()? : [ %u ]\n", expression_.c_str(), expression_.empty() );
        return expression_.empty();
    }

    bool hasChildFunction()
    {
        PRINT_DEBUG( "ParsedFunction with expression [ %s ] hasChildFunction()? : [ %u ]\n", expression_.c_str(), expression_.find( ParsedFunction::FUNC_PREFIX_ ) != std::string::npos );
        return expression_.find( ParsedFunction::FUNC_PREFIX_ ) != std::string::npos;
    }

    void expand()
    {
        expand( _FunctionMap() );
    }

    void expand( std::vector<ParsedFunction> const & functions )
    {
        _FunctionMap function_map;
        for( auto function_it = functions.cbegin(); function_it != functions.cend(); ++function_it )
        {
            auto const & function = *function_it;
            function_map[function.name_] = function;
        }
        return expand( function_map );
    }

    void expand( _FunctionMap const & functions )
    {
        PRINT_DEBUG( "Expanding ParsedFunction with expression [ %s ] and function pool of size [ %zu ]\n", expression_.c_str(), functions.size() );
        while( !empty() && hasChildFunction() )
        {
            expandNextChildFunction( functions );
        }
    }

    void generateExpression()
    {
        expression_ = definition_;
        PRINT_DEBUG( "Copied expression [ %s ] from definiton [ %s ]\n", expression_.c_str(), definition_.c_str() );
    }

    void generateExpression( std::vector<std::string> const & variables )
    {
        PRINT_DEBUG( "Generating expression from definition [ %s ] and variable replacements:\n", definition_.c_str() );
        for( size_t i = 0; i < variables.size(); ++i )
        {
            PRINT_DEBUG( "> [ var_%s ] -> [ %s ]\n", parameters_.at( i ).c_str(), variables.at( i ).c_str() );
        }

        // for each new variable, replace the corresponding old variable
        for( size_t i = 0; i < variables.size(); ++i )
        {
            size_t var_start_idx = std::string::npos;
            do
            {
                std::string const input_var = "var_" + parameters_.at( i );
                var_start_idx = expression_.find( input_var );
                if( var_start_idx != std::string::npos ) expression_.replace( var_start_idx, input_var.size(), variables.at( i ) );
            }
            while( var_start_idx != std::string::npos );
        }
        PRINT_DEBUG( "Generated expression [ %s ] from definiton [ %s ]\n", expression_.c_str(), definition_.c_str() );
    }

    void normalize()
    {
        for( size_t i = 0; i < parameters_.size(); ++i )
        {
            size_t var_start_idx = std::string::npos;
            do
            {
                std::string const input_var = "var_" + parameters_.at( i );
                var_start_idx = expression_.find( input_var );
                if( var_start_idx != std::string::npos ) expression_.replace( var_start_idx, input_var.size(), parameters_.at( i ) );
            }
            while( var_start_idx != std::string::npos );
        }
    }

    static size_t findMatchingParenthesisIdx( std::string const & expression, size_t const & start_idx = 0 )
    {
        PRINT_DEBUG( "Attempting to find matching () pair in expression [ %s ] starting at index [ %zu ]\n", expression.c_str(), start_idx );
        size_t next_closing_idx = expression.find( ")", start_idx );
        size_t next_opening_idx = expression.rfind( "(", next_closing_idx );

        PRINT_DEBUG( "Initial closing/opening indices [ %zu ], [ %zu ]\n", next_closing_idx, next_opening_idx );

        size_t current_closing_idx = std::string::npos;

        // count the number of "(" until the first ")" is found
        // while we can still find occurrances of our desired character
        while( next_closing_idx != std::string::npos && next_opening_idx != std::string::npos )
        {
            current_closing_idx = next_closing_idx;

            // the "next" index is the next occurrance of our character after the current index
            next_closing_idx = expression.find( ")", next_closing_idx + 1 );
            next_opening_idx = expression.rfind( "(", next_opening_idx - 1 );
        }

        // if no more pairs found, return the current closing index (which is std::string::npos if zero total pairs were found)
        if( next_closing_idx == std::string::npos && next_opening_idx == std::string::npos ) return current_closing_idx;

        // parenthesis mismatch
        return std::string::npos;
    }

    // f1( x ) : x^2
    //
    // f2( a, b, c ) : a * b^2 + 2 * func_f1( c )
    //
    // f3( y, z ) : func_f1( func_f2( y, z, 2 ) )
    //
    // expandNextChildFunction()
    // -> [f1]([func_f2( y, z, 2 )])
    //    - name: f1
    //    - variables: func_f2( y, z, 2 )
    //    - definition: (func_f2( y, z, 2 ))^2
    //    expandNextChildFunction()
    //    -> [f2]([y, z, 2])^2
    //       - name: f2
    //       - variables: y, z, 2
    //       - definition: ( y * z^2 + func_f1( 2 ) )^2
    //       expandNextChildFunction()
    //       -> ( y * z^2 + 2 * [f1]([2]) )^2
    //          - name: f1
    //          - variables: 2
    //          - definition: (2)^2
    //
    // result: ( y * z^2 + 2 * 2^2 )^2
    void expandNextChildFunction( _FunctionMap const & functions )
    {
        PRINT_DEBUG( "Attempting to expand next child expression from base expression [ %s ] using function pool of size [ %zu ]\n", expression_.c_str(), functions.size() );
        // find the first occurance of "func_"
        size_t const func_idx = expression_.find( ParsedFunction::FUNC_PREFIX_ );

        // if not found, there's nothing to expand
        if( func_idx == std::string::npos ) return;

        PRINT_DEBUG( "Found FUNC_PREFIX at index [ %zu ]\n", func_idx );

        // remove "func_"
        expression_ = expression_.replace( func_idx, ParsedFunction::FUNC_PREFIX_.size(), "" );

        PRINT_DEBUG( "Removed FUNC_PREFIX from expression [ %s ]\n", expression_.c_str() );

        // find the start of the argument expression
        size_t const argument_expr_start_idx = expression_.find( "(" ) + 1;
        // find the end of the argument expression
        size_t const argument_expr_end_idx = ParsedFunction::findMatchingParenthesisIdx( expression_, argument_expr_start_idx );

        PRINT_DEBUG( "Found start/end of the expression [ %s ] at indices [ %zu ], [ %zu ]\n", expression_.c_str(), argument_expr_start_idx, argument_expr_end_idx );

        // the name of the function we're going to need to look up is now between func_idx and argument_expr_start_idx
        std::string const name = expression_.substr( func_idx, argument_expr_start_idx - 1 - func_idx );

        // the list of arguments this function takes is encoded in the string between the argument start index and the argument end index
        std::string const argument_str = expression_.substr( argument_expr_start_idx, argument_expr_end_idx - argument_expr_start_idx );

        PRINT_DEBUG( "Found sub-function with name [ %s ] and argument expression [ %s ]\n", name.c_str(), argument_str.c_str() );

        // parse the arguments out of the argument string by skipping all parenthesis-contained content
        // ( arg1, func_some_function( arg2, sin( arg3 ) ), cos( arg4 ) ) -> [ arg1, some_function( arg2, sin( arg3 ) ), cos( arg4 ) ]
        std::vector<std::string> arguments;

        size_t argument_start_idx = 0;
        for( size_t idx = 0; idx < argument_str.size(); ++idx )
        {
            std::string const current_char = argument_str.substr( idx, 1 );
            if( current_char == "," )
            {
                arguments.push_back( argument_str.substr( argument_start_idx, idx - argument_start_idx ) );
                argument_start_idx = idx + 1;
            }
            else if( current_char == "(" ) idx = ParsedFunction::findMatchingParenthesisIdx( argument_str, idx );
        }

        arguments.push_back( argument_str.substr( argument_start_idx ) );

        auto child_function_it = functions.find( name );

        if( child_function_it == functions.end() ) return;

        ParsedFunction child_function = child_function_it->second;
        child_function.generateExpression( arguments );
        child_function.expand( functions );

        expression_.replace( func_idx, argument_expr_end_idx - func_idx, child_function.expression_ );
    }
};

#endif // SPARKY_PARSEDFUNCTION_H_
