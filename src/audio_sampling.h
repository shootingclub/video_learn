// 音频采样
// my_header.h
#ifndef AUDIO_SAMPLING_H
#define AUDIO_SAMPLING_H


//extern "C" { //C++中特殊处理
//#include "libavdevice/avdevice.h"
//#include "libavformat/avformat.h"
//#include "libavutil/log.h"
//#include "libavcodec/avcodec.h"
//}

#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavutil/log.h"
#include "libavcodec/avcodec.h"

void audio_sampling();

#endif






//const char *AUDIO_FORMAT_MACOS = "avfoundation";

//// 注册设备
//void register_audio_device() {
//    return avdevice_register_all();
//}
//
//// 使用该函数可以方便地确定FFmpeg是否支持特定的输入媒体格式，以便在处理输入流之前进行验证和选择正确的输入格式
//// 不同的平台使用不同的输入格式：macos 使用 avfoundation
//const AVInputFormat *format_audio() {
//    return av_find_input_format(AUDIO_FORMAT_MACOS);
//}
//
//// 打开设备获取音频流
//void open_audio_device() {
//    int ret = 0;
//    char errors[1024];
//    AVFormatContext *fmt_ctx = NULL;
//    // 给以个目标地址获取音视频流，注释: ":0" 获取本地外置麦克风获取音频流
//    const char *url = ":0";
//    AVDictionary *options = NULL;
//    ret = avformat_open_input(&fmt_ctx, url, format_audio(), &options);
//    if (ret < 0) {
//        av_strerror(ret, errors, 1024);
//        printf(reinterpret_cast<const char *>(stderr), "Fail to open audio device ,[%d],%s\n", ret, errors);
//        return;
//
//    }
//
//}








