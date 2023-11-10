#include "gtest/gtest.h"
#include <iostream>
#include "player.h"

TEST(PlayerTestSuite, PLAYER_LOG) {
    printf("hello world\n");
    av_log_set_level(AV_LOG_DEBUG);
    av_log(nullptr, AV_LOG_INFO, "hello world \n");
}
