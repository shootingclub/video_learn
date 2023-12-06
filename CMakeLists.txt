#cmake_minimum_required(VERSION 3.26)
cmake_minimum_required(VERSION 3.15)
project(video_learn)
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)
# 设置运行架构
set(CMAKE_OSX_ARCHITECTURES "arm64")

set(FFMPEG_DIR /usr/local/ffmpeg) # FFmpeg的安装目录
include_directories(${FFMPEG_DIR}/include/) # 头文件路径
link_directories(${FFMPEG_DIR}/lib/) # 链接库路径

#file(GLOB SRCS
#        audio *.cpp
#        video *.cpp
#
#
#)
set(SOURCE_FILES
        main.cpp
)

add_executable(video_learn ${SOURCE_FILES})


add_subdirectory(video)
add_subdirectory(audio)
add_subdirectory(player)
add_subdirectory(pushStream)
add_subdirectory(libyuv)
add_subdirectory(test)
add_subdirectory(examples)

include_directories(/usr/local/ffmpeg/include)
link_directories(/usr/local/ffmpeg/lib)

target_link_libraries(video_learn
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        swresample
        swscale
)



