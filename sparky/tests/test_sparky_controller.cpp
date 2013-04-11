// preprocessor directives
#include <sparky/sparky_controller.h>

// executes main program code
int main( int argc, char** argv )
{
    std::string yaml_doc_path = "params/params_prototype1.yaml";

    if( argc <= 1 ) PRINT_DEBUG( "Using default path [ %s ] for yaml document.\n", yaml_doc_path.c_str() );
    else
    {
        yaml_doc_path = argv[1];
        PRINT_DEBUG( "Using path [ %s ] for yaml document.\n", yaml_doc_path.c_str() );
    }

    std::ifstream fin( yaml_doc_path.c_str() );
    YAML::Parser parser( fin );
    YAML::Node nodes;
    parser.GetNextDocument( nodes );

    PRINT_DEBUG( "Document has %zu nodes\n", nodes.size() );

    sparky::SparkyController sparky( nodes, true );
    sparky.joint_angle_controller_.setServoTarget( 0, 5, 1500 );

//    sparky.setJointAngle( "r_elbow", 90 );

    sleep( 2 );

    return 0;
} // main(int, char**)
