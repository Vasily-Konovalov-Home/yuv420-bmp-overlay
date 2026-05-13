#include "overlay.h"
#include <stdexcept>
#include <string>

void overlayYuvFrame(YUVFrame& base, const YUVFrame& overlay,
                     const int offsetX, const int offsetY)
{
    // Насильно округляем смещение вниз до чётного
    int adjustedOffsetX = (offsetX / 2) * 2;
    int adjustedOffsetY = (offsetY / 2) * 2;

    // Насильно обрезаем размеры до чётных
    int adjustedOverlayWidth = (overlay.width / 2) * 2;
    int adjustedOverlayHeight = (overlay.height / 2) * 2;

    if (adjustedOffsetX < 0 || adjustedOffsetY < 0 ||
        adjustedOffsetX + adjustedOverlayWidth > base.width ||
        adjustedOffsetY + adjustedOverlayHeight > base.height) {
        throw std::runtime_error("Overlay > then Base frame");
    }

    // Y-плоскость
    for (int row = 0; row < adjustedOverlayHeight; ++row) {
        const uint8_t* srcRow = overlay.y.data() + static_cast<size_t>(row) * overlay.width;
        uint8_t*       dstRow = base.y.data()    + static_cast<size_t>(adjustedOffsetY + row) * base.width + adjustedOffsetX;

        for (int col = 0; col < adjustedOverlayWidth; ++col) {
            dstRow[col] = srcRow[col];
        }
    }

    // U и V плоскости
    const int uvWidth   = adjustedOverlayWidth  / 2;
    const int uvHeight  = adjustedOverlayHeight / 2;
    const int uvOffsetX = adjustedOffsetX / 2;
    const int uvOffsetY = adjustedOffsetY / 2;

    for (int row = 0; row < uvHeight; ++row) {
        const uint8_t* srcRowU = overlay.u.data() + static_cast<size_t>(row) * (overlay.width / 2);
        uint8_t*       dstRowU = base.u.data()    + static_cast<size_t>(uvOffsetY + row) * (base.width / 2) + uvOffsetX;

        const uint8_t* srcRowV = overlay.v.data() + static_cast<size_t>(row) * (overlay.width / 2);
        uint8_t*       dstRowV = base.v.data()    + static_cast<size_t>(uvOffsetY + row) * (base.width / 2) + uvOffsetX;

        for (int col = 0; col < uvWidth; ++col) {
            dstRowU[col] = srcRowU[col];
            dstRowV[col] = srcRowV[col];
        }
    }
}