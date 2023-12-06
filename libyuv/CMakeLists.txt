project(yuvUtil)
set(HEADER_FILES
        yuvUtil.h
)

set(SOURCE_FILES
        yuvUtil.cpp


)


add_library(yuvUtil STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(yuvUtil PUBLIC .)
# 添加ffmpeg库
include_directories(/Users/yaohua/Downloads/libyuv/include)
link_directories(/Users/yaohua/Downloads/libyuv/lib)

target_link_libraries(yuvUtil
        yuv
)