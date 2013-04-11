#include <sparky/transfer_function.h>
#include <fstream>

int main( int argc, char ** argv )
{
    std::string yaml_doc_path = "params/transfer_function.yaml";

    if( argc <= 1 ) PRINT_DEBUG( "Using default path [ %s ] for yaml document.\n", yaml_doc_path.c_str() );
    else
    {
        yaml_doc_path = argv[1];
        PRINT_DEBUG( "Using path [ %s ] for yaml document.\n", yaml_doc_path.c_str() );
    }

    std::ifstream fin( yaml_doc_path.c_str() );
    YAML::Parser parser( fin );
    YAML::Node document;

    try
    {
        parser.GetNextDocument( document );

        PRINT_DEBUG( "Document has %zu nodes\n", document.size() );

        YAML::Node const & functions = document["transfer_functions"];

        std::vector<sparky::TransferFunction> transfer_functions( functions.size() );

        mu::Parser parser;

        for( size_t i = 0; i < functions.size(); ++i )
        {
            sparky::TransferFunction & transfer_function = transfer_functions[i];

            functions[i] >> transfer_function;
//            transfer_function.exportTo( parser );
        }

        try
        {
            parser.SetExpr( "radial_lever(1, 2, 3)" );
            PRINT_INFO( "%f\n", parser.Eval() );
        }
        catch( mu::Parser::exception_type const & e )
        {
            PRINT_WARN( "%s\n", e.GetMsg().c_str() );
        }
    }
    catch( YAML::Exception const & e )
    {
        PRINT_WARN( "%s\n", e.what() );
    }

    return 0;
}
