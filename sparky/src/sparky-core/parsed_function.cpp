#include <sparky/parsed_function.h>

namespace sparky
{

    ParsedFunction::ParsedFunction( std::string const & name, std::vector<std::string> parameters, std::string const & definition )
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

    ParsedFunction::ParsedFunction( ParsedFunction const & other )
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

    ParsedFunction::ParsedFunction( YAML::Node const & node )
    {
        init( node );
    }

    void ParsedFunction::init( YAML::Node const & node )
    {
        try
        {
            node["name"] >> name_;
            YAML::Node const & parameters_node = node["parameters"];
            parameters_.resize( parameters_node.size() );
            node["definition"] >> definition_;

            for( size_t i = 0; i < parameters_node.size(); ++i )
            {
                parameters_node[i] >> parameters_[i];
            }
        }
        catch ( YAML::Exception const & e )
        {
            PRINT_WARN( "%s\n", e.what() );
        }

        PRINT_DEBUG( "Initializing parsed function with name [ %s ] definition [ %s ] and variables:\n", name_.c_str(), definition_.c_str() );
        for( auto variable_it = parameters_.cbegin(); variable_it != parameters_.cend(); ++variable_it )
        {
            PRINT_DEBUG( "> [ %s ]\n", variable_it->c_str() );
        }
    }

    bool ParsedFunction::empty()
    {
        bool const empty_val = expression_.empty();
        PRINT_DEBUG( "ParsedFunction with expression [ %s ] empty()? : [ %u ]\n", expression_.c_str(), empty_val );
        return empty_val;
    }

    bool ParsedFunction::hasChildFunction()
    {
        bool const has_child_function_val = expression_.find( ParsedFunction::FUNC_PREFIX_ ) != std::string::npos;
        PRINT_DEBUG( "ParsedFunction with expression [ %s ] hasChildFunction()? : [ %u ]\n", expression_.c_str(), has_child_function_val );
        return has_child_function_val;
    }

    void ParsedFunction::expand()
    {
        expand( _FunctionMap() );
    }

    void ParsedFunction::expand( std::vector<ParsedFunction> const & functions )
    {
        PRINT_DEBUG( "Expanding ParsedFunction given pool of [ %zu ] functions\n", functions.size() );
        _FunctionMap function_map;
        for( auto function_it = functions.cbegin(); function_it != functions.cend(); ++function_it )
        {
            auto function = *function_it;
            function_map[function.name_] = function;
            PRINT_DEBUG( "Storing function [ %s ]\n", function.name_.c_str() );
        }
        return expand( function_map );
    }

    void ParsedFunction::expand( _FunctionMap const & functions )
    {
        PRINT_DEBUG( "Expanding ParsedFunction with expression [ %s ] and function pool of size [ %zu ]\n", expression_.c_str(), functions.size() );
        if( !hasChildFunction() ) expression_ = definition_;
        else
        {
            while( !empty() && hasChildFunction() )
            {
                expandNextChildFunction( functions );
            }
        }
    }

    void ParsedFunction::generateExpression()
    {
        expression_ = definition_;
        PRINT_DEBUG( "Copied expression [ %s ] from definiton [ %s ]\n", expression_.c_str(), definition_.c_str() );
    }

    void ParsedFunction::generateExpression( std::vector<std::string> const & variables )
    {
        PRINT_DEBUG( "Generating expression from definition [ %s ] and variable replacements:\n", definition_.c_str() );
        for( size_t i = 0; i < variables.size(); ++i )
        {
            PRINT_DEBUG( "> [ var_%s ] -> [ %s ]\n", parameters_.at( i ).c_str(), variables.at( i ).c_str() );
        }

        // build map of input vars->output vars
        std::map<std::string, std::string> variable_swap_map;
        for( size_t i = 0; i < parameters_.size(); ++i )
        {
            variable_swap_map[parameters_.at( i )] = variables.at( i );
        }

        std::string output_expression = "(";

        // look for the first instance of a var
        // parse out name
        // replace argument definition in output expression
        // repeat until no more instances of var

        size_t segment_start_idx = 0;
        size_t segment_end_idx = 0;

        // do this while we can still find instances of ParsedFunction::VAR_PREFIX_
        do
        {
            // advance this pointer to the beginning of the first occurance of ParsedFunction::VAR_PREFIX_, or std::string::npos if none found
            size_t const new_segment_end_idx = definition_.find( ParsedFunction::VAR_PREFIX_, segment_start_idx );

            // if we found something
            if( new_segment_end_idx != std::string::npos )
            {
                segment_end_idx = new_segment_end_idx;

                PRINT_DEBUG( "Found VAR_PREFIX_ at index [ %zu ] in definition of length [ %zu ]\n", segment_end_idx, definition_.size() );

                size_t const var_fullname_start_idx = segment_end_idx;

                // advance this pointer to the beginning of the variable name, which should be just after the end of the occurance of ParsedFunction::VAR_PREFIX_
                segment_end_idx += ParsedFunction::VAR_PREFIX_.size();
                // remember where this variable name starts
                size_t const var_name_start_idx = segment_end_idx;
                // for each character in the expression
                while( segment_end_idx <= definition_.size() )
                {
                    // if the current character is not ( alphanumeric or "_" or end of string not reached ), we have reached the end of the current variable name
                    if( segment_end_idx >= definition_.size() || ( !std::isalnum( static_cast<int>( definition_.at( segment_end_idx ) ) ) && definition_.at( segment_end_idx ) != '_' ) )
                    {
                        // the variable name starts at var_name_start_idx and ends at segment_end_idx (and is segment_end_idx - var_name_start_idx long)
                        std::string const input_variable_name = definition_.substr( var_name_start_idx, segment_end_idx - var_name_start_idx );
                        PRINT_DEBUG( "Variable has name [ %s ]\n", input_variable_name.c_str() );
                        // see if we know about this variable (we should, or somebody dun goof'd)
                        auto variable_swap_map_it = variable_swap_map.find( input_variable_name );
                        // if we do know about this variable
                        if( variable_swap_map_it != variable_swap_map.cend() )
                        {
                            std::string const & output_variable_name = variable_swap_map_it->second;
                            PRINT_DEBUG( "Input variable [ %s ] has corresponding output expression [ %s ]\n", input_variable_name.c_str(), output_variable_name.c_str() );

                            // dump this segment into our output expression, but with the input var swapped for the output var
                            std::string output_segment = definition_.substr( segment_start_idx, segment_end_idx - segment_start_idx );
                            PRINT_DEBUG( "Extracted new output segment [ %s ]\n", output_segment.c_str() );

                            output_segment.replace( var_fullname_start_idx - segment_start_idx, ParsedFunction::VAR_PREFIX_.size() + input_variable_name.size(), "(" + output_variable_name + ")" );
                            PRINT_DEBUG( "Updated output segment [ %s ]\n", output_segment.c_str() );

                            output_expression += output_segment;
                            PRINT_DEBUG( "New output expression [ %s ]\n", output_expression.c_str() );
                        }

                        segment_start_idx = segment_end_idx;

                        // break out of this loop and move on
                        break;
                    }
                    else
                    {
                        PRINT_DEBUG( "Looking for end of segment; current char is [ %c ]\n", definition_.at( segment_end_idx ) );
                        segment_end_idx ++;
                    }
                }
            }
            else
            {
                std::string const output_segment = definition_.substr( segment_start_idx );
                PRINT_DEBUG( "Extracted new output segment [ %s ]\n", output_segment.c_str() );

                PRINT_DEBUG( "New output expression [ %s ]\n", output_expression.c_str() );
            }
        }
        while( segment_end_idx < definition_.size() );

        expression_ = output_expression + ")";

        PRINT_DEBUG( "Generated expression [ %s ] from definiton [ %s ]\n", expression_.c_str(), definition_.c_str() );
    }

    void ParsedFunction::normalize()
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

    size_t ParsedFunction::findMatchingParenthesisIdx( std::string const & expression, size_t const & start_idx )
    {
        PRINT_DEBUG( "Attempting to find matching () pair in expression [ %s ] starting at index [ %zu ]\n", expression.c_str(), start_idx );
        std::string working_expression = expression;
        size_t next_closing_idx = start_idx;
        size_t next_opening_idx = next_closing_idx;

        size_t current_closing_idx = std::string::npos;

        do
        {
            next_closing_idx = working_expression.find( ")", next_opening_idx + 1 );
            next_opening_idx = working_expression.rfind( "(", next_closing_idx - 1 );

            PRINT_DEBUG( "Closing/opening indices [ %zu ], [ %zu ]\n", next_closing_idx, next_opening_idx );

            if( next_opening_idx != std::string::npos && next_closing_idx != std::string::npos )
            {
                current_closing_idx = next_closing_idx;

                if( next_opening_idx > start_idx )
                {
                    working_expression.replace( next_closing_idx, 1, "|" );
                    working_expression.replace( next_opening_idx, 1, "|" );

                    PRINT_DEBUG( "Updated working expression [ %s ]\n", working_expression.c_str() );
                }
            }
        }
        while( next_opening_idx != std::string::npos && next_closing_idx != std::string::npos && next_opening_idx > start_idx );

        return current_closing_idx;
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
    void ParsedFunction::expandNextChildFunction( _FunctionMap const & functions )
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
        size_t const argument_expr_start_idx = expression_.find( "(", func_idx ) + 1;
        // find the end of the argument expression
        size_t const argument_expr_end_idx = ParsedFunction::findMatchingParenthesisIdx( expression_, argument_expr_start_idx - 1 );

        PRINT_DEBUG( "Found start/end of the expression [ %s ] at indices [ %zu ], [ %zu ]\n", expression_.c_str(), argument_expr_start_idx, argument_expr_end_idx );

        // the name of the function we're going to need to look up is now between func_idx and argument_expr_start_idx
        std::string const name = expression_.substr( func_idx, argument_expr_start_idx - 1 - func_idx );

        // the list of arguments this function takes is encoded in the string between the argument start index and the argument end index
        std::string const arguments_str = expression_.substr( argument_expr_start_idx, argument_expr_end_idx - argument_expr_start_idx );

        PRINT_DEBUG( "Found sub-function with name [ %s ] and argument expression [ %s ]\n", name.c_str(), arguments_str.c_str() );

        // parse the arguments out of the argument string by skipping all parenthesis-contained content
        // ( arg1, func_some_function( arg2, sin( arg3 ) ), cos( arg4 ) ) -> [ arg1, some_function( arg2, sin( arg3 ) ), cos( arg4 ) ]
        std::vector<std::string> arguments;

        size_t argument_start_idx = 0;
        for( size_t idx = 0; idx < arguments_str.size(); ++idx )
        {
            std::string const current_char = arguments_str.substr( idx, 1 );
            if( current_char == "," )
            {
                arguments.push_back( arguments_str.substr( argument_start_idx, idx - argument_start_idx ) );
                argument_start_idx = idx + 1;
            }
            else if( current_char == "(" ) idx = ParsedFunction::findMatchingParenthesisIdx( arguments_str, idx );
        }

        arguments.push_back( arguments_str.substr( argument_start_idx ) );

        auto child_function_it = functions.find( name );

        if( child_function_it == functions.end() )
        {
            PRINT_WARN( "Definition for child function [ %s ] not found\n", name.c_str() );
            return;
        }

        ParsedFunction child_function = child_function_it->second;
        child_function.expand( functions );
        child_function.generateExpression( arguments );

        expression_.replace( func_idx, argument_expr_end_idx - func_idx + 1, child_function.expression_ );
        PRINT_DEBUG( "Updated expression [ %s ] with child expression [ %s ]\n", expression_.c_str(), child_function.expression_.c_str() );
    }

    std::string const ParsedFunction::FUNC_PREFIX_ = "func_";
    std::string const ParsedFunction::VAR_PREFIX_ = "var_";
}

void operator>>( YAML::Node const & node, sparky::ParsedFunction & function )
{
    function.init( node );
}

void operator>>( YAML::Node const & node, std::vector<sparky::ParsedFunction> & functions )
{
    try
    {
        functions.resize( node.size() );
        for( size_t i = 0; i < node.size(); ++i )
        {
            node[i] >> functions[i];
//            functions.push_back( node[i].to<sparky::ParsedFunction>() );
        }
    }
    catch ( YAML::Exception const & e )
    {
        PRINT_WARN( "%s\n", e.what() );
    }
}
