#ifndef YUV420_BMP_OVERLAY_CONFIG_H
#define YUV420_BMP_OVERLAY_CONFIG_H

#include <string>

struct Config {
    std::string bmpPath;
    std::string inputYuvPath;
    std::string outputYuvPath;
    int videoWidth;
    int videoHeight;
    int overlayOffsetX = 0;
    int overlayOffsetY = 0;
    int numThreads = 0; //  0 = авто (hardware_concurrency)
};

/*
 *  parseConfig()
 *  Заполняем структуру Конфиг,
 *  Принимаем колличество аргументов и сами аргументы,
 *  На выходе заполненная структура
 */
Config parseConfig(int argc, char *argv[]);

#endif //YUV420_BMP_OVERLAY_CONFIG_H
