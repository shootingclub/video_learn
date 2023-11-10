project(player)
set(HEADER_FILES
        player.h
)

set(SOURCE_FILES
        player.cpp


)


add_library(player STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(player PUBLIC .)
# 添加ffmpeg库
include_directories(/usr/local/ffmpeg/include)
link_directories(/usr/local/ffmpeg/lib)

target_link_libraries(player
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        swresample
        swscale
)