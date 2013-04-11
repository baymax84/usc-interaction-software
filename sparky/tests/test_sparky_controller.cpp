// preprocessor directives
#include <sparky/sparky_controller.h>

// executes main program code
int main( int argc, char** argv )
{
    std::string yaml_doc_path = "params/params.yaml";

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

    auto const & joints_map = sparky.joint_angle_controller_.getJoints();

    std::vector<std::string> target_joints{ "HeadTurn", "LtElbow", "RtElbow", "LtArmOut", "RtArmOut", "LtArmForward", "RtArmForward" };
    for( auto joint_name_it = target_joints.cbegin(); joint_name_it != target_joints.cend(); ++joint_name_it )
    {
        std::string const & joint_name = *joint_name_it;
        auto const & joint_it = joints_map.find( joint_name );

        sparky::JointAnglePair const min_limit = sparky.joint_angle_controller_.getJointAngleMinLimitPair( joint_name );
        sparky::JointAnglePair const max_limit = sparky.joint_angle_controller_.getJointAngleMaxLimitPair( joint_name );
        double const min_joint_angle = min_limit.first;
        double const max_joint_angle = max_limit.first;

        int const num_segments = 10;
        for( int i = 0; i <= num_segments; ++i )
        {
            double const segment = min_joint_angle + ( max_joint_angle - min_joint_angle ) * (double)( i ) / (double)(num_segments);
            sparky.setJointAngle( joint_name, segment );
            usleep( 1.0 * 1000000 / num_segments );
        }
        for( int i = 0; i <= num_segments; ++i )
        {
            double const segment = min_joint_angle + ( max_joint_angle - min_joint_angle ) * (double)( num_segments - i ) / (double)(num_segments);
            sparky.setJointAngle( joint_name, segment );
            usleep( 1.0 * 1000000 / num_segments );
        }
        usleep( 0.25 * 1000000 );
        sparky.setJointAngle( joint_name, joint_it->second.home_ );
    }

/*
    sparky.setJointAngle( "HeadTurn", -30 );
    sleep( 2 );
    sparky.setJointAngle( "HeadTurn", 30 );
*/
    return 0;
} // main(int, char**)
