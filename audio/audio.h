

#ifndef VIDEO_LEARN_AUDIO_H
#define VIDEO_LEARN_AUDIO_H
#define __STDC_CONSTANT_MACROS

extern "C" { //C++中特殊处理
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavutil/log.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h" // 从采样
}
#define FMT_NAME "avfoundation"
#define FMT_PACKET_SIZE 2048
#define BUFFER_SIZE 1024


#include <iostream>
#include <unistd.h>
#include <cstring>

namespace audio {

    struct SwrConvertData {
        uint8_t **src_data;
        int src_linesize;
        uint8_t **dst_data;
        int dst_linesize;
    };

    class sampling {
    public:
        // 采样 pcm
        void sampling_pcm(const char *file_name, int record_time);

        // 重采样 pcm
        void resampling_pcm(const char *file_name, int record_time, int int_sample_rate, int out_sample_rate);

        // 重采样 aac
        void resampling_aac(const char *file_name, int record_time, int int_sample_rate, int out_sample_rate);
    };
}


#endif //VIDEO_LEARN_AUDIO_H
