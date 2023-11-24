
#include "gtest/gtest.h"
#include <iostream>
#include "audio.h"

TEST(AudioTestSuite, AudioSamplingPcm) {

    audio::sampling sampling;
    sampling.sampling_pcm("/Users/yaohua/c_workspace/video_learn/audio/test.pcm", 300);

}

TEST(AudioTestSuite, AudioReSamplingPcm) {
    audio::sampling sampling;
    sampling.resampling_pcm("/Users/yaohua/c_workspace/video_learn/audio/test.pcm", 300, 48000, 44100);
}

TEST(AudioTestSuite, AudioReSamplingAAC) {
    audio::sampling sampling;
    sampling.resampling_aac("/Users/yaohua/c_workspace/video_learn/audio/test.aac", 300, 48000, 44100);
}
