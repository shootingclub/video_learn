project(audio)
set(HEADER_FILES

)

set(SOURCE_FILES
        audio.cpp


)

add_executable(a audio.cpp)


add_library(audio STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(audio PUBLIC .)
# 添加ffmpeg库
include_directories(/usr/local/ffmpeg/include)
link_directories(/usr/local/ffmpeg/lib)

target_link_libraries(audio
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        swresample
        swscale
)