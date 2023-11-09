//
// Created by 张耀华 on 2023/11/7.
//

#include "video.h"

namespace video {

    AVDictionary **set_video_options(AVDictionary **options,
                                     char *video_size,
                                     char *framerate,
                                     char *pixel_format
    ) {
        if (!video_size) {
            av_dict_set(options, "video_size", video_size, 0);
        } else if (!framerate) {
            av_dict_set(options, "framerate", framerate, 0);
        } else if (!pixel_format) {
            av_dict_set(options, "pixel_format", pixel_format, 0);
        }
        return options;
    }

    void open_video(AVFormatContext **fmt_ctx, const char *url, AVDictionary **options) {
        //0
        int ret;
        char errors[BUFFER_SIZE];
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
        ret = avformat_open_input(fmt_ctx, url, avInputFormat, options);
        if (ret < 0) {
            av_strerror(ret, errors, BUFFER_SIZE);
            printf("Fail to open audio device ,[%d],%s\n", ret, errors);
            return;
        }
    }


    void video::fmtDefault(const char *file_name, int record_time) {
        int ret;
        char errors[BUFFER_SIZE];
        // 给以个目标地址获取音视频流，注释: ":0" 获取本地外置麦克风获取音频流
        // 可以是一个网络地址，本地文件，对于设备给一个设备名字就行 获取音频流
        // 先传入视频设备编号，再传入音频设备编号
        // [[video device]:[audit device]]
        const char *devicename = "0";

        // 上下文相关
        AVFormatContext *fmt_ctx = NULL;
        // avInputFormat 输入设备方式的参数设置
        AVDictionary *options = NULL;
        // 初始化av_packet 读取设备音频包
        AVPacket pkt;

        //1 设置视频参数 打开输入设备，准备开始录音
        av_dict_set(&options, "video_size", "640x480", 0);
        av_dict_set(&options, "framerate", "30", 0);
        open_video(&fmt_ctx, devicename, &options);

        //2 创建文件
        FILE *out_file = fopen(file_name, "wb+");

        //3 初始化数据包
        ret = av_new_packet(&pkt, FMT_PACKET_SIZE);
        if (ret < 0) {
            av_strerror(ret, errors, BUFFER_SIZE);
            printf("init av_new_packet is error ,[%d],%s\n", ret, errors);
            return;

        }

        // 读取多少秒
        while (record_time) {
            //4 将数据读取到数据包内
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

            // 将数据以二进制形式写入文件, 1:代表每次写几个包
//            fwrite(pkt.data, pkt.size, 1, out_file);
            fwrite(pkt.data, VIDEO_FMT_DEFAULT_SIZE, 1, out_file);
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

    void video::fmtNv12(const char *file_name, int record_time) {

        int ret;
        char errors[BUFFER_SIZE];
        // 给以个目标地址获取音视频流，注释: ":0" 获取本地外置麦克风获取音频流
        // 可以是一个网络地址，本地文件，对于设备给一个设备名字就行 获取音频流
        // 先传入视频设备编号，再传入音频设备编号
        // [[video device]:[audit device]]
        const char *devicename = "0";

        // 上下文相关
        AVFormatContext *fmt_ctx = NULL;
        // avInputFormat 输入设备方式的参数设置
        AVDictionary *options = NULL;
        // 初始化av_packet 读取设备音频包
        AVPacket pkt;

        //1 设置视频参数 打开输入设备，准备开始录音
        av_dict_set(&options, "video_size", "640x480", 0);
        av_dict_set(&options, "framerate", "30", 0);
        av_dict_set(&options, "pixel_format", "nv12", 0);
        open_video(&fmt_ctx, devicename, &options);

        //2 创建文件
        FILE *out_file = fopen(file_name, "wb+");

        //3 初始化数据包
        ret = av_new_packet(&pkt, FMT_PACKET_SIZE);
        if (ret < 0) {
            av_strerror(ret, errors, BUFFER_SIZE);
            printf("init av_new_packet is error ,[%d],%s\n", ret, errors);
            return;

        }

        // 读取多少秒
        while (record_time) {
            //4 将数据读取到数据包内
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

            // 将数据以二进制形式写入文件, 1:代表每次写几个包
//            fwrite(pkt.data, pkt.size, 1, out_file);
            fwrite(pkt.data, VIDEO_FMT_NV12_SIZE, 1, out_file);
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

}
