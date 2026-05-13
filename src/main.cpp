#include "config.h"
#include "bmp_reader.h"
#include "rgb_to_yuv.h"
#include "yuv_rw.h"
#include "overlay.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdlib>

int main(const int argc, char* argv[])
{
    try {
        Config cfg = parseConfig(argc, argv);

        // BMP  overlay YUV
        std::cout << "Reading BMP: " << cfg.bmpPath << std::endl;
        BmpImg bmp = readBMP(cfg.bmpPath);
        std::cout << "  Size: " << bmp.width << "*" << bmp.height << std::endl;

        if (bmp.width > cfg.videoWidth || bmp.height > cfg.videoHeight) {
            throw std::runtime_error("BMP is larger than video frame");
        }

        std::cout << "Converting BMP to YUV420 (BT.601 studio range)..." << std::endl;
        YUVFrame overlay = convertRgbToYuv420(bmp);

        // Открываем файлы
        std::ifstream inFile(cfg.inputYuvPath, std::ios::binary);
        if (!inFile.is_open()) {
            throw std::runtime_error("Cannot open input YUV: " + cfg.inputYuvPath);
        }

        std::ofstream outFile(cfg.outputYuvPath, std::ios::binary);
        if (!outFile.is_open()) {
            throw std::runtime_error("Cannot open output YUV: " + cfg.outputYuvPath);
        }

        // Обрабатываем кадры
        std::cout << "Processing frames..." << std::endl;
        int frameCount = 0;
        YUVFrame frame;

        while (readYUVFrame(inFile, frame, cfg.videoWidth, cfg.videoHeight)) {
            overlayYuvFrame(frame, overlay, cfg.overlayOffsetX, cfg.overlayOffsetY);
            writeYUVFrame(outFile, frame);
            ++frameCount;
        }

        std::cout << "Done. " << frameCount << " frames processed." << std::endl;
        std::cout << "Output: " << cfg.outputYuvPath << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}