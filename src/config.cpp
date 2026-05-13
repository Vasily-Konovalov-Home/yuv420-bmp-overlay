#include "config.h"

#include <iostream>
#include <stdexcept>
#include <string>

constexpr int REQUIRED_ARGC = 6; //  program + bmp + input + output + width + height

/*  parseIntArg()
 *  Безопастно парсим цифры
 */
static int parseIntArg(const char* arg, int position) {
    try {
        return std::stoi(arg);
    } catch (const std::invalid_argument&) {
        throw std::runtime_error("Invalid integer argument at position " + std::to_string(position));
    } catch (const std::out_of_range&) {
        throw std::runtime_error("Integer argument out of range at position " + std::to_string(position));
    }
}

Config parseConfig(int argc, char *argv[]) {
    if (argc < REQUIRED_ARGC) {
        throw std::runtime_error(
            "Usage: " + std::string(argv[0]) +
            " <bmp_file> <input_yuv> <output_yuv> <width> <height> [offsetX] [offsetY] [numThreads]");
    }

    Config cfg;
    cfg.bmpPath      = argv[1];
    cfg.inputYuvPath = argv[2];
    cfg.outputYuvPath = argv[3];

    //  Парсим width, height
    cfg.videoWidth  = parseIntArg(argv[4], 4);
    cfg.videoHeight = parseIntArg(argv[5], 5);

    //  Проверяем положительность размеров видео
    if (cfg.videoWidth <= 0 || cfg.videoHeight <= 0) {
        throw std::runtime_error("Width and height must be positive");
    }

    //  Опциональные параметры
    if (argc >= 7) {
        cfg.overlayOffsetX = parseIntArg(argv[6], 6);
    }
    if (argc >= 8) {
        cfg.overlayOffsetY = parseIntArg(argv[7], 7);
    }
    if (argc >= 9) {
        cfg.numThreads = parseIntArg(argv[8], 8);
    }

    return cfg;
}
