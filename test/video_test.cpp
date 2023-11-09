//
// Created by 张耀华 on 2023/11/7.
//



#include "gtest/gtest.h"
#include <iostream>
#include "video.h"

TEST(VideoTestSuite, FMTDEFUALT) {
    video::video video;
    video.fmtDefault("/Users/yaohua/c_workspace/video_learn/video/uyvy.yuv", 300);
}

TEST(VideoTestSuite, FMTNV12) {
    video::video video;
    video.fmtNv12("/Users/yaohua/c_workspace/video_learn/video/nv12.yuv", 300);
}

TEST(VideoTestSuite, FMTYUV420_H264) {
    video::video video;
    video.fmtH264("/Users/yaohua/c_workspace/video_learn/video/yuvH264.yuv", 300);
}