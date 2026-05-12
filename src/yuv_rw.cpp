#include "yuv_rw.h"
#include <stdexcept>
#include <string>

bool readYUVFrame(std::ifstream &inFile, YUVFrame &frame, int width, int height) {
    frame.width = width;
    frame.height = height;

    const size_t ySize = static_cast<size_t>(width) * height;
    const size_t uvSize = static_cast<size_t>(width / 2) * (height / 2);

    frame.y.resize(ySize);
    frame.u.resize(uvSize);
    frame.v.resize(uvSize);

    // Читаем Y-плоскость
    inFile.read(reinterpret_cast<char *>(frame.y.data()), ySize);
    if (!inFile) {
        return false; // конец файла
    }

    // Читаем U-плоскость
    inFile.read(reinterpret_cast<char *>(frame.u.data()), uvSize);
    if (!inFile) {
        throw std::runtime_error("Unexpected end of file while read U");
    }

    // Читаем V-плоскость
    inFile.read(reinterpret_cast<char *>(frame.v.data()), uvSize);
    if (!inFile) {
        throw std::runtime_error("Unexpected end of file while read V");
    }

    return true;
}

void writeYUVFrame(std::ofstream &outFile, const YUVFrame &frame) {
    outFile.write(reinterpret_cast<const char *>(frame.y.data()), frame.y.size());
    outFile.write(reinterpret_cast<const char *>(frame.u.data()), frame.u.size());
    outFile.write(reinterpret_cast<const char *>(frame.v.data()), frame.v.size());
}
