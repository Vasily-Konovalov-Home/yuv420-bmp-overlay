#include "config.h"

#include <iostream>
#include <stdexcept>
#include <string>

constexpr int REQUIRED_ARGC = 6; //  program + bmp + input + output + width + height

Config parseConfig(int argc, char *argv[]) {
    if (argc < REQUIRED_ARGC) {
        throw std::runtime_error(
            "Usage: " + std::string(argv[0]) +
            " <bmp_file> <input_yuv> <output_yuv> <width> <height> [offsetX] [offsetY]");
    }

    Config cfg;
    cfg.bmpPath = argv[1];
    cfg.inputYuvPath = argv[2];
    cfg.outputYuvPath = argv[3];
    cfg.videoWidth = std::stoi(argv[4]);
    cfg.videoHeight = std::stoi(argv[5]);

    if (argc >= 7) {
        cfg.overlayOffsetX = std::stoi(argv[6]);
    }
    if (argc >= 8) {
        cfg.overlayOffsetY = std::stoi(argv[7]);
    }

    return cfg;
}
