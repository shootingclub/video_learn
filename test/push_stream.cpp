#include "gtest/gtest.h"
#include <iostream>
#include "pushStream.h"


TEST(PushStreamTestSuite, PushStream) {
    char *flv = "/Users/lichao/Documents/new_killer.flv";
    char *rtmpaddr = "rtmp://localhost/live/room";
    stream::pushStream pushStream;
    pushStream.publish_stream(flv,rtmpaddr);
}

