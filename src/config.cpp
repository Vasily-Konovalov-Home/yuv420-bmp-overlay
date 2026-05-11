#include "config.h"
#include <iostream>
#include <cstdlib>

constexpr int REQUIRED_ARGC = 2;

Config parseConfig(const int argc, char* argv[]) {
    if (argc < REQUIRED_ARGC) {
        std::cerr << "Usage: " << argv[0] << " <bmp_file>" << std::endl;
        std::exit(1);
    }

    Config myConfig;
    myConfig.bmpPath = argv[1];

    return myConfig;
}