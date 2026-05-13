#ifndef YUV420_BMP_OVERLAY_OVERLAY_H
#define YUV420_BMP_OVERLAY_OVERLAY_H

#include "yuv_rw.h"

/*
 *  overlayYuvFrame()
 *  Накладывает маленький кадр overlay на большой кадр base
 *  по умолчанию кадр overlay размещается в левом верхнем углу кадра base
 *  Копируются все три плоскости (Y, U, V).
 *  Если размеры overlay не кратны 2-м, тогда обрезаем его
 *  сли offset не кратны 2-м, тогда округляем
 *
 *  Бросаем исключения если
 *  width и height overlay должны быть <= width и height base
 */
void overlayYuvFrame(YUVFrame& base, const YUVFrame& overlay,
                     int offsetX = 0, int offsetY = 0);

#endif //YUV420_BMP_OVERLAY_OVERLAY_H
