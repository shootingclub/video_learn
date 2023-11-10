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