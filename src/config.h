#ifndef YUV420_BMP_OVERLAY_CONFIG_H
#define YUV420_BMP_OVERLAY_CONFIG_H

#include <string>

struct Config {
    std::string bmpPath;
};

/*  Заполняем структуру Конфиг,
 *  Принимаем колличество аргументов и сами аргументы,
 *  На выходе заполненная структура
 */
Config parseConfig(int argc, char* argv[]);

#endif //YUV420_BMP_OVERLAY_CONFIG_H
