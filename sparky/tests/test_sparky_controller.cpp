// preprocessor directives
#include <sparky/sparky_controller.h>

// executes main program code
int main( int argc, char** argv )
{
	sparky::SparkyController sparky( "/dev/ttyACM1" );
	sparky.connect();
	sparky.isConnected();
	sparky.disconnect();
	return 0;
} // main(int, char**)
