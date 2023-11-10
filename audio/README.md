采样 mac m1

```shell
ffplay -ar 48000 -ac 1 -f f32le test.pcm
```

重采样 mac m1

```shell
ffplay -ar 44100 -ac 1 -f s16le test.pcm
```

编码aac

```shell
ffmpeg -ar 44100 -ac 1 -f s16le -i test.pcm -c:a libfdk_aac  output.aac
```

