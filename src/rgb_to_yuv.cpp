#include "rgb_to_yuv.h"

#include <stdexcept>
#include <string>

static uint8_t clampToUint8(int value)
{
    if (value < 0)   return 0;
    if (value > 255) return 255;
    return static_cast<uint8_t>(value);
}

YUVFrame convertRgbToYuv420(const BmpImg& bmp, const YuvCoefficients& c)
{
    if (bmp.width % 2 != 0 || bmp.height % 2 != 0) {
        throw std::runtime_error(
            "BMP width and height must be even for YUV420 conversion");
    }

    const int width  = bmp.width;
    const int height = bmp.height;

    YUVFrame yuv;
    yuv.width  = width;
    yuv.height = height;

    const size_t ySize  = static_cast<size_t>(width) * height;
    const size_t uvSize = static_cast<size_t>(width / 2) * (height / 2);

    yuv.y.resize(ySize);
    yuv.u.resize(uvSize);
    yuv.v.resize(uvSize);

    //  Константы для студийного диапазона BT.601
    constexpr float Y_SCALE  = 219.0f / 255.0f;   //    219 уровней яркости (219 = 235 (Белый по докам) - 16 (Черный))
    constexpr float Y_OFFSET = 16.0f;

    constexpr float UV_SCALE  = 224.0f / 255.0f;  //    224 уровня цветности (224 = 240 (максимум цвета по докам) - 16 (минимум))
    constexpr float UV_OFFSET = 128.0f; //  отсутствие цвета (224/2 + 16 = 128)

    for (int blockY = 0; blockY < height / 2; ++blockY) {
        for (int blockX = 0; blockX < width / 2; ++blockX) {

            const int baseX = blockX * 2;
            const int baseY = blockY * 2;

            // BMP-буфер: строка 0 - низ
            const int bmpRowBottom = baseY;
            const int bmpRowTop    = baseY + 1;

            const size_t rowSize = static_cast<size_t>(width) * 3;

            const uint8_t* rowBottom = bmp.rgbData.data() + static_cast<size_t>(bmpRowBottom) * rowSize;
            const uint8_t* rowTop    = bmp.rgbData.data() + static_cast<size_t>(bmpRowTop)    * rowSize;

            const uint8_t* px[4] = {
                rowBottom + (baseX + 0) * 3,  //    p00 - левый нижний
                rowBottom + (baseX + 1) * 3,  //    p10 - правый нижний
                rowTop    + (baseX + 0) * 3,  //    p01 - левый верхний
                rowTop    + (baseX + 1) * 3   //    p11 - правый верхний
            };

            //  Суммируем R, G, B по блоку 2*2 для U и V
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

                int Yi = static_cast<int>(Yf + 0.5f);  //   округление
                uint8_t Y_byte = clampToUint8(Yi);

                //  Индекс в Y-плоскости: разворачиваем BMP (низ=0) в YUV (верх=0)
                int pixelX = baseX + (p % 2);      // 0 или 1
                int pixelY = baseY + (p / 2);      // 0 или 1
                int yuvRow = height - 1 - pixelY;
                yuv.y[static_cast<size_t>(yuvRow) * width + pixelX] = Y_byte;
            }

            //  U и V из средних R, G, B блока, в студийном диапазоне [16, 240]
            float avgR = static_cast<float>(sumR) / 4.0f;
            float avgG = static_cast<float>(sumG) / 4.0f;
            float avgB = static_cast<float>(sumB) / 4.0f;

            float Uf = c.ur * avgR + c.ug * avgG + c.ub * avgB;
            float Vf = c.vr * avgR + c.vg * avgG + c.vb * avgB;

            //  Масштабируем: [-128, 127] → [16, 240] с центром 128
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

    return yuv;
}