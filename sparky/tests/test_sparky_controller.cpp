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

    std::vector<std::string> target_joints{ "l_elbow", "r_elbow", "l_arm_out", "r_arm_out", "l_arm_fwd", "r_arm_fwd" };
    for( auto joint_name_it = target_joints.cbegin(); joint_name_it != target_joints.cend(); ++joint_name_it )
    {
        std::string const & joint_name = *joint_name_it;

        sparky::JointAnglePair const min_limit = sparky.joint_angle_controller_.getJointAngleMinLimitPair( joint_name );
        sparky::JointAnglePair const max_limit = sparky.joint_angle_controller_.getJointAngleMaxLimitPair( joint_name );
        double const min_joint_angle = min_limit.first;
        double const max_joint_angle = max_limit.first;

        int const num_segments = 10;
        for( int i = 0; i < num_segments; ++i )
        {
            double const segment = min_joint_angle + ( max_joint_angle - min_joint_angle ) * (double)( i ) / (double)(num_segments);
            sparky.setJointAngle( joint_name, segment );
            usleep( 1.0 * 1000000 / num_segments );
        }
        for( int i = 0; i < num_segments; ++i )
        {
            double const segment = min_joint_angle + ( max_joint_angle - min_joint_angle ) * (double)( num_segments - i ) / (double)(num_segments);
            sparky.setJointAngle( joint_name, segment );
            usleep( 1.0 * 1000000 / num_segments );
        }
    }

//    sparky.setJointAngle( "r_arm_fwd", 90 );
    sleep( 2 );

    return 0;
} // main(int, char**)
