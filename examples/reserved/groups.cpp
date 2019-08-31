#include <hekate>
#include <hekate/timer>

int main(int argc, char **argv) {
    hekate::AutoTimer("This is a timer");

    hekate::App app("K3Pi goofit fitter");

    std::string file;
    hekate::Option *opt = app.add_option("-f,--file,file", file, "File name")->required()->group("Important");

    int count;
    hekate::Option *copt = app.add_flag("-c,--count", count, "Counter")->required()->group("Important");

    double value; // = 3.14;
    app.add_option("-d,--double", value, "Some Value")->group("Other");

    try {
        app.parse(argc, argv);
    } catch(const hekate::ParseError &e) {
        return app.exit(e);
    }

    std::cout << "Working on file: " << file << ", direct count: " << app.count("--file")
              << ", opt count: " << opt->count() << std::endl;
    std::cout << "Working on count: " << count << ", direct count: " << app.count("--count")
              << ", opt count: " << copt->count() << std::endl;
    std::cout << "Some value: " << value << std::endl;

    return 0;
}
