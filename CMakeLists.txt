cmake_minimum_required(VERSION 3.15)
project(video_learn)
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)
# 设置运行架构
set(CMAKE_OSX_ARCHITECTURES "arm64")

set(FFMPEG_DIR /usr/local/ffmpeg) # FFmpeg的安装目录
include_directories(${FFMPEG_DIR}/include/) # 头文件路径
link_directories(${FFMPEG_DIR}/lib/) # 链接库路径

add_executable(video_learn main.cpp src/audio.h
        src/audio.cpp
)

target_link_libraries(video_learn
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        swresample
        swscale
        )

