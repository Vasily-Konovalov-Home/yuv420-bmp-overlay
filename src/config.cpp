#include "config.h"
#include <iostream>
#include <cstdlib>
#include <stdexcept>

constexpr int REQUIRED_ARGC = 2;

Config parseConfig(const int argc, char* argv[]) {
    if (argc < REQUIRED_ARGC) {
        throw std::runtime_error ("Usage: " + std::string(argv[0]) + " <bmp_file>");
    }

    Config myConfig;
    myConfig.bmpPath = argv[1];

    return myConfig;
}