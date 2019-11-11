#include <hekate>
#include <iostream>

using namespace hekate;
using namespace std;

int main(int argc, char **argv) {
  cli argline;
  argline << opt<ilk::flag>("-a", "--all", "Newton's Inspiration")
          << opt<ilk::flag>("-b", "--beta_mode", "For testing")
          << opt<ilk::flag>("-f", "Force the situation")
          << opt<ilk::integer>("-n", "--count", "How many times to repeat")
          << opt<ilk::string>("-s", "--string", "Just an ordinary string")
          << param<ilk::string, 0, 1>("Label to use")

          << cmd(
                 "walk",
                 [](auto &c) {
                   c << opt<ilk::integer>("--stride", "")
                     << cmd("through", [](auto &c) {
                          c << opt<ilk::unitary>("--speed",
                                                  "Speed of movement")
                            << param<ilk::string, 1>("Comments");
                        });
                 },
                 nullptr, "You have to walk before you can run")

          << cmd(
                 "fly",
                 [](auto &c) {
                   c << opt<ilk::flag>("--fast", "Rapid movement")
                     << cmd("through", [](auto &c) {
                          c << opt<ilk::unitary>("--speed",
                                                  "Speed of movement")
                            << param<ilk::string, 1>("Comments");
                        });
                 },
                 [](auto &cob) {
                   cout << "RUNNABLE for fly invoked" << endl;
                 },
                 "When you are ready to fly, the world is your oyster!")
          << [](auto &cob) {
               cout << "MAIN RUUNNABLE invoked" << endl;
             };

  main_args_t a1{{"-a", "-f", "walk", "--stride", "through", "--speed", "10"}};
  main_args_t a2{{"-a", "-f", "fly", "--fast", "through", "--speed", "200"}};
  main_args_t a3{{"-a", "-f", "fly", "--fast", "through", "--speed=500"}};
  main_args_t a4{{"-af",      "fly", "--fast", "through", "--speed=500"}};

  cout << "*** a1 main_args_t hard coded:" << endl;
  argline(a1);
  
  cout << endl << "*** a2 main_args_t hard coded:" << endl;
  argline(a2);

  cout << endl << "*** a3 main_args_t hard coded:" << endl;
  argline(a3);

  cout << endl << "*** a4 main_args_t hard coded:" << endl;
  argline(a4);

  cout << endl << endl << "*** from cli parms:" << endl;
  return argline(argc, argv);
}
