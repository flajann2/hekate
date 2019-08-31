from conans import ConanFile, CMake, tools


class HekateConan(ConanFile):
    name = "hekate"
    version = "0.0.1"
    license = "MIT"
    author = "Fred Mitchell fred.mitchell@gmx.de"
    url = "https://github.com/flajann2/hekate.git"
    description = "Hekate powerful Command Line Parser"
    topics = ("command line", "parser", "powerful", "DSL")
    exports_sources = "include/*"
    no_copy_source = True

    def package(self):
        self.copy("*.h", dst="include", src="hekate")

    def package_info(self):
        self.cpp_info.libs = ["hekate"]

