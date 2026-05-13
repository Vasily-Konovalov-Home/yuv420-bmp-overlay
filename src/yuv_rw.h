#ifndef YUV420_BMP_OVERLAY_YUV_RW_H
#define YUV420_BMP_OVERLAY_YUV_RW_H

#include <cstdint>
#include <vector>
#include <fstream>

struct YUVFrame {
    int width;
    int height;
    std::vector<uint8_t> y;
    std::vector<uint8_t> u;
    std::vector<uint8_t> v;
};

/*
 *  readYUVFrame()
 *  Читаем один кадр YUV420
 *  false если кадров больше нет
 *  бросаем исключение если кадр не полный
 */
bool readYUVFrame(std::ifstream& inFile, YUVFrame& frame, int width, int height);

/*
 *  writeYUVFrame()
 *  аписываем один кадр в YUV420
 */
void writeYUVFrame(std::ofstream& outFile, const YUVFrame& frame);

#endif //YUV420_BMP_OVERLAY_YUV_RW_H
