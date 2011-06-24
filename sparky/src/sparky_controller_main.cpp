// preprocessor directives
#include <sparky/sparky_controller.h>

// executes main program code
int main(int argc, char** argv)
{
  sparky::SparkyController sparky("/dev/ttyACM0");
  msc.connect();
  msc.isConnected();
  msc.disconnect();
  return 0;
} // main(int, char**)
