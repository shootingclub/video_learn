#include "audio_sampling.h"

static int rec_status = 0;

// 音频重采样
void audio_sampling() {
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
    //4
    // 初始化av_packet 读取设备音频包
    AVPacket pkt;
    ret = av_new_packet(&pkt, 4096);
    if (ret < 0) {
        av_strerror(ret, errors, 1024);
        printf("init av_new_packet is error ,[%d],%s\n", ret, errors);
        return;

    }
    // 读取音频设备 500
    int count = 0;
    while ((ret = av_read_frame(fmt_ctx, &pkt) == 0) && count++ < 500) {
        av_log(nullptr, AV_LOG_INFO, "pkt size is %d (%p) count=%d\n", pkt.size, pkt.data, count);
//        printf("pkt size is %d (%p) count=%d\n", pkt.size, pkt.data, count);
        // 录制500 秒之后 释放pkt
//        av_packet_unref(&pkt);
    }
    if (ret != 0) {
        av_strerror(ret, errors, 1024);
        printf("read av_read_frame  ,[%d],%s\n", ret, errors);
        return;

    }

    // 录制结束 释放上下文 release ctx
    avformat_close_input(&fmt_ctx);
    av_log(nullptr, AV_LOG_DEBUG, "finish \n");

}


#if 0
int main(int argc, char *argv[])
{
    rec_audio();
    return 0;
}
#endif

