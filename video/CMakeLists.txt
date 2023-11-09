project(video)
set(HEADER_FILES
        video.h
)

set(SOURCE_FILES
        video.cpp

)


add_library(video STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(video PUBLIC .)
