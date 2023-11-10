#include "player.h"

namespace player {
    // 设置日志级别 AV_LOG_DEBUG
    void set_log(int level) {
        av_log_set_level(level);
        av_log(nullptr, AV_LOG_INFO, "hello world \n");
    }
}