#include  "src/audio.h"
#include <iostream>

int main() {

//    printf("Hello, World! \n");
//    std::cout << "Hello, World!" << std::endl;
    av_log_set_level(AV_LOG_DEBUG);//设置日志级别
    av_log(nullptr, AV_LOG_INFO, "hello ffmpeg\n");//打印日志
    auto info = avcodec_configuration();//avcodec配置信息
    av_log(nullptr, AV_LOG_DEBUG, "配置:%s", info);

    Audio::sampling("/Users/yaohua/c_workspace/vido_learn/audio.pcm", 500);
    return 0;
}


