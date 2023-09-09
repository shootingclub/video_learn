// 音频采样
// my_header.h
#ifndef AUDIO_SAMPLING_H
#define AUDIO_SAMPLING_H

#include <unistd.h>
#include <string>

static int rec_status = 1;

extern "C" { //C++中特殊处理
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavutil/log.h"
#include "libavcodec/avcodec.h"
}

//#include "libavdevice/avdevice.h"
//#include "libavformat/avformat.h"
//#include "libavutil/log.h"
//#include "libavcodec/avcodec.h"

class Audio {
private:
//    int audio_recoder_time_;  // 录制时间
//    const char *base_path_; // 录制路径
public:
//    Audio(const char *base_path_, int audio_recoder_time) :
//            base_path_(base_path_), audio_recoder_time_(audio_recoder_time) {}

    // 音频录制
    static void record(const char *file_name, int record_time);

protected:
    // 保护成员变量
    int myProtectedVar;
};

void Audio::record(const char *file_name, int record_time) {
    //1
    // 注册设备
    avdevice_register_all();
    // 2
    // 使用该函数可以方便地确定FFmpeg是否支持特定的输入媒体格式，以便在处理输入流之前进行验证和选择正确的输入格式
    // 同的平台获取到的输入设备方式不一样：
    // macos 使用 avfoundation
    // windows 使用 dshow
    // linux 使用 alsa
    const AVInputFormat *avInputFormat = av_find_input_format("avfoundation");
    //3
    int ret;
    char errors[1024];
    AVFormatContext *fmt_ctx = NULL;
    // 给以个目标地址获取音视频流，注释: ":0" 获取本地外置麦克风获取音频流
    // 可以是一个网络地址，本地文件，对于设备给一个设备名字就行 获取音频流
    const char *url = ":0";
    // avInputFormat 输入设备方式的参数设置
    AVDictionary *options = NULL;
    // 打开输入设备，准备开始录音
    ret = avformat_open_input(&fmt_ctx, url, avInputFormat, &options);
    if (ret < 0) {
        av_strerror(ret, errors, 1024);
        printf("Fail to open audio device ,[%d],%s\n", ret, errors);
        return;

    }
    // 创建文件
    FILE *outfile = fopen(file_name, "wb+");
    //4
    // 初始化av_packet 读取设备音频包
    AVPacket pkt;
    ret = av_new_packet(&pkt, 2048);
    if (ret < 0) {
        av_strerror(ret, errors, 1024);
        printf("init av_new_packet is error ,[%d],%s\n", ret, errors);
        return;

    }

    // 读取多少秒
    while (record_time) {
        ret = av_read_frame(fmt_ctx, &pkt);
        // 这里可能读取的时候，ret返回-35 表示设备还没准备好, 先睡眠1s
        if (ret == -35) {
            sleep(1);
            continue;
        }
        if (ret < 0) {
            break;
        }
        av_log(nullptr, AV_LOG_INFO, "pkt size is %d (%p) record_time %d \n", pkt.size, pkt.data, record_time);

        // 将读取到的声音二进制写进去文件
        fwrite(pkt.data, pkt.size, 1, outfile);
        fflush(outfile);

        av_packet_unref(&pkt);
        --record_time;
    }

    //关闭文件流
    fclose(outfile);

    // 录制结束 释放上下文 release ctx
    avformat_close_input(&fmt_ctx);
    av_log(nullptr, AV_LOG_DEBUG, "finish \n");
}

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








