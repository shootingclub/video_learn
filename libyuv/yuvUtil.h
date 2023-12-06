//
// Created by 张耀华 on 2023/12/6.
//

#ifndef VIDEO_LEARN_YUVUTIL_H
#define VIDEO_LEARN_YUVUTIL_H

#include <stdio.h>
#include <stdint.h>


extern "C" {
#include "libyuv.h"
}
namespace yuvUtil {

    class yuvUtil {

        void scale(uint8_t *srcYuvData, uint8_t *dstYuvData, int width, int height, int dstWidth, int dstHeight);

    };
}


//int main() {
//    uint32_t width = 700, height = 700;
//    uint32_t dstWidth = 100, dstHeight = 100;
//    uint8_t YUV[width * height * 3 / 2];
//    uint8_t YUV_SCALE[dstWidth * dstHeight * 3 / 2];
//
//    FILE *yuv420pFile = fopen("/Users/staff/Desktop/rainbow-yuv420p.yuv", "rb");
//    fread(YUV, sizeof(YUV), 1, yuv420pFile);
//
//    scale(YUV, YUV_SCALE, width, height, dstWidth, dstHeight);
//
//    FILE *yuvScaleFile = fopen("/Users/staff/Desktop/rainbow-yuv420p-scale-6.yuv", "wb");
//    fwrite(YUV_SCALE, sizeof(YUV_SCALE), 1, yuvScaleFile);
//
//    fclose(yuvScaleFile);
//    fclose(yuv420pFile);
//    return 0;
//}

#endif //VIDEO_LEARN_YUVUTIL_H
