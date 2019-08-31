// Modify the help print so that argument values are accessible
// Note that this will not shortcut `->required` and other similar options

#include <hekate>
#include <iostream>

int main(int argc, char ** const argv) {
    hekate::App test;

    // Remove help flag because it shortcuts all processing
    test.set_help_flag();

    // Add custom flag that activates help
    auto help = test.add_flag("-h,--help", "Request help");

    std::string some_option;
    test.add_option("-a", some_option, "Some description");

    try {
        test.parse(argc, argv);
        if(*help)
            throw hekate::CallForHelp();
    } catch(const hekate::Error &e) {
        std::cout << "Option string:" << some_option << std::endl;
        return test.exit(e);
    }

    std::cout << "Option string:" << some_option << std::endl;
    return 0;
}
