//
// Created by 张耀华 on 2023/11/7.
//

#include "video.h"

void video::haha() {
    std::cout << "Hello, World!" << std::endl;
    av_log_set_level(AV_LOG_DEBUG);//设置日志级别
    av_log(nullptr, AV_LOG_INFO, "hello ffmpeg\n");//打印日志
    const char *info = avcodec_configuration();//avcodec配置信息
    av_log(nullptr, AV_LOG_DEBUG, "配置:%s", info);
    std::cout << "video_test hha" << std::endl;
}
