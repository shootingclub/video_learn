# ffmpeg

| 环境       | 安装目录          | 环境变量        |
| ---------- | ----------------- | --------------- |
| ffmpeg 5.0 | /usr/local/ffmpeg | ~/.bash_profile |
|            |                   |                 |
|            |                   |                 |

## 依赖库安装

>macOS 采用源码安装的ffmpeg 可使用此方法安装libfdk-aac libx264 libx265
>sudo ./configure --prefix=/usr/local/ffmpeg/ --enable-shared --disable-static --enable-gpl --enable-nonfree --enable-libfdk-aac --enable-libx264 --enable-libx265
>sudo make -j 4
>sudo make install

##  代码结构

| 库            | 说明                               |
| ------------- | ---------------------------------- |
| libavcodec    | 提供了一系列编码实现               |
| libavformat   | 实现在流协议，容器格式及其本IO访问 |
| libavutil     | 包括hash器，解码器和各种工具函数   |
| libavfilter   | 提供了各种音视频过滤器             |
| libavdevice   | 提供了访问捕获设备和回放设备接口   |
| libswresample | 实现混音和重采样                   |
| libswscale    | 实现了色彩转换和缩放功能           |

# srs

| 环境                     | 安装目录                    | 环境变量srs-server-6.0-d0.tar.gz |
| ------------------------ | --------------------------- | -------------------------------- |
| srs-server-6.0-d0.tar.gz | /Users/yaohua/Downloads/srs |                                  |
|                          |                             |                                  |
|                          |                             |                                  |

## 编译

```bash
cd srs/trunk 
./configure  
make 
./objs/srs -c conf/srs.conf
```



## 修改配置

```nginx
# main config for srs.
# @see full.conf for detail config.

listen              1935;
max_connections     50;
#srs_log_tank        file;
#srs_log_file        ./objs/srs.log;
daemon              on;
http_api {
    enabled         on;
    listen          1985;
}
http_server {
    enabled         on;
    listen          8080;
    dir             ./objs/nginx/html;
}
rtc_server {
    enabled on;
    listen 8000; # UDP port
    # @see https://ossrs.net/lts/zh-cn/docs/v4/doc/webrtc#config-candidate
    candidate $CANDIDATE;
}
vhost __defaultVhost__ {
    hls {
        enabled         on;
    }
    http_remux {
        enabled     on;
        mount       [vhost]/[app]/[stream].flv;
    }
    rtc {
        enabled     on;
        # @see https://ossrs.net/lts/zh-cn/docs/v4/doc/webrtc#rtmp-to-rtc
        rtmp_to_rtc off;
        # @see https://ossrs.net/lts/zh-cn/docs/v4/doc/webrtc#rtc-to-rtmp
        rtc_to_rtmp off;
    }

    play{
        gop_cache_max_frames 2500;
    }
}
```

## 日志

```bash
tail -n 30 -f ./objs/srs.log
```

## summary

The build summary:
+------------------------------------------------------------------------------------
For SRS benchmark, gperf, gprof and valgrind, please read:
https://www.jianshu.com/p/6d4a89359352
+------------------------------------------------------------------------------------
|The main server usage: ./objs/srs -c conf/srs.conf, start the srs server
|     About HLS, please read https://ossrs.net/lts/zh-cn/docs/v4/doc/delivery-hls
|     About DVR, please read https://ossrs.net/lts/zh-cn/docs/v4/doc/dvr
|     About SSL, please read https://ossrs.net/lts/zh-cn/docs/v4/doc/rtmp-handshake
|     About transcoding, please read https://ossrs.net/lts/zh-cn/docs/v4/doc/ffmpeg
|     About ingester, please read https://ossrs.net/lts/zh-cn/docs/v4/doc/ingest
|     About http-callback, please read https://ossrs.net/lts/zh-cn/docs/v4/doc/http-callback
|     Aoubt http-server, please read https://ossrs.net/lts/zh-cn/docs/v4/doc/http-server
|     About http-api, please read https://ossrs.net/lts/zh-cn/docs/v4/doc/http-api
|     About stream-caster, please read https://ossrs.net/lts/zh-cn/docs/v4/doc/streamer
|     (Disabled) About VALGRIND, please read https://github.com/ossrs/state-threads/issues/2
+------------------------------------------------------------------------------------
binaries, please read https://ossrs.net/lts/zh-cn/docs/v4/doc/install
You can:

```bash
./objs/srs -c conf/srs.conf
```

to start the srs server, with config conf/srs.conf.



## 推流与拉流

### 推流

```bash
#视频
ffmpeg -re -stream_loop -1 -i /Users/yaohua/c_workspace/video_learn/player/video.mp4 -c:v copy -f flv rtmp://localhost/live/livestream

#音频
ffmpeg -re -stream_loop -1 -i /Users/yaohua/c_workspace/video_learn/player/video.mp4 -c:a copy -f flv rtmp://localhost/live/livestream

#音频+视频
ffmpeg -re -stream_loop -1 -i /Users/yaohua/c_workspace/video_learn/player/video.mp4 -c copy -f flv rtmp://localhost/live/livestream

# -i 后面是你要推流的本地视频地址
# rtmp 后面是你要将视频推送到的服务器IP
```

### 拉流

```bash
#  使用该命令拉流
ffplay http://localhost:8080/live/livestream.flv
```





