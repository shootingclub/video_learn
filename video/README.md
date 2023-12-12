uvvy

```shell
ffplay -pix_fmt uyvy422 -s 640x480 default.yuv
```

nv12

```shell
ffplay -pix_fmt nv12 -s 640x480 nv12.yuv
```

yuv420p

```shell
ffplay -s 640x480  yuv420.yuv 
```

yuv分量

```shell
1.
#提取yuv
ffmpeg -i lizi.mp4 -an -c:v rawvideo -pix_fmt yuv420p out.yuv
ffplay -pix_fmt yuv420p -s 1920x1080 out.yuv
#播放y分量
ffplay -s 640x480 -vf extractplanes='y' yuvH264.yuv
#播放u分量
ffplay -s 640x480 -vf extractplanes='u' yuvH264.yuv
#播放v分量
ffplay -s 640x480 -vf extractplanes='v' yuvH264.yuv

2.
#提取yuv各个分量
ffmpeg -i lizi.mp4 -filter_complex 'extractplanes=y+u+v[y][u][v]' -map '[y]' y.yuv -map '[u]' u.yuv -map '[v]' v.yuv
#播放y分量
ffplay -s 640x480 -pix_fmt gray y.yuv
#播放u分量 分辨率除以2
ffplay -s (640/2)x(480/2) -pix_fmt gray u.yuv
#播放v分量 分辨率除以2
ffplay -s (640/2)x(480/2) -pix_fmt gray v.yuv

```

转码h264

```shell
ffmpeg -s 640x480 -i  yuvH264.yuv -c:v libx264 -crf 23 output.h264
```

gop 图像组
按照相关性对帧进行分组