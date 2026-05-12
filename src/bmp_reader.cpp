#include "bmp_reader.h"
#include <fstream>
#include <string>
#include <stdexcept>
#include <iostream>
#include <cstdlib>

//  Структуры для заголовков bmp
#pragma pack(push, 1)
//  Ссылка на док-цию:  https://learn.microsoft.com/ru-ru/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader
struct BmpFileHeader {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};

//  Ссылка на док-цию:  https://learn.microsoft.com/ru-ru/windows/win32/api/wingdi/ns-wingdi-bitmapinfoheader
struct BmpInfoHeader {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)

/*
 *  Константы для проверки с ТЗ
 *  (Предполагается, что BMP файл содержит только данные
 *  в формате RGB 24 бит/пиксел без альфа канала,
 *  без палитры, без компрессии.)
 */

constexpr uint16_t BMP_TYPE = 0x4D42; //  BM
constexpr uint16_t BMP_INFO_HEADER_SIZE = 0x28; //  40
constexpr uint16_t BMP_BIT_PER_PIXEL = 0x18; //  24
constexpr uint32_t BMP_COMPRESSION_NULL = 0;
constexpr uint32_t BMP_PALETTED_NULL = 0;

/*
 *  validateBMP()
 *  Проверяем BMP на соответствие с условиями
 *  При нарушении бросаем std::runtime_error
 */
static void validateBMP(const BmpFileHeader &fileHeader,
                        const BmpInfoHeader &infoHeader) {
    if (fileHeader.bfType != BMP_TYPE) {
        throw std::runtime_error("Not a BMP file");
    }
    if (infoHeader.biSize != BMP_INFO_HEADER_SIZE) {
        throw std::runtime_error("Unsupported BMP header size");
    }
    if (infoHeader.biBitCount != BMP_BIT_PER_PIXEL) {
        throw std::runtime_error("Only 24 bit BMP supported");
    }
    if (infoHeader.biCompression != BMP_COMPRESSION_NULL) {
        throw std::runtime_error("Unsupported BMP compression");
    }
    if (infoHeader.biClrUsed != BMP_PALETTED_NULL) {
        throw std::runtime_error("Unsupported BMP paletted");
    }
    if (infoHeader.biHeight<=0 || infoHeader.biWidth <= 0) {
        throw std::runtime_error("Invalid BMP height or width");
    }
}

BmpImg readBMP(const std::string &bmpPath) {
    std::ifstream bmpFile(bmpPath, std::ios::binary);
    if (!bmpFile.is_open()) {
        throw std::runtime_error("Could not open bmp file: " + bmpPath);
    }

    BmpFileHeader fileHeader{};
    BmpInfoHeader infoHeader{};

    bmpFile.read(reinterpret_cast<char *>(&fileHeader), sizeof(fileHeader));
    bmpFile.read(reinterpret_cast<char *>(&infoHeader), sizeof(infoHeader));

    validateBMP(fileHeader, infoHeader);

    int rowSizeRaw = infoHeader.biWidth * 3;    //  Red byte + Green byte + Blue byte = 3
    int rowSizePadded = ((rowSizeRaw + 3) / 4) * 4;
    int padding = rowSizePadded - rowSizeRaw;

    BmpImg myBmp{};
    myBmp.width = infoHeader.biWidth;
    myBmp.height = infoHeader.biHeight;
    myBmp.rgbData.resize(static_cast<size_t>(myBmp.width * myBmp.height * 3));

    for (int row = 0; row < myBmp.height; row++) {
        uint8_t* dst = myBmp.rgbData.data() + static_cast<size_t>(row * rowSizeRaw);

        bmpFile.read(reinterpret_cast<char*>(dst), rowSizeRaw);

        if (padding > 0) {
            bmpFile.seekg(padding, std::ios::cur);
        }

        if (!bmpFile) {
            throw std::runtime_error("Unexpected end of file " + std::to_string(row));
        }
    }

    return myBmp;
}
