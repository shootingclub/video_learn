
#define __STDC_CONSTANT_MACROS

extern "C" { //C++中特殊处理
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavutil/log.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h" // 从采样
}

#include <iostream>


int main() {

    std::cout << "Hello, World!" << std::endl;
    av_log_set_level(AV_LOG_DEBUG);//设置日志级别
    av_log(nullptr, AV_LOG_INFO, "hello ffmpeg\n");//打印日志
    const char *info = avcodec_configuration();//avcodec配置信息
    av_log(nullptr, AV_LOG_DEBUG, "配置:%s", info);

    return 0;
}

