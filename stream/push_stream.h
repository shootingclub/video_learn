

#define __STDC_CONSTANT_MACROS
#ifndef VIDEO_LEARN_PUSHSTREAM_H
#define VIDEO_LEARN_PUSHSTREAM_H


#include <cstdio>
#include <stdlib.h>
#include <unistd.h>

extern "C" {
#include "librtmp/rtmp.h"
}


namespace stream {
    class stream {

    private:
        RTMPPacket *alloc_packet();

        int read_data(FILE *fp, RTMPPacket **packet);

        FILE *open_flv(char *flv_name);

        RTMP *conect_rtmp_server(char *rtmpaddr);

        void send_data(FILE *fp, RTMP *rtmp);

    public:
        void publish_stream(char *flv, char *rtmpaddr);
    };
}


#endif //VIDEO_LEARN_PUSHSTREAM_H

int main(int argc, const char *argv[]) {
    char *flv = "/Users/lichao/Documents/new_killer.flv";
    char *rtmpaddr = "rtmp://localhost/live/room";
    stream::stream pushStream;
    pushStream.publish_stream(flv, rtmpaddr);
}