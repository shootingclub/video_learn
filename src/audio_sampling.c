#include "audio_sampling.h"

// 音频重采样
void audio_sampling() {
    // 注册设备
    avdevice_register_all();
    // 使用该函数可以方便地确定FFmpeg是否支持特定的输入媒体格式，以便在处理输入流之前进行验证和选择正确的输入格式
    // 不同的平台使用不同的输入格式：macos 使用 avfoundation
    const AVInputFormat *avInputFormat = av_find_input_format("avfoundation");
    int ret;
    char errors[1024];
    AVFormatContext *fmt_ctx = NULL;
    // 给以个目标地址获取音视频流，注释: ":0" 获取本地外置麦克风获取音频流
    const char *url = ":0";
    AVDictionary *options = NULL;
    ret = avformat_open_input(&fmt_ctx, url, avInputFormat, &options);
    if (ret < 0) {
        av_strerror(ret, errors, 1024);
        printf("Fail to open audio device ,[%d],%s\n", ret, errors);
        return;

    }
}


#if 0
int main(int argc, char *argv[])
{
    rec_audio();
    return 0;
}
#endif

