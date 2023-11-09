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
#define FMT_NAME "avfoundation"
#define FMT_PACKET_SIZE 2048
#define BUFFER_SIZE 1024
#define VIDEO_FMT_NV12_SIZE 460800
#define VIDEO_FMT_DEFAULT_SIZE 614400
#define BUFFER_SIZE 1024
#define FMT_CAMERA "0"
#define FMT_DESK "1"
#define FMT_V_WIDTH 640
#define FMT_V_HEIGHT 480

#include <iostream>
#include <unistd.h>
#include <cstring>

namespace video {
    class video {

    public:
        void fmtDefault(const char *file_name, int record_time);

        void fmtNv12(const char *file_name, int record_time);

        void fmtH264(const char *file_name, int record_time);
    };
}


#endif //VIDEO_LEARN_VIDEO_H
