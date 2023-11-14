#include "gtest/gtest.h"
#include <iostream>
#include "player.h"

TEST(PlayerTestSuite, MEDIA_INFO) {
    player::player player;
    player.media_info("/Users/yaohua/c_workspace/video_learn/player/video.mp4");
//    std::cout << "---------" << std::endl;
//    player.media_info("/Users/yaohua/c_workspace/video_learn/player/out_video.aac");
}

TEST(PlayerTestSuite, EXTRACT_AUDIO_BASE_INFO) {
    //dst: aac mp4 mov
    player::player player;
    player.extract_audio_base("/Users/yaohua/c_workspace/video_learn/player/video.mp4",
                              "/Users/yaohua/c_workspace/video_learn/player/out_video.mp4");
}

TEST(PlayerTestSuite, EXTRACT_VIDEO_BASE_INFO) {
    //dst: h264 mov flv
    player::player player;
    player.extract_video_base("/Users/yaohua/c_workspace/video_learn/player/video.mp4",
                              "/Users/yaohua/c_workspace/video_learn/player/out_video.flv");
}

TEST(PlayerTestSuite, REMUX_BASE_INFO) {
    //dst: h264 mov flv
    player::player player;
    player.remux_base("/Users/yaohua/c_workspace/video_learn/player/video.mp4",
                      "/Users/yaohua/c_workspace/video_learn/player/video.mov");
}


TEST(PlayerTestSuite, CUT_BASE_INFO) {
    //dst: h264 mov flv
    player::player player;
    player.cut_base("/Users/yaohua/c_workspace/video_learn/player/video.mp4",
                    "/Users/yaohua/c_workspace/video_learn/player/video.mov", 10, 60);
}

TEST(PlayerTestSuite, ENCODE_VIDEO_INFO) {
    //dst: h264 mov flv
    player::player player;
    player.encode_video("libx264", "/Users/yaohua/c_workspace/video_learn/player/video_encode_h264.h264");
}

TEST(PlayerTestSuite, ENCODE_AUDIO_INFO) {
    //dst: h264 mov flv
    player::player player;
    player.encode_audio("libfdk_aac", "/Users/yaohua/c_workspace/video_learn/player/audio_encode_aac.aac");
}


