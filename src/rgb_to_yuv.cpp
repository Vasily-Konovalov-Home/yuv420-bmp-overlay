#include "rgb_to_yuv.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

//  Подстраховка
static uint8_t clampToUint8(int value)
{
    if (value < 0)   return 0;
    if (value > 255) return 255;
    return static_cast<uint8_t>(value);
}

//  Константы для студийного диапазона BT.601
//  Документация: ITU-R BT.601-7, JPEG T-REC-T.871
static constexpr float Y_SCALE   = 219.0f / 255.0f;   //  219 уровней яркости (219 = 235 (Белый) - 16 (Черный))
static constexpr float Y_OFFSET  = 16.0f;
static constexpr float UV_SCALE  = 224.0f / 255.0f;   //  224 уровня цветности (224 = 240 (максимум цвета) - 16 (минимум))
static constexpr float UV_OFFSET = 128.0f;            //  отсутствие цвета (224/2 + 16 = 128)

/*
 *  processStrip()
 *  Обрабатываем блоки 2*2 в диапазоне [startBlockY, endBlockY).
 *  Потокобезопасна при условии непересекающихся диапазонов по blockY.
 *  Пишем напрямую в плоскости Y, U, V итогового кадра.
 */
static void processStrip(const BmpImg& bmp,
                         YUVFrame& yuv,
                         const YuvCoefficients& c,
                         const int startBlockY,
                         const int endBlockY,
                         const int width,
                         const int height)
{
    //  BMP хранит строки снизу вверх, без паддинга.
    //  В rgbData каждая строка плотно упакована: width * 3 байт.
    const size_t rowSize = static_cast<size_t>(bmp.width) * 3;

    for (int blockY = startBlockY; blockY < endBlockY; ++blockY) {
        for (int blockX = 0; blockX < width / 2; ++blockX) {

            const int baseX = blockX * 2;
            const int baseY = blockY * 2;

            // BMP-буфер: строка 0 - низ
            const int bmpRowBottom = baseY;
            const int bmpRowTop    = baseY + 1;

            const uint8_t* rowBottom = bmp.rgbData.data() + static_cast<size_t>(bmpRowBottom) * rowSize;
            const uint8_t* rowTop    = bmp.rgbData.data() + static_cast<size_t>(bmpRowTop)    * rowSize;

            const uint8_t* px[4] = {
                rowBottom + (baseX + 0) * 3,  // p00 - левый нижний
                rowBottom + (baseX + 1) * 3,  // p10 - правый нижний
                rowTop    + (baseX + 0) * 3,  // p01 - левый верхний
                rowTop    + (baseX + 1) * 3   // p11 - правый верхний
            };

            //  Суммируем R, G, B по блоку 2×2 для последующего усреднения U и V
            int sumR = 0, sumG = 0, sumB = 0;

            //  Обрабатываем 4 пикселя блока: вычисляем Y и накапливаем R,G,B
            for (int p = 0; p < 4; ++p) {
                //  BMP порядок: px[0]=B, px[1]=G, px[2]=R
                int B = static_cast<int>(px[p][0]);
                int G = static_cast<int>(px[p][1]);
                int R = static_cast<int>(px[p][2]);

                sumR += R;
                sumG += G;
                sumB += B;

                //  Y в студийном диапазоне [16, 235]
                float Yf = c.yr * static_cast<float>(R)
                         + c.yg * static_cast<float>(G)
                         + c.yb * static_cast<float>(B);

                //  Масштабируем: [0, 255] → [16, 235]
                Yf = Yf * Y_SCALE + Y_OFFSET;

                int Yi = static_cast<int>(Yf + 0.5f);  // округление
                uint8_t Y_byte = clampToUint8(Yi);

                //  Индекс в Y-плоскости: разворачиваем BMP (низ=0) в YUV (верх=0)
                int pixelX = baseX + (p % 2);   //  0 или 1
                int pixelY = baseY + (p / 2);   //  0 или 1
                int yuvRow = height - 1 - pixelY;
                yuv.y[static_cast<size_t>(yuvRow) * width + pixelX] = Y_byte;
            }

            //  U и V из средних R, G, B блока, в студийном диапазоне [16, 240]
            float avgR = static_cast<float>(sumR) / 4.0f;
            float avgG = static_cast<float>(sumG) / 4.0f;
            float avgB = static_cast<float>(sumB) / 4.0f;

            float Uf = c.ur * avgR + c.ug * avgG + c.ub * avgB;
            float Vf = c.vr * avgR + c.vg * avgG + c.vb * avgB;

            //  Масштабируем: [-128, 127] в [16, 240] с центром 128
            Uf = Uf * UV_SCALE + UV_OFFSET;
            Vf = Vf * UV_SCALE + UV_OFFSET;

            int Ui = static_cast<int>(Uf + 0.5f);
            int Vi = static_cast<int>(Vf + 0.5f);

            uint8_t U_byte = clampToUint8(Ui);
            uint8_t V_byte = clampToUint8(Vi);

            //  UV-плоскость: разворачиваем как Y
            int uvRow = height / 2 - 1 - blockY;
            yuv.u[static_cast<size_t>(uvRow) * (width / 2) + blockX] = U_byte;
            yuv.v[static_cast<size_t>(uvRow) * (width / 2) + blockX] = V_byte;
        }
    }
}

YUVFrame convertRgbToYuv420(const BmpImg& bmp,
                             int numThreads,
                             const YuvCoefficients& coeffs)
{
    const int width  = (bmp.width  % 2 != 0) ? bmp.width  - 1 : bmp.width;
    const int height = (bmp.height % 2 != 0) ? bmp.height - 1 : bmp.height;

    //  Предупреждаем, если размеры были обрезаны
    if (bmp.width != width || bmp.height != height) {
        std::cerr << "Warning: BMP dimensions (" << bmp.width << "x" << bmp.height
                  << ") are not even. Cropping to " << width << "x" << height
                  << " for YUV420 conversion." << std::endl;
    }

    if (width < 2 || height < 2) {
        throw std::runtime_error("BMP too small after rounding to even dimensions");
    }

    //  Автоматический выбор числа потоков
    if (numThreads == 0) {
        numThreads = static_cast<int>(std::thread::hardware_concurrency());
        if (numThreads <= 0) numThreads = 2;    //  fallback, если hardware_concurrency вернул 0
    }
    if (numThreads < 1) numThreads = 1;

    YUVFrame yuv;
    yuv.width  = width;
    yuv.height = height;
    yuv.y.resize(static_cast<size_t>(width) * height);
    yuv.u.resize(static_cast<size_t>(width / 2) * (height / 2));
    yuv.v.resize(static_cast<size_t>(width / 2) * (height / 2));

    const int totalBlocks = height / 2;

    //  Если потоков больше, чем блочных строк,тогда ограничиваем число потоков
    if (numThreads > totalBlocks) {
        numThreads = totalBlocks;
    }

    if (numThreads <= 1) {
        //  Однопоточный вариант
        processStrip(bmp, yuv, coeffs, 0, totalBlocks, width, height);
    } else {
        //  Многопоточный вариант
        //  делим блочные строки между потоками
        std::vector<std::thread> threads;
        threads.reserve(static_cast<size_t>(numThreads));

        const int blocksPerThread = totalBlocks / numThreads;
        const int remainder = totalBlocks % numThreads;

        int startBlock = 0;
        for (int t = 0; t < numThreads; ++t) {
            //  Первым (remainder) потокам достаётся на одну блочную строку больше
            int threadBlocks = blocksPerThread + (t < remainder ? 1 : 0);
            int endBlock = startBlock + threadBlocks;

            //  Запускаем поток
            threads.emplace_back(processStrip,
                                 std::cref(bmp),
                                 std::ref(yuv),
                                 std::cref(coeffs),
                                 startBlock,
                                 endBlock,
                                 width,
                                 height);

            startBlock = endBlock;
        }

        //  Ожидаем завершения всех потоков
        for (auto& th : threads) {
            th.join();
        }
    }

    return yuv;
}