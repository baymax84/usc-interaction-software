/*
 * - name: f1_fwd
 *   variable_names: [ "y" ]
 *   definition: "2 * y"
 *
 *   expand()
 *     expression_ = "2 * y"
 *     name_ = "f1_fwd"
 *     variable_names_ = { "y" }
 *
 * - name: f2_fwd
 *   variable_names: [ "x" ]
 *   definition: "2 * func_f1_fwd( x + 1 )"
 *
 *   expand()
 *     2 * expand( f1_fwd( expand( x + 1 ) ) )
 *     2 * expand( f1_fwd( x + 1 ) )
 *     2 * 2 * ( x + 1 )
 */

class ParsedFunction
{
public:
    typedef std::map<std::string, ParsedFunction> _FunctionMap;
    std::string expression_;
    std::string name_;
    std::vector<std::string> variable_names_;

    static std::string const FUNC_PREFIX_;

    ParsedFunction( std::string const & expression = "", std::string const & name = "" )
    :
        expression_( expression ),
        name_( name )
    {
        //
    }

    void empty()
    {
        return expression_.empty();
    }

    bool hasChildFunction()
    {
        return expression_.find( ParsedFunction::FUNC_PREFIX_ ) != std::string::npos;
    }

    void expand()
    {
        expand( _FunctionMap() );
    }

    void expand( _FunctionMap const & functions )
    {
        while( !empty() && hasChildFunction() )
        {
            expandNextChildFunction();
        }
    }

    static size_t findNextParenthesisIdx( std::string const & expression, size_t start_idx )
    {
        size_t level = 0;
        size_t current_idx = start_idx;
        std::string character = "(";


        // count the number of "(" until the first ")" is found
        while( current_idx != std::string::npos )
        {
            size_t next_idx = expression.find( character, current_idx );
            if( next_idx != std::string::npos )
            if( && character == "(" )
            {
                level ++;
                current_idx = next_idx;
            }
        }

    void expandNextChildFunction( _FunctionMap const & functions )
    {
        std::string expression = expression_;

        // find the first occurance of "func_"
        size_t const func_index = expression.find( ParsedFunction::FUNC_PREFIX_ );

        // if not found, there's nothing to expand
        if( func_index == std::string::npos ) return;

        // remove "func_"
        expression = expression.substr( func_index + ParsedFunction::FUNC_PREFIX_.size() );
        name = expression.substr( expression_.find( "(" ) );

        result.expression_ = expression_.substr( func_index + ParsedFunction::FUNC_PREFIX_.size(),
        ParsedFunction(
    }
};
