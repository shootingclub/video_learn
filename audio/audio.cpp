//
// Created by 张耀华 on 2023/11/7.
//

#include "audio.h"

namespace audio {

    bool has_pcm_suffix(const char *filename) {
        const int len = strlen(filename);
        return (len >= 4) && (strcmp(filename + len - 4, ".pcm") == 0);
    }

    SwrContext *init_swr(int int_sample_rate, int out_sample_rate) {
        SwrContext *swr_ctx = NULL;
        //channel, number/
        swr_ctx = swr_alloc_set_opts(NULL,                //ctx
                                     AV_CH_LAYOUT_STEREO, //输出channel布局
                                     AV_SAMPLE_FMT_S16,   //输出的采样格式
                                     out_sample_rate,     //输出采样率
                                     AV_CH_LAYOUT_STEREO, //输入channel布局
                                     AV_SAMPLE_FMT_FLT,   //输入的采样格式
                                     int_sample_rate,     //输入的采样率
                                     0, NULL);

        if (!swr_ctx) {
            printf("set SwrContext option failed \n");
            return nullptr;
        }

        if (swr_init(swr_ctx) < 0) {
            printf("init SwrContext failed\n");
            return nullptr;
        }

        return swr_ctx;
    }

    // 初始化采样上下文
    SwrConvertData init_swr_convert_data() {
        SwrConvertData swr;
        //2048/4=512/2=256
        //创建输入缓冲区
        av_samples_alloc_array_and_samples(&swr.src_data,         //输出缓冲区地址
                                           &swr.src_linesize,     //缓冲区的大小
                                           1,                 //通道个数
                                           512,               //单通道采样个数
                                           AV_SAMPLE_FMT_FLT, //采样格式
                                           0);

        //创建输出缓冲区
        av_samples_alloc_array_and_samples(&swr.dst_data,         //输出缓冲区地址
                                           &swr.dst_linesize,     //缓冲区的大小
                                           1,                 //通道个数
                                           512,               //单通道采样个数
                                           AV_SAMPLE_FMT_S16, //采样格式
                                           0);


        return swr;
    }

    void open_microphone(AVFormatContext **fmt_ctx, const char *url, AVDictionary **options) {
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

    // 采样
    void audio::sampling::sampling_pcm(const char *file_name, int record_time) {
        if (!has_pcm_suffix(file_name)) {
            printf("The file does not have .pcm suffix.\n");
            return;
        }
        int ret;
        char errors[BUFFER_SIZE];
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
        AVPacket pkt;

        //1 打开输入设备，准备开始录音
        open_microphone(&fmt_ctx, devicename, &options);

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

    // 采样pcm
    void audio::sampling::resampling_pcm(const char *file_name, int record_time,
                                         int int_sample_rate,
                                         int out_sample_rate) {
        if (!has_pcm_suffix(file_name)) {
            printf("The file does not have .pcm suffix.\n");
            return;
        }
        int ret;
        char errors[BUFFER_SIZE];
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
        AVPacket pkt;

        //1 打开输入设备，准备开始录音
        open_microphone(&fmt_ctx, devicename, &options);

        //2 创建文件
        FILE *out_file = fopen(file_name, "wb+");

        //3 初始化数据包
        ret = av_new_packet(&pkt, FMT_PACKET_SIZE);
        if (ret < 0) {
            av_strerror(ret, errors, BUFFER_SIZE);
            printf("init av_new_packet is error ,[%d],%s\n", ret, errors);
            return;

        }
        //4 初始化重采样上下文
        SwrContext *swrCtx = init_swr(int_sample_rate, out_sample_rate);
        //5 初始化重采样上下文转化数组
        SwrConvertData swrConvertData = init_swr_convert_data();

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

            //进行内存拷贝（按字节拷贝）
            memcpy((void *) swrConvertData.src_data[0], (void *) pkt.data, pkt.size);
            //重采样
            swr_convert(swrCtx,                     //重采样的上下文
                        swrConvertData.dst_data,                    //输出结果缓冲区
                        512,                                    //每个通道的采样数
                        (const uint8_t **) swrConvertData.src_data, //输入缓冲区
                        512);                                   //输入单个通道的采样数

            // 将读取到的声音二进制写进去文件
            fwrite(swrConvertData.dst_data[0], 1, swrConvertData.dst_linesize, out_file);
            fflush(out_file);

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


    // 采样aac
    void audio::sampling::resampling_aac(const char *file_name,
                                         int record_time,
                                         int int_sample_rate,
                                         int out_sample_rate) {
//        if (!has_pcm_suffix(file_name)) {
//            printf("The file does not have .pcm suffix.\n");
//            return;
//        }
        int ret;
        char errors[BUFFER_SIZE];
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
        AVPacket pkt;
        AVPacket *newpkt;

        //1 打开输入设备，准备开始录音
        open_microphone(&fmt_ctx, devicename, &options);

        //2 创建文件
        FILE *out_file = fopen(file_name, "wb+");


        ret = av_new_packet(&pkt, FMT_PACKET_SIZE);
        if (ret < 0) {
            av_strerror(ret, errors, BUFFER_SIZE);
            printf("init av_new_packet is error ,[%d],%s\n", ret, errors);
            return;

        }
        //3.1 打开编码器
        const AVCodec *avCodec = avcodec_find_encoder_by_name("libfdk_aac");
        // 创建上下文
        //创建 codec 上下文
        AVCodecContext *codec_ctx = avcodec_alloc_context3(avCodec);
        codec_ctx->sample_fmt = AV_SAMPLE_FMT_S16;          //输入音频的采样大小
        codec_ctx->channel_layout = AV_CH_LAYOUT_STEREO;    //输入音频的channel layout
        codec_ctx->channels = 2;                            //输入音频 channel 个数
        codec_ctx->sample_rate = 44100;                     //输入音频的采样率
        codec_ctx->bit_rate = 0; //AAC_LC: 128K, AAC HE: 64K, AAC HE V2: 32K
        codec_ctx->profile = FF_PROFILE_AAC_HE_V2; //阅读 ffmpeg 代码
        //打开编码器
        if (avcodec_open2(codec_ctx, avCodec, nullptr) < 0) {
            printf("open acc avcodec_open2 failed \n");
            return;
        }
        AVFrame *avFrame = av_frame_alloc();
        avFrame->nb_samples = 512; // 单通道一个音频帧采样数
        avFrame->format = AV_SAMPLE_FMT_S16; //每个采样大小
        avFrame->channel_layout = AV_CH_LAYOUT_STEREO; //channle layout
        av_frame_get_buffer(avFrame, 0);
        //音频输入数据
        if (!avFrame->buf[0]) {
            printf("av_frame_alloc failed \n");
            return;
        }

        //3 堆中分配编码后的数据
        newpkt = av_packet_alloc();
        if (!newpkt) {
            printf("av_packet_alloc failed \n");
            return;
        }

        //4 初始化重采样上下文
        SwrContext *swrCtx = init_swr(int_sample_rate, out_sample_rate);
        //5 初始化重采样上下文转化数组
        SwrConvertData swrConvertData = init_swr_convert_data();



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

            //进行内存拷贝（按字节拷贝）
            memcpy((void *) swrConvertData.src_data[0], (void *) pkt.data, pkt.size);
            //重采样
            swr_convert(swrCtx,                     //重采样的上下文
                        swrConvertData.dst_data,                    //输出结果缓冲区
                        512,                                    //每个通道的采样数
                        (const uint8_t **) swrConvertData.src_data, //输入缓冲区
                        512);                                   //输入单个通道的采样数

            memcpy((void *) avFrame->data[0], (void *) swrConvertData.dst_data[0], swrConvertData.dst_linesize);
            ret = avcodec_send_frame(codec_ctx, avFrame);
            while (ret >= 0) {
                //获取编码后的音频数据
                ret = avcodec_receive_packet(codec_ctx, newpkt);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    printf(" encoding audio avcodec_receive_packet %d\n", ret);
                    return;
                } else if (ret < 0) {
                    printf("Error, encoding audio frame\n");
                    exit(-1);
                }
                // 将读取到的声音二进制写进去文件
                fwrite(newpkt->data, 1, newpkt->size, out_file);
                fflush(out_file);
            }

            av_packet_unref(&pkt);
            --record_time;
        }

        //释放 AVFrame 和 AVPacket
        av_frame_free(&avFrame);
        av_packet_free(&newpkt);

        //释放重采样缓冲区
        //释放输入输出缓冲区
        if (swrConvertData.src_data) {
            av_freep(&swrConvertData.src_data[0]);
        }
        av_freep(swrConvertData.src_data);

        if (swrConvertData.dst_data) {
            av_freep(&swrConvertData.src_data[0]);
        }
        av_freep(swrConvertData.src_data);

        //释放重采样的上下文
        if (swrCtx) {
            swr_free(&swrCtx);
        }

        avcodec_free_context(&codec_ctx);

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


