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
#include <cctype>
#include <sparky/macros.h>
#include <yaml-cpp/yaml.h>

namespace sparky
{

class ParsedFunction
{
public:
    typedef std::map<std::string, ParsedFunction> _FunctionMap;
    std::string definition_;
    std::string expression_;
    std::string name_;
    std::vector<std::string> parameters_;

    static std::string const FUNC_PREFIX_;
    static std::string const VAR_PREFIX_;

    ParsedFunction( std::string const & name = "", std::vector<std::string> parameters = std::vector<std::string>(), std::string const & definition = "" );

    ParsedFunction( ParsedFunction const & other );

    ParsedFunction( YAML::Node const & node );

    void init( YAML::Node const & node );

    bool empty();

    bool hasChildFunction();

    void expand();

    void expand( std::vector<ParsedFunction> const & functions );

    void expand( _FunctionMap const & functions );

    void generateExpression();

    void generateExpression( std::vector<std::string> const & variables );

    void normalize();

    static size_t findMatchingParenthesisIdx( std::string const & expression, size_t const & start_idx = 0 );

    void expandNextChildFunction( _FunctionMap const & functions );
};

} // sparky

void operator>>( YAML::Node const & node, sparky::ParsedFunction & function );
void operator>>( YAML::Node const & node, std::vector<sparky::ParsedFunction> & function );

#endif // SPARKY_PARSEDFUNCTION_H_
