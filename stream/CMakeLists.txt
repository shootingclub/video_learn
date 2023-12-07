project(stream)
set(HEADER_FILES
        push_stream.h
)

set(SOURCE_FILES
        push_stream.cpp

)


add_library(stream STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(stream PUBLIC .)

# 添加ffmpeg库
include_directories(/opt/homebrew/Cellar/rtmpdump/2.4-20151223_3/include)
link_directories(/opt/homebrew/Cellar/rtmpdump/2.4-20151223_3/lib)

target_link_libraries(stream
        rtmp
)
