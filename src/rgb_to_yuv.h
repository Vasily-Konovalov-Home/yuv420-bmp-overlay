#ifndef YUV420_BMP_OVERLAY_RGB_TO_YUV_H
#define YUV420_BMP_OVERLAY_RGB_TO_YUV_H

#include "bmp_reader.h"
#include "yuv_rw.h"

/*
 *  Коэффициенты для преобразования RGB - YUV420
 *  Документация: ITU-R BT.601-7, JPEG T-REC-T.871
 */
struct YuvCoefficients {
    float yr, yg, yb;
    float ur, ug, ub;
    float vr, vg, vb;
};

/*  Коэфициенты выведены в Mathcad
 *  и совпали со значениями из Википедии для JPEG T-REC-T.871
 */
constexpr YuvCoefficients BT601_COEFFS = {
    0.299f,     0.587f,     0.114f,      // Y
   -0.1687f,   -0.3313f,    0.5f,        // U
    0.5f,      -0.4187f,   -0.0813f       // V
};

/*
 *  convertRgbToYuv420()
 *  Преобразуем BMP в YUV420
 *
 *  BMP хранит строки снизу вверх. Результат разворачивается:
 *  YUVFrame.y[0] = верхняя строка
 *  По умолчанию коэффициенты BT.601
 *  Во время тестов (пипеткой в paint) они показали себя хорошо
 *  с разбросом в +-1 RGB в масштабе (0-255)
 */
YUVFrame convertRgbToYuv420(const BmpImg& bmp,
                             const YuvCoefficients& coeffs = BT601_COEFFS);

#endif //YUV420_BMP_OVERLAY_RGB_TO_YUV_H
