#include "config.h"
#include "bmp_reader.h"
#include "rgb_to_yuv.h"
#include "yuv_rw.h"
#include "overlay.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

int main(const int argc, char *argv[]) {
    try {
        Config cfg = parseConfig(argc, argv);

        // BMP overlay YUV
        std::cout << "Reading BMP: " << cfg.bmpPath << std::endl;
        BmpImg bmp = readBMP(cfg.bmpPath);
        std::cout << "  Size: " << bmp.width << "*" << bmp.height << std::endl;

        if (bmp.width > cfg.videoWidth || bmp.height > cfg.videoHeight) {
            throw std::runtime_error("BMP is larger than video frame");
        }

        // Конвертация с замером времени
        int actualThreads = cfg.numThreads;
        if (actualThreads == 0) {
            actualThreads = std::thread::hardware_concurrency();
            if (actualThreads <= 0) actualThreads = 2;
        }

        std::cout << "Converting BMP to YUV420 (BT.601 studio range)";
        std::cout << " [" << actualThreads << " threads]";
        std::cout << "..." << std::endl;

        auto convertStart = std::chrono::steady_clock::now();

        YUVFrame overlay = convertRgbToYuv420(bmp, cfg.numThreads);

        auto convertEnd = std::chrono::steady_clock::now();
        auto convertMs = std::chrono::duration_cast<std::chrono::milliseconds>(convertEnd - convertStart).count();
        std::cout << "  Conversion took: " << convertMs << " ms" << std::endl;

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
        auto processStart = std::chrono::steady_clock::now();

        while (readYUVFrame(inFile, frame, cfg.videoWidth, cfg.videoHeight)) {
            overlayYuvFrame(frame, overlay, cfg.overlayOffsetX, cfg.overlayOffsetY);
            writeYUVFrame(outFile, frame);
            ++frameCount;
        }

        auto processEnd = std::chrono::steady_clock::now();
        auto processMs = std::chrono::duration_cast<std::chrono::milliseconds>(processEnd - processStart).count();

        std::cout << "Done. " << frameCount << " frames processed in " << processMs << " ms" << std::endl;
        std::cout << "Output: " << cfg.outputYuvPath << std::endl;

        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
