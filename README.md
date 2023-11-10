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

