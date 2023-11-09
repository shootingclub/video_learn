uvvy
```shell
ffplay -pix_fmt uyvy422 -s 640x480 uyvy.yuv
```
nv12
```shell
ffplay -pix_fmt nv12 -s 640x480 nv12.yuv
```
yuv420p
```shell
ffplay -s 640x480 -vf extractplanes='y' yuvH264.yuv
ffplay -s 640x480 -vf extractplanes='u' yuvH264.yuv
ffplay -s 640x480 -vf extractplanes='v' yuvH264.yuv

```
gop
按照相关性对帧进行分组