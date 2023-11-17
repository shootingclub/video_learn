project(pushStream)
set(HEADER_FILES
        pushStream.h
)

set(SOURCE_FILES
        pushStream.cpp

)

add_library(pushStream STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(pushStream PUBLIC .)
# 添加ffmpeg库
include_directories(/opt/homebrew/Cellar/rtmpdump/2.4-20151223_3/include)
link_directories(/opt/homebrew/Cellar/rtmpdump/2.4-20151223_3/lib)

target_link_libraries(pushStream
        rtmp
)