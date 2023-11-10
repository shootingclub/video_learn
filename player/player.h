

#ifndef VIDEO_LEARN_PLAYER_H
#define VIDEO_LEARN_PLAYER_H

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

#include <iostream>
#include <unistd.h>
#include <cstring>

namespace player {
    class player_test {


    };
}


#endif //VIDEO_LEARN_PLAYER_H
