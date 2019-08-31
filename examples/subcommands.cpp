#include <hekate/hekate>
#include <iostream>

using namespace hekate;
using namespace std;

int main(int argc, char **argv) {
  cli argline;
  argline << opt<type::flag>("-a", "--all", "Newton's Inspiration")
          << opt<type::flag>("-b", "--beta_mode", "For testing")
          << opt<type::flag>("-f", "Force the situation")
          << opt<type::numeric>("-n", "--count", "How many times to repeat")
          << param<type::string, 0, 1>("Label to use")

          << cmd("walk",
                 [](auto &c) {
                   c << opt<type::numeric>("--stride", "")
                     << cmd("through", [](auto &c) {
                          c << opt<type::unitary>("--speed",
                                                  "Speed of movement")
                            << param<type::string, 1>("Comments");
                        });
                 })

          << cmd("fly",
                 [](auto &c) {
                   c << opt<type::flag>("--fast", "Rapid movement")
                     << cmd("through", [](auto &c) {
                          c << opt<type::unitary>("--speed",
                                                  "Speed of movement")
                            << param<type::string, 1>("Comments");
                        });
                 },
                 [](auto &cob) { cout << "RUNNABLE for fly invoked" << endl; })

          << [](auto &cob) { cout << "MAIN RUUNNABLE invoked" << endl; };

  main_args_t a1{"-a", "-f", "walk", "--stride", "through", "--speed", "10"};
  main_args_t a2{"-a", "-f", "fly", "--fast", "through", "--speed", "200"};

  cout << "*** a1 main_args_t hard coded:" << endl;
  argline(a1);
  
  cout << endl << "*** a2 main_args_t hard coded:" << endl;
  argline(a2);

  cout << endl << endl << "*** from cli parms:" << endl;
  return argline(argc, argv);
}
