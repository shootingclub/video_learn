

#ifndef VIDEO_LEARN_PLAYER_H
#define VIDEO_LEARN_PLAYER_H

#define __STDC_CONSTANT_MACROS

extern "C" { //C++中特殊处理
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavutil/log.h"
#include "libavutil/mathematics.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h" // 从采样
}
#define FMT_NAME "avfoundation"
#define FMT_PACKET_SIZE 2048
#define BUFFER_SIZE 1024

#include <iostream>
#include <unistd.h>
#include <cstring>

namespace player {
    class player {

    private:
        void extract_audio(char *filename);

        void extract_video();

        int select_channel_layout(const AVCodec *codec);

        int select_best_sample_rate(const AVCodec *codec);

        int check_sample_fmt(const AVCodec *codec, AVSampleFormat sample_fmt);

        int encode(AVCodecContext *avCodecContext, AVFrame *avFrame, AVPacket *avPacket, FILE *out);

    public:
        void media_info(char *filename);

        void extract_audio_base(char *srcFile, char *dstFile);

        void extract_video_base(char *srcFile, char *dstFile);

        void remux_base(char *srcFile, char *dstFile);

        void cut_base(char *srcFile, char *dstFile, double startTime, double endTime);

        void av_merge_base(char *srcFile1, char *srcFile2, char *dstFile);

        void encode_video(char *codecName, char *dstFile);

        void encode_audio(char *codecName, char *dstFile);

        void gen_pic(char *srcFile, char *dstFile);

    };
}


#endif //VIDEO_LEARN_PLAYER_H
