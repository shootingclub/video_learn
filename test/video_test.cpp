#include "gtest/gtest.h"
#include <iostream>
#include "video.h"

TEST(VideoTestSuite, FMT_DEFUALT) {
    // ffplay -pix_fmt uyvy422 -s 640x480 default.yuv
    video::video video;
    video.fmtDefault("/Users/yaohua/c_workspace/video_learn/video/default.yuv", 300);
}

TEST(VideoTestSuite, FMT_NV12) {
    // ffplay -pix_fmt nv12 -s 640x480 nv12.yuv
    video::video video;
    video.fmtNV12("/Users/yaohua/c_workspace/video_learn/video/nv12.yuv", 300);
}

TEST(VideoTestSuite, FMT_YUV420) {
    video::video video;
    video.fmtYUV420("/Users/yaohua/c_workspace/video_learn/video/yuv420.yuv", 300);
}

TEST(VideoTestSuite, FMT_H264) {
    video::video video;
    video.fmtH264("/Users/yaohua/c_workspace/video_learn/video/video.h264", 300);
}