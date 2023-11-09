project(example)


set(SOURCE_FILES
        avio_list_dir.cpp
        avio_reading.cpp
        decode_audio.cpp
        decode_video.cpp
        demuxing_decoding.cpp
        encode_audio.cpp
        encode_video.cpp
        extract_mvs.cpp
        filter_audio.cpp
        filtering_audio.cpp
        filtering_video.cpp
        http_multiclient.cpp
        hw_decode.cpp
        metadata.cpp
        muxing.cpp
        qsvdec.cpp
        remuxing.cpp
        resampling_audio.cpp
        scaling_video.cpp
        transcode_aac.cpp
        transcoding.cpp
        vaapi_encode.cpp
        vaapi_transcode.cpp

)

add_executable(example ${SOURCE_FILES})

target_include_directories(example PUBLIC .)
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