#include "config.h"
#include "bmp_reader.h"
#include "yuv_rw.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

int main(const int argc, char *argv[]) {
    try {
        // Парсим аргументы (пока только путь к BMP)
        Config cfg = parseConfig(argc, argv);

        // Тест 1: читаем BMP
        BmpImg bmp = readBMP(cfg.bmpPath);

        std::cout << "BMP loaded: " << bmp.width << "*" << bmp.height
                << std::endl << bmp.rgbData.size() << " bytes)" << std::endl;

        // Тест 2: открываем YUV и читаем первый кадр
        // Для теста размеры для CYF 352*288
        std::ifstream YUVFile("akiyo_cif.yuv", std::ios::binary);
        if (!YUVFile.is_open()) {
            std::cerr << "Info: YUV file not found" << std::endl;
        } else {
            YUVFrame frame;
            const int TEST_WIDTH = 352;
            const int TEST_HEIGHT = 288;

            if (readYUVFrame(YUVFile, frame, TEST_WIDTH, TEST_HEIGHT)) {
                std::cout << "YUV frame read: " << frame.width << "*" << frame.height << std::endl;
                std::cout << "  TEST_WIDTH * TEST_HEIGHT:  " << TEST_WIDTH * TEST_HEIGHT << std::endl;
                std::cout << "  Y size:  " << frame.y.size() << " bytes" << std::endl;
                std::cout << "  TEST_WIDTH/2 * TEST_HEIGHT/2:  " << TEST_WIDTH / 2 * TEST_HEIGHT / 2 << std::endl;
                std::cout << "  U size:  " << frame.u.size() << " bytes" << std::endl;
                std::cout << "  V size:  " << frame.v.size() << " bytes" << std::endl;
            }
        }

        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
