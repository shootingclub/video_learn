

#include "video.h"

namespace video {


    AVDictionary *set_video_options(AVDictionary *options,
                                    char *video_size,
                                    char *framerate,
                                    char *pixel_format
    ) {
        av_dict_set(&options, "video_size", video_size, 0);
        av_dict_set(&options, "framerate", framerate, 0);
        av_dict_set(&options, "pixel_format", pixel_format, 0);

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

    void open_encoder_h264(int width, int height, AVCodecContext **enc_ctx) {
        int ret;
        const AVCodec *codec = NULL;
        codec = avcodec_find_encoder_by_name("libx264");
        if (!codec) {
            printf("codec libx264 not found \n");
            exit(-1);
        }
        *enc_ctx = avcodec_alloc_context3(codec);
        if (!*enc_ctx) {
            printf("avcodec_alloc_context3 failed \n");
            exit(-1);
        }
        //SPS/PPS
        (*enc_ctx)->profile = FF_PROFILE_H264_HIGH_444;
        (*enc_ctx)->level = 50; // 5.0 清晰度非常高
        (*enc_ctx)->width = FMT_V_WIDTH;
        (*enc_ctx)->height = FMT_V_HEIGHT;
        //设置gop
        (*enc_ctx)->gop_size = 250; //gop之间的间隔，如果设置很小的话那么i帧就很多，码流就很大；同理相反
        (*enc_ctx)->keyint_min = 25;//每隔25帧，自动插入一个i帧
        //设置b帧
        (*enc_ctx)->max_b_frames = 3; // option
        (*enc_ctx)->has_b_frames = 1; // option
        //设置参考帧
        (*enc_ctx)->refs = 3; // option
        //设置输入yuv格式
        (*enc_ctx)->pix_fmt = AV_PIX_FMT_YUV420P;
        //设置码率 600kbps
        (*enc_ctx)->bit_rate = 600000;
        //设置帧率
        (*enc_ctx)->time_base = (AVRational) {1, 25}; //帧与帧之间的时间间隔
        (*enc_ctx)->framerate = (AVRational) {25, 1}; //帧率 每25秒帧
        ret = avcodec_open2((*enc_ctx), codec, nullptr);
        if (ret < 0) {
            printf("could not open codec: %s!\n", av_err2str(ret));
            exit(1);
        }
    }

    void encode(AVCodecContext *enc_ctx,
                AVFrame *frame,
                AVPacket *newpkt,
                FILE *outfile) {

        int ret;
        //送原始数据给编码器进行编码
        ret = avcodec_send_frame(enc_ctx, frame);
        if (ret < 0) {
            printf("Error, Failed to send a frame for enconding!\n");
            exit(1);
        }

        //从编码器获取编码好的数据
        while (ret >= 0) {
            ret = avcodec_receive_packet(enc_ctx, newpkt);
            //如果编码器数据不足时会返回  EAGAIN,或者到数据尾时会返回 AVERROR_EOF
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                printf(" encoding video avcodec_receive_packet %d\n", ret);
                return;
            } else if (ret < 0) {
                printf("Error, Failed to encode!\n");
                exit(1);
            }

            fwrite(newpkt->data, newpkt->size, 1, outfile);
            fflush(outfile);
            av_packet_unref(newpkt);
        }
    }

    AVFrame *create_frame(int width, int height) {
        int ret;
        AVFrame *avFrame = NULL;
        avFrame = av_frame_alloc();
        if (!avFrame) {
            printf("create_av_frame failed \n");
            av_frame_free(&avFrame);
            return nullptr;
        }
        avFrame->width = width;
        avFrame->height = height;
        avFrame->format = AV_PIX_FMT_YUV420P;

        // alloc inner memory
        ret = av_frame_get_buffer(avFrame, 32); // 按32位对齐 4字节
        if (ret < 0) {
            printf("av_frame_get_buffer failed \n");
            av_frame_free(&avFrame);
            return nullptr;
        }
        return avFrame;
    }

    void video::fmtDefault(const char *file_name, int record_time) {
        int ret;
        char errors[BUFFER_SIZE];
        // 给以个目标地址获取音视频流，注释: ":0" 获取本地外置麦克风获取音频流
        // 可以是一个网络地址，本地文件，对于设备给一个设备名字就行 获取音频流
        // 先传入视频设备编号，再传入音频设备编号
        // [[video device]:[audit device]]
        const char *devicename = FMT_CAMERA;

        // 上下文相关
        AVFormatContext *fmt_ctx = NULL;
        AVCodecContext *enc_ctx = NULL;
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

    void video::fmtNV12(const char *file_name, int record_time) {

        int ret;
        char errors[BUFFER_SIZE];
        // 给以个目标地址获取音视频流，注释: ":0" 获取本地外置麦克风获取音频流
        // 可以是一个网络地址，本地文件，对于设备给一个设备名字就行 获取音频流
        // 先传入视频设备编号，再传入音频设备编号
        // [[video device]:[audit device]]
        const char *devicename = FMT_CAMERA;

        // 上下文相关
        AVFormatContext *fmt_ctx = NULL;
        // avInputFormat 输入设备方式的参数设置
        AVDictionary *options = nullptr;
        // 初始化av_packet 读取设备音频包
        AVPacket pkt;

        //1 设置视频参数 打开输入设备，准备开始录音
        options = set_video_options(options, "640x480", "30", "nv12");
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

    void video::fmtYUV420(const char *file_name, int record_time) {

        int ret;
        char errors[BUFFER_SIZE];
        // 给以个目标地址获取音视频流，注释: ":0" 获取本地外置麦克风获取音频流
        // 可以是一个网络地址，本地文件，对于设备给一个设备名字就行 获取音频流
        // 先传入视频设备编号，再传入音频设备编号
        // [[video device]:[audit device]]
        const char *devicename = FMT_CAMERA;

        // 上下文相关
        AVFormatContext *fmt_ctx = NULL;
        // avInputFormat 输入设备方式的参数设置
        AVDictionary *options = NULL;
        // 初始化av_packet 读取设备音频包
        AVPacket pkt;

        //1 设置视频参数 打开输入设备，准备开始录音
        options = set_video_options(options, "640x480", "30", "nv12");
        open_video(&fmt_ctx, devicename, &options);
        // 创建AVFrame
        AVFrame *frame = create_frame(FMT_V_WIDTH, FMT_V_HEIGHT);

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

            // YYYYYYYYUVUV NV12
            // YYYYYYYYUUVV YUV420
            // copy y 数据 data[0]
            int size = FMT_V_HEIGHT * FMT_V_WIDTH;
            // size 之后的data是 UV UV UV ....
            memcpy(frame->data[0], pkt.data, size);
            for (int i = 0; i < size / 4; i++) {
                frame->data[1][i] = pkt.data[size + i * 2];
                frame->data[2][i] = pkt.data[size + i * 2 + 1];
            }

            fwrite(frame->data[0], size, 1, out_file);
            fwrite(frame->data[1], size / 4, 1, out_file);
            fwrite(frame->data[2], size / 4, 1, out_file);
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

    void video::fmtH264(const char *file_name, int record_time) {

        int ret;
        int base = 0;
        char errors[BUFFER_SIZE];
        // 给以个目标地址获取音视频流，注释: ":0" 获取本地外置麦克风获取音频流
        // 可以是一个网络地址，本地文件，对于设备给一个设备名字就行 获取音频流
        // 先传入视频设备编号，再传入音频设备编号
        // [[video device]:[audit device]]
        const char *devicename = FMT_CAMERA;

        // 上下文相关
        AVFormatContext *fmt_ctx = nullptr;
        AVCodecContext *enc_ctx = nullptr;
        // avInputFormat 输入设备方式的参数设置
        AVDictionary *options = nullptr;
        // 初始化av_packet 读取设备音频包
        AVPacket pkt;

        //1 设置视频参数 打开输入设备，准备开始录音
        options = set_video_options(options, "640x480", "30", "nv12");
        open_video(&fmt_ctx, devicename, &options);
        // 打开编码器
        open_encoder_h264(FMT_V_WIDTH, FMT_V_HEIGHT, &enc_ctx);
        // 创建AVFrame
        AVFrame *frame = create_frame(FMT_V_WIDTH, FMT_V_HEIGHT);
        // 创建AVPacket
        AVPacket *newPacket = av_packet_alloc();
        if (!newPacket) {
            printf("av_packet_alloc failed \n");
            av_packet_free(&newPacket);
        }

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

            // YYYYYYYYUVUV NV12
            // YYYYYYYYUUVV YUV420
            // copy y 数据 data[0]
            int size = FMT_V_HEIGHT * FMT_V_WIDTH;
            // size 之后的data是 UV UV UV ....
            memcpy(frame->data[0], pkt.data, size);
            for (int i = 0; i < size / 4; i++) {
                frame->data[1][i] = pkt.data[size + i * 2];
                frame->data[2][i] = pkt.data[size + i * 2 + 1];
            }

//            printf("send frame to encoder pts=%lld", frame->pts);
            frame->pts = base++;
            encode(enc_ctx, frame, newPacket, out_file);
            // 释放pkg指针
            av_packet_unref(&pkt);
            --record_time;
        }
        encode(enc_ctx, nullptr, newPacket, out_file);
        //关闭文件流
        fclose(out_file);
        // 关闭设备，释放上下文指针
        if (fmt_ctx) {
            avformat_close_input(&fmt_ctx);
        }
        av_log(nullptr, AV_LOG_DEBUG, "rec h264 finish\n");

    }
}
