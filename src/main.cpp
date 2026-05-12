#include "config.h"
#include "bmp_reader.h"
#include <iostream>
#include <stdexcept>
#include <cstdlib>

//  Тест readBMP
int main(const int argc, char* argv[])
{
    try {
        Config cfg = parseConfig(argc, argv);
        BmpImg bmp = readBMP(cfg.bmpPath);

        std::cout << "BMP loaded successfully:" << std::endl;
        std::cout << "  Width:  " << bmp.width  << std::endl;
        std::cout << "  Height: " << bmp.height << std::endl;
        std::cout << "  RGB data size: " << bmp.rgbData.size() << " bytes" << std::endl;

        if (!bmp.rgbData.empty()) {
            const uint8_t* firstPixel = bmp.rgbData.data();
            const uint8_t* lastPixel  = bmp.rgbData.data() + bmp.rgbData.size() - 3;

            std::cout << "  First pixel (bottom-left, B G R): "
                      << static_cast<int>(firstPixel[0]) << " "
                      << static_cast<int>(firstPixel[1]) << " "
                      << static_cast<int>(firstPixel[2]) << std::endl;

            std::cout << "  Last pixel  (top-right, B G R):   "
                      << static_cast<int>(lastPixel[0]) << " "
                      << static_cast<int>(lastPixel[1]) << " "
                      << static_cast<int>(lastPixel[2]) << std::endl;
        }

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}