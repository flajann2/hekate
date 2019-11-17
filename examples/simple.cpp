#include <hekate>
#include <iostream>

using namespace hekate;
using namespace std;

int main(int argc, char **argv) { 
  cli argline;
  argline << param<ilk::string, 1, 2>("Simple parameters");

  return argline(argc, argv);
}

