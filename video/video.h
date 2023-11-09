//
// Created by 张耀华 on 2023/11/7.
//

#ifndef VIDEO_LEARN_VIDEO_H
#define VIDEO_LEARN_VIDEO_H
#define __STDC_CONSTANT_MACROS

extern "C" { //C++中特殊处理
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavutil/log.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h" // 从采样
}

#include <iostream>

class video {

public:
    void haha();
};


#endif //VIDEO_LEARN_VIDEO_H
