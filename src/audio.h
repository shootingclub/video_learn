// 音频采样
// my_header.h
#ifndef AUDIO_SAMPLING_H
#define AUDIO_SAMPLING_H

#include <unistd.h>
#include <string>


#define FMT_NAME "avfoundation"

extern "C" { //C++中特殊处理
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavutil/log.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h" // 从采样
}

class Audio {
private:
//    int audio_recoder_time_;  // 录制时间
//    const char *base_path_; // 录制路径
public:
//    Audio(const char *base_path_, int audio_recoder_time) :
//            base_path_(base_path_), audio_recoder_time_(audio_recoder_time) {}

    // 录制
    static void sampling();

    // 音频采样（录制）
    static void sampling(const char *file_name, int record_time);

    // 音频重采样
    static void resample(const char *file_name, int record_time);

protected:
    // 保护成员变量
    int myProtectedVar;
};


void Audio::sampling(const char *file_name, int record_time) {
    //0
    int ret;
    char errors[1024];
    // 给以个目标地址获取音视频流，注释: ":0" 获取本地外置麦克风获取音频流
    // 可以是一个网络地址，本地文件，对于设备给一个设备名字就行 获取音频流
    // 先传入视频设备编号，再传入音频设备编号
    // [[video device]:[audit device]]
    const char *devicename = ":0";


    // 上下文相关
    AVFormatContext *fmt_ctx = NULL;
    // avInputFormat 输入设备方式的参数设置
    AVDictionary *options = NULL;

    // 初始化av_packet 读取设备音频包
    AVPacket pkt;;

    //1
    // 注册设备
    avdevice_register_all();

    // 2
    // 使用该函数可以方便地确定FFmpeg是否支持特定的输入媒体格式，以便在处理输入流之前进行验证和选择正确的输入格式
    // 同的平台获取到的输入设备方式不一样：
    // macos 使用 avfoundation
    // windows 使用 dshow
    // linux 使用 alsa
    const AVInputFormat *avInputFormat = av_find_input_format(FMT_NAME);
    if (!avInputFormat) {
        printf("获取输入对象失败\n");
        return;
    }

    //3
    // 打开输入设备，准备开始录音
    ret = avformat_open_input(&fmt_ctx, devicename, avInputFormat, &options);
    if (ret < 0) {
        av_strerror(ret, errors, 1024);
        printf("Fail to open audio device ,[%d],%s\n", ret, errors);
        return;

    }

    //4
    // 创建文件
    FILE *out_file = fopen(file_name, "wb+");
    //5
    ret = av_new_packet(&pkt, 4096);
    if (ret < 0) {
        av_strerror(ret, errors, 1024);
        printf("init av_new_packet is error ,[%d],%s\n", ret, errors);
        return;

    }

    //6
    // 读取多少秒
    while (record_time) {
        ret = av_read_frame(fmt_ctx, &pkt);
        // 这里可能读取的时候，ret返回-35 表示设备还没准备好, 先睡眠1s
        if (ret == 1 || ret == -35) {
            usleep(10000);
            continue;
        }

        if (ret < 0) {
            break;
        }
        av_log(nullptr, AV_LOG_INFO, "pkt size is %d (%p) record_time %d \n", pkt.size, pkt.data, record_time);

        // 将数据以二进制形式写入文件
        fwrite(pkt.data, pkt.size, 1, out_file);
        // 从缓冲区写入文件
        fflush(out_file);
        // 释放pkg指针
        av_packet_unref(&pkt);
        --record_time;
    }

    //关闭文件流
    fclose(out_file);


    av_log(NULL, AV_LOG_DEBUG, "finish\n");

    AVStream *stream = fmt_ctx->streams[0];
    AVCodecParameters *params = stream->codecpar;
    av_log(nullptr, AV_LOG_DEBUG,
           "结束录制 采样率：%d  声道：%d 位深：%d 采样格式：%d 每一个样本的一个声道占用多少个字节:%d \n",
           params->sample_rate,
           params->channels,
           av_get_bits_per_sample(params->codec_id),
           params->format,
           av_get_bytes_per_sample((AVSampleFormat) params->format)
    );
    // 关闭设备，释放上下文指针
    avformat_close_input(&fmt_ctx);
}

void Audio::resample(const char *file_name, int record_time) {
    //1
    // 注册设备
    avdevice_register_all();
    // 2
    // 使用该函数可以方便地确定FFmpeg是否支持特定的输入媒体格式，以便在处理输入流之前进行验证和选择正确的输入格式
    // 同的平台获取到的输入设备方式不一样：
    // macos 使用 avfoundation
    // windows 使用 dshow
    // linux 使用 alsa
    const AVInputFormat *avInputFormat = av_find_input_format(FMT_NAME);
    //3
    int ret;
    char errors[1024];
    AVFormatContext *fmt_ctx = nullptr;
    // 给以个目标地址获取音视频流，注释: ":0" 获取本地外置麦克风获取音频流
    // 可以是一个网络地址，本地文件，对于设备给一个设备名字就行 获取音频流
    const char *url = ":0";
    // avInputFormat 输入设备方式的参数设置
    AVDictionary *options = nullptr;
    // 打开输入设备，准备开始录音
    ret = avformat_open_input(&fmt_ctx, url, avInputFormat, &options);
    if (ret < 0) {
        av_strerror(ret, errors, 1024);
        printf("Fail to open audio device ,[%d],%s\n", ret, errors);
        return;

    }
    // 创建文件
    FILE *outfile = fopen(file_name, "wb+");
    SwrContext *swr_ctx = NULL;
    // channel , number /
    swr_ctx = swr_alloc_set_opts(NULL, //ctx
                                 AV_CH_LAYOUT_STEREO, // 输出 channel 布局
                                 AV_SAMPLE_FMT_S16,  // 输出的采样格式 位深
                                 44100,              // 输出采样频率
                                 AV_CH_LAYOUT_STEREO,   // 输入 channel 布局
                                 AV_SAMPLE_FMT_FLT,    // 输入的采样格式 位深
                                 48000,                // 输入采样频率
                                 0,
                                 NULL);
    if (!swr_ctx) {
        av_strerror(ret, errors, 1024);
        printf("swr_ctx is error ,[%d],%s\n", ret, errors);
        return;
    }
    // 初始化重采样上下文
    ret = swr_init(swr_ctx);
    if (ret < 0) {
        av_strerror(ret, errors, 1024);
        printf("swr_init is error ,[%d],%s\n", ret, errors);
        return;
    }

    //4
    // 初始化av_packet 读取设备音频包
    AVPacket pkt;
    ret = av_new_packet(&pkt, 2048);
    if (ret < 0) {
        av_strerror(ret, errors, 1024);
        printf("init av_new_packet is error ,[%d],%s\n", ret, errors);
        return;

    }
    uint8_t **src_data = NULL;
    int src_linesize = 0;

    uint8_t **dst_data = NULL;
    int dst_linesize = 0;
    // 创建输入缓冲区
    av_samples_alloc_array_and_samples(&src_data,    // 输出缓冲器地址
                                       &src_linesize,   // 缓冲区的大小
                                       2,            // 通道个数
                                       512,           // 单通道采样个数
                                       AV_SAMPLE_FMT_FLT, // 采样格式
                                       0);
    // 创建输出缓冲区
    av_samples_alloc_array_and_samples(&dst_data,    // 输出缓冲器地址
                                       &dst_linesize,   // 缓冲区的大小
                                       2,            // 通道个数
                                       512,           // 单通道采样个数
                                       AV_SAMPLE_FMT_S16, // 采样格式
                                       0);
    // 读取多少秒
    while (record_time) {
        ret = av_read_frame(fmt_ctx, &pkt);
        // 这里可能读取的时候，ret返回-35 表示设备还没准备好, 先睡眠1s
        if (ret == 1 || ret == -35) {
            usleep(10000);
            continue;
        }

        if (ret < 0) {
            break;
        }
        av_log(nullptr, AV_LOG_INFO, "pkt size is %d (%p) record_time %d \n", pkt.size, pkt.data, record_time);

        //进行内存拷贝（按字节拷贝）
        memcpy((void *) src_data[0], (void *) pkt.data, pkt.size);
        // 重采样
        swr_convert(swr_ctx,  // 重采样上下文
                    dst_data,// 输出结果缓冲区
                    512, // 输出单个通道的采样个数
                    (const uint8_t **) src_data, // 输入缓冲区
                    512 // 输入单个通道的采样树
        );
        // 将读取到的声音二进制写进去文件
        fwrite(dst_data[0], 1, dst_linesize, outfile);
        fflush(outfile);
        av_packet_unref(&pkt);
        --record_time;
    }

    //关闭文件流
    fclose(outfile);

    // 释放输入输出缓冲区
    if (src_data) {
        av_freep(&src_data[0]);
    }
    av_freep(src_data);

    if (dst_data) {
        av_freep(&dst_data[0]);
    }
    av_freep(dst_data);
    // 释放重采样的上下文
    swr_free(&swr_ctx);

    AVStream *stream = fmt_ctx->streams[0];
    AVCodecParameters *params = stream->codecpar;
    av_log(nullptr, AV_LOG_DEBUG, "结束录制 采样率：%d  声道：%d 位深：%d \n", params->sample_rate, params->channels,
           av_get_bits_per_sample(params->codec_id));
    // 录制结束 释放上下文 release ctx
    avformat_close_input(&fmt_ctx);
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








