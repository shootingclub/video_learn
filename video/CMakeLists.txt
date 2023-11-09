project(video)
set(HEADER_FILES
        video.h
)

set(SOURCE_FILES
        video.cpp

)

add_library(video STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(video PUBLIC .)
# 添加ffmpeg库
include_directories(/usr/local/ffmpeg/include)
link_directories(/usr/local/ffmpeg/lib)

target_link_libraries(video
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        swresample
        swscale
)