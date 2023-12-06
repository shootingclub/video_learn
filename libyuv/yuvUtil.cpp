//
// Created by 张耀华 on 2023/12/6.
//

#include "yuvUtil.h"

namespace yuvUtil {
    void yuvUtil::scale(uint8_t *srcYuvData, uint8_t *dstYuvData, int width, int height, int dstWidth, int dstHeight) {
        I420Scale(
                srcYuvData,
                width,
                width*height,
        (width )/2,
                srcYuvData width *height((width 1)/2)*((height 1)/2),
                (width 1)/2,
                width,
                height,
                dstYuvData,
                dstWidth,
                dstYuvData dstWidth *dstWidth,
                (dstWidth 1)/2,
                dstYuvData dstWidth *dstHeight((dstWidth 1)/2)*((dstHeight 1)/2),
                (dstWidth 1)/2,
                dstWidth,
                dstHeight,
                kFilterNone
        );
    }
}
