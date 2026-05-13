#include "config.h"
#include "bmp_reader.h"
#include "rgb_to_yuv.h"
#include "yuv_rw.h"
#include "overlay.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>

int main(const int argc, char* argv[])
{
    try {
        // --- Аргументы ---
        // Пока жёстко прописываем пути и размеры для теста.
        // Потом заменим на parseConfig с нормальными аргументами.

        const std::string bmpPath      = "bmp_test.bmp";
        const std::string inputYuvPath = "out.yuv";
        const std::string outputYuvPath = "output.yuv";
        const int videoWidth  = 1920;  // CIF
        const int videoHeight = 1080;

        // --- 1. Читаем BMP ---
        std::cout << "Reading BMP: " << bmpPath << std::endl;
        BmpImg bmp = readBMP(bmpPath);
        std::cout << "  Size: " << bmp.width << "x" << bmp.height << std::endl;

        // --- 2. Конвертируем BMP -> YUV420 (overlay) ---
        std::cout << "Converting BMP to YUV420..." << std::endl;
        YUVFrame overlay = convertRgbToYuv420(bmp);

        // Проверяем, что overlay помещается в видео
        if (overlay.width > videoWidth || overlay.height > videoHeight) {
            throw std::runtime_error("BMP is larger than video frame");
        }

        // --- 3. Открываем входной и выходной YUV-файлы ---
        std::ifstream inFile(inputYuvPath, std::ios::binary);
        if (!inFile.is_open()) {
            throw std::runtime_error("Cannot open input YUV: " + inputYuvPath);
        }

        std::ofstream outFile(outputYuvPath, std::ios::binary);
        if (!outFile.is_open()) {
            throw std::runtime_error("Cannot open output YUV: " + outputYuvPath);
        }

        // --- 4. Обрабатываем кадры ---
        std::cout << "Processing frames..." << std::endl;
        int frameCount = 0;
        YUVFrame frame;

        while (readYUVFrame(inFile, frame, videoWidth, videoHeight)) {
            // Накладываем overlay на кадр
            overlayYuvFrame(frame, overlay, 0, 0);

            // Записываем результат
            writeYUVFrame(outFile, frame);

            ++frameCount;
        }

        std::cout << "Done. " << frameCount << " frames processed." << std::endl;
        std::cout << "Output: " << outputYuvPath << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}