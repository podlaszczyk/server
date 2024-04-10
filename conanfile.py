from conans import ConanFile, CMake


class GameConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "catch2/3.4.0", "fakeit/2.4.0 "
    generators = "cmake_find_package"
