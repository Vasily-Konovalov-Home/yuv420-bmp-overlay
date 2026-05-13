#ifndef YUV420_BMP_OVERLAY_BMP_READER_H
#define YUV420_BMP_OVERLAY_BMP_READER_H

#include <string>
#include <vector>
#include <cstdint>

struct BmpImg {
    int width;
    int height;
    std::vector<uint8_t> rgbData;
};

/*
 *  Читаем BMP файл, проверяем формат
 *  (по заданию: формате RGB 24 бит/пиксел без альфа канала,
 *  без палитры, без компрессии)
 *  Заполняем структуру BmpImg (размеры и rgb данные)
 *  Возвращаем BmpImg
 */

BmpImg readBMP(const std::string &bmpPath);

#endif //YUV420_BMP_OVERLAY_BMP_READER_H
