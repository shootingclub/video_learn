#include "player.h"

namespace player {

    int player::select_best_sample_rate(const AVCodec *codec) {
        const int *p;
        int best_samplerate = 0;

        if (!codec->supported_samplerates)
            return 44100;

        p = codec->supported_samplerates;
        while (*p) {
            if (!best_samplerate || abs(44100 - *p) < abs(44100 - best_samplerate))
                best_samplerate = *p;
            p++;
        }
        return best_samplerate;
    }

    int player::check_sample_fmt(const AVCodec *codec, AVSampleFormat sample_fmt) {
        const enum AVSampleFormat *p = codec->sample_fmts;
        // 查找sample_fmt 最后一项是AV_SAMPLE_FMT_NONE，如果不是继续循环，直到最后
        while (*p != AV_SAMPLE_FMT_NONE) {
            if (*p == sample_fmt) {
                return 1;
            }
            p++;
        }
        return 0;
    }

    int player::select_channel_layout(const AVCodec *codec) {
        const uint64_t *p;
        uint64_t best_ch_layout = 0;
        int best_nb_channels = 0;
        if (!codec->channel_layouts) {
            return AV_CH_LAYOUT_STEREO;
        }
        p = codec->channel_layouts;
        while (*p) {
            int nb_channels = av_get_channel_layout_nb_channels(*p);
            if (nb_channels > best_nb_channels) {
                best_ch_layout = *p;
                best_nb_channels = nb_channels;
            }
            p++;
        }
        return best_ch_layout;
    }

    int player::encode(AVCodecContext *avCodecContext, AVFrame *avFrame, AVPacket *avPacket, FILE *out) {
        int ret = -1;
        ret = avcodec_send_frame(avCodecContext, avFrame);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "failed to send frame to encoder");
            exit(-1);
        }
        while (ret >= 0) {
            ret = avcodec_receive_packet(avCodecContext, avPacket);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                return 0;
            } else if (ret < 0) {
                av_log(nullptr, AV_LOG_ERROR, "failed to receive frame to encoder");
                return -1;
            }
            fwrite(avPacket->data, 1, avPacket->size, out);
            av_packet_unref(avPacket);
        }
    }

    // 设置日志级别 AV_LOG_DEBUG
    void set_log(int level) {

        av_log_set_level(level);
        av_log(nullptr, AV_LOG_INFO, "hello world \n");
    }

    void player::extract_audio(char *filename) {
        int ret;
        int idx;
        AVFormatContext *pFmtCtx = nullptr;
        //1.打开音频
        ret = avformat_open_input(&pFmtCtx, filename, nullptr, nullptr);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "%s \n", av_err2str(ret));
            exit(-1);
        }
        // 2.找到音频
        // wanted_stream_nb:想查找的流id，如果是-1那么返回找到的第一个音频流并发索引序号返回
        idx = av_find_best_stream(pFmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
        if (idx < 0) {
            av_log(pFmtCtx, AV_LOG_ERROR, "does not include audio stream  \n");
            exit(-1);
        }

    }

    void player::media_info(char *filename) {
        int err_code;
        char errors[1024];
        const char *video_type, *audio_type;


        AVFormatContext *fmt_ctx = NULL;

        /* open input file, and allocate format context */
        err_code = avformat_open_input(&fmt_ctx, filename, NULL, NULL);
        if (err_code < 0) {
            av_strerror(err_code, errors, 1024);
            fprintf(stderr, "Could not open source file %s, %d(%s)\n", filename, err_code, errors);
            exit(1);
        }

        /* retreive stream information */
        err_code = avformat_find_stream_info(fmt_ctx, NULL);
        if (err_code < 0) {
            av_strerror(err_code, errors, 1024);
            fprintf(stderr, "Could not open source file %s, %d(%s)\n", filename, err_code, errors);
            exit(1);
        }

//        try {
//            // 可能抛出异常的代码
//
//        } catch (const std::runtime_error &e) {
//            // 捕获并处理运行时错误
//            std::cerr << "捕获到运行时错误： " << e.what() << std::endl;
//        }
//        for (int i = 0; i < sizeof(fmt_ctx->streams); ++i) {
//            if (fmt_ctx->streams[i]) {
//                // 4. find  the video decoder and fetch video data
//                const AVCodec *codec = avcodec_find_decoder(fmt_ctx->streams[i]->codecpar->codec_id);
//                if (codec) {
//                    // video_type = video_codec->name; //Better
//                    video_type = avcodec_get_name(codec->id); // Debug test
//                    std::cout << "stream number is: " << i << " codec is: " << video_type << std::endl;
//                } else {
//                    std::cerr << "Find video codec failed !" << std::endl;
//                }
//            }
//        }



        /* dump input information to stderr */
        av_dump_format(fmt_ctx, 0, filename, 0);

        /* close input file */
        avformat_close_input(&fmt_ctx);
    }

    void player::extract_audio_base(char *srcAudioFile, char *dstAudioFile) {
        AVFormatContext *pFmtCtx = nullptr;
        AVFormatContext *oFmtCtx = nullptr;
        const AVOutputFormat *outFmt;
        AVStream *outStream;
        AVStream *inStream;
        AVPacket pkt;
        // 1.处理一些参数（日志级别）
        int ret;
        int idx;
        av_log_set_level(AV_LOG_DEBUG);
        // 2.打开多媒体文件
//        AVInputFormat: 如果不指定音频格式，那么它会按照src音频文件的后缀名称进行音频解析，如果指定可以使用如下参数：
//        在FFmpeg的AVInputFormat结构中，音频格式的常量枚举值有很多种。以下是一些常见的音频格式常量：
//        AV_INPUT_FORMAT_NAME_AUTO：自动选择正确的输入格式。
//        AV_INPUT_FORMAT_NAME_MPEG：MPEG音频格式。
//        AV_INPUT_FORMAT_NAME_MP3：MP3音频格式。
//        AV_INPUT_FORMAT_NAME_WAV：WAV音频格式。
//        AV_INPUT_FORMAT_NAME_PCM：PCM音频格式。
//        AV_INPUT_FORMAT_NAME_AAC：AAC音频格式。
//        AV_INPUT_FORMAT_NAME_AC3：AC-3音频格式。
//        AV_INPUT_FORMAT_NAME_DTS：DTS音频格式。
//        AV_INPUT_FORMAT_NAME_VORBIS：Vorbis音频格式。
//        AV_INPUT_FORMAT_NAME_FLAC：FLAC音频格式。
//        AV_INPUT_FORMAT_NAME_ADPCM_EA：ADPCM EA音频格式。
//        AV_INPUT_FORMAT_NAME_ADPCM_IMA_EA：ADPCM IMA EA音频格式。
//        AV_INPUT_FORMAT_NAME_ADPCM_MS：ADPCM MS音频格式。
//        AV_INPUT_FORMAT_NAME_ALAC：ALAC音频格式。
//        AV_INPUT_FORMAT_NAME_AMR_NB：AMR-NB音频格式。
//        AV_INPUT_FORMAT_NAME_AMR_WB：AMR-WB音频格式。
//        AV_INPUT_FORMAT_NAME_FLV：FLV音频格式。
//        AV_INPUT_FORMAT_NAME_G722：G.722音频格式。
//        AV_INPUT_FORMAT_NAME_G723_1：G.723.1音频格式。
//        AV_INPUT_FORMAT_NAME_G726：G.726音频格式。
//        AVInputFormat结构体中还有很多其他的音频格式常量，
//        这里只列举了一些常见的例子。你可以查阅FFmpeg的文档或源代码以获取更全面的列表。
        std::cout << "打印输入流信息" << std::endl;
        ret = avformat_open_input(&pFmtCtx, srcAudioFile, nullptr, nullptr);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "%s \n", av_err2str(ret));
            exit(-1);
        }
        //首先找到输入文件流,打印输入文件信息（音频、视频）
//        std::cout << "打印输入媒体信息" << std::endl;
//        media_info(srcAudioFile);
        // 3.从多媒体文件中找到音频
        // wanted_stream_nb:想查找的流id，如果是-1那么返回找到的第一个音频流并发索引序号返回
        idx = av_find_best_stream(pFmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
        if (idx < 0) {
            av_log(pFmtCtx, AV_LOG_ERROR, "does not include audio stream  \n");
            goto _ERROR;
        }
        // 4.打开目的文件上下文,设置一些参数
        oFmtCtx = avformat_alloc_context();
        if (!oFmtCtx) {
            av_log(nullptr, AV_LOG_ERROR, "no memory \n");
            goto _ERROR;
        }
        outFmt = av_guess_format(nullptr, dstAudioFile, nullptr);
        //将输出目标文件的信息 设置给了输出上下文
        oFmtCtx->oformat = outFmt;
        // 5.为目的文件，创建一个新的音频流
        outStream = avformat_new_stream(oFmtCtx, nullptr);
        // 6.为新的输出音频流设置参数
        inStream = pFmtCtx->streams[idx];
        //假设输入流的音频参数（编解码器） 与输出音频流的音频参数（编解码器）一致，那么直接copy
        avcodec_parameters_copy(outStream->codecpar, inStream->codecpar);
        //根据多媒体文件自动适配编解码器
        outStream->codecpar->codec_tag = 0;
        //绑定（输出上下文与我们的目标文件绑定在一起）
        ret = avio_open2(&oFmtCtx->pb, dstAudioFile, AVIO_FLAG_WRITE, nullptr, nullptr);
        if (ret < 0) {
            av_log(oFmtCtx, AV_LOG_ERROR, "%s", av_err2str(ret));
            goto _ERROR;
        }
        // 7.写多媒体文件头到目的文件
        ret = avformat_write_header(oFmtCtx, nullptr);
        if (ret < 0) {
            av_log(oFmtCtx, AV_LOG_ERROR, "%s", av_err2str(ret));
            goto _ERROR;
        }
        // 8.从多媒体文件汇总读取到音频数据到目的文件
        while (av_read_frame(pFmtCtx, &pkt) >= 0) {
            //读取到的流的index 是我们想要读取的idx
            if (pkt.stream_index == idx) {
                //将读取到的音频包进行设置,然后写入目标输出文件的上下文中
                pkt.pts = av_rescale_q_rnd(pkt.pts,
                                           inStream->time_base,
                                           outStream->time_base,
                                           (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
                pkt.dts = pkt.pts;
                pkt.duration = av_rescale_q(pkt.duration, inStream->time_base, outStream->time_base);
                pkt.stream_index = 0;
                pkt.pos = -1;
                av_interleaved_write_frame(oFmtCtx, &pkt);
                av_packet_unref(&pkt);
            }
        }

        // 9.写多媒体文件到文件中
        av_write_trailer(oFmtCtx);
        // 10.将申请的资源释放掉



        _ERROR:
        if (pFmtCtx) {
            avformat_close_input(&pFmtCtx);
            pFmtCtx = nullptr;
        }
        if (oFmtCtx) {
            avformat_close_input(&oFmtCtx);
            pFmtCtx = nullptr;
            if (oFmtCtx->pb) {
                avio_close(oFmtCtx->pb);
            }
        }
        printf(" error finish");
        exit(-1);

    }

    void player::extract_video_base(char *srcAudioFile, char *dstAudioFile) {
        AVFormatContext *pFmtCtx = nullptr;
        AVFormatContext *oFmtCtx = nullptr;
        const AVOutputFormat *outFmt;
        AVStream *outStream;
        AVStream *inStream;
        AVPacket pkt;
        // 1.处理一些参数（日志级别）
        int ret;
        int idx;
        av_log_set_level(AV_LOG_DEBUG);
        // 2.打开多媒体文件
//        AVInputFormat: 如果不指定音频格式，那么它会按照src音频文件的后缀名称进行音频解析，如果指定可以使用如下参数：
//        在FFmpeg的AVInputFormat结构中，音频格式的常量枚举值有很多种。以下是一些常见的音频格式常量：
//        AV_INPUT_FORMAT_NAME_AUTO：自动选择正确的输入格式。
//        AV_INPUT_FORMAT_NAME_MPEG：MPEG音频格式。
//        AV_INPUT_FORMAT_NAME_MP3：MP3音频格式。
//        AV_INPUT_FORMAT_NAME_WAV：WAV音频格式。
//        AV_INPUT_FORMAT_NAME_PCM：PCM音频格式。
//        AV_INPUT_FORMAT_NAME_AAC：AAC音频格式。
//        AV_INPUT_FORMAT_NAME_AC3：AC-3音频格式。
//        AV_INPUT_FORMAT_NAME_DTS：DTS音频格式。
//        AV_INPUT_FORMAT_NAME_VORBIS：Vorbis音频格式。
//        AV_INPUT_FORMAT_NAME_FLAC：FLAC音频格式。
//        AV_INPUT_FORMAT_NAME_ADPCM_EA：ADPCM EA音频格式。
//        AV_INPUT_FORMAT_NAME_ADPCM_IMA_EA：ADPCM IMA EA音频格式。
//        AV_INPUT_FORMAT_NAME_ADPCM_MS：ADPCM MS音频格式。
//        AV_INPUT_FORMAT_NAME_ALAC：ALAC音频格式。
//        AV_INPUT_FORMAT_NAME_AMR_NB：AMR-NB音频格式。
//        AV_INPUT_FORMAT_NAME_AMR_WB：AMR-WB音频格式。
//        AV_INPUT_FORMAT_NAME_FLV：FLV音频格式。
//        AV_INPUT_FORMAT_NAME_G722：G.722音频格式。
//        AV_INPUT_FORMAT_NAME_G723_1：G.723.1音频格式。
//        AV_INPUT_FORMAT_NAME_G726：G.726音频格式。
//        AVInputFormat结构体中还有很多其他的音频格式常量，
//        这里只列举了一些常见的例子。你可以查阅FFmpeg的文档或源代码以获取更全面的列表。
        std::cout << "打印输入流信息" << std::endl;
        ret = avformat_open_input(&pFmtCtx, srcAudioFile, nullptr, nullptr);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "%s \n", av_err2str(ret));
            exit(-1);
        }
        //首先找到输入文件流,打印输入文件信息（音频、视频）
        std::cout << "打印输入媒体信息" << std::endl;
        media_info(srcAudioFile);
        // 3.从多媒体文件中找到视频
        // wanted_stream_nb:想查找的流id，如果是-1那么返回找到的第一个音频流并发索引序号返回
        idx = av_find_best_stream(pFmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
        if (idx < 0) {
            av_log(pFmtCtx, AV_LOG_ERROR, "does not include audio stream  \n");
            goto _ERROR;
        }
        // 4.打开目的文件上下文,设置一些参数
        oFmtCtx = avformat_alloc_context();
        if (!oFmtCtx) {
            av_log(nullptr, AV_LOG_ERROR, "no memory \n");
            goto _ERROR;
        }
        outFmt = av_guess_format(nullptr, dstAudioFile, nullptr);
        //将输出目标文件的信息 设置给了输出上下文
        oFmtCtx->oformat = outFmt;
        // 5.为目的文件，创建一个新的视频流
        outStream = avformat_new_stream(oFmtCtx, nullptr);
        // 6.为新的输出视频流设置参数
        inStream = pFmtCtx->streams[idx];
        //假设输入流的音频参数（编解码器） 与输出音频流的音频参数（编解码器）一致，那么直接copy
        avcodec_parameters_copy(outStream->codecpar, inStream->codecpar);
        //根据多媒体文件自动适配编解码器
        outStream->codecpar->codec_tag = 0;
        //绑定（输出上下文与我们的目标文件绑定在一起）
        ret = avio_open2(&oFmtCtx->pb, dstAudioFile, AVIO_FLAG_WRITE, nullptr, nullptr);
        if (ret < 0) {
            av_log(oFmtCtx, AV_LOG_ERROR, "%s", av_err2str(ret));
            goto _ERROR;
        }
        // 7.写多媒体文件头到目的文件
        ret = avformat_write_header(oFmtCtx, nullptr);
        if (ret < 0) {
            av_log(oFmtCtx, AV_LOG_ERROR, "%s", av_err2str(ret));
            goto _ERROR;
        }
        // 8.从多媒体文件汇总读取到音频数据到目的文件
        while (av_read_frame(pFmtCtx, &pkt) >= 0) {
            //读取到的流的index 是我们想要读取的idx
            if (pkt.stream_index == idx) {
                //将读取到的音频包进行设置,然后写入目标输出文件的上下文中
                pkt.pts = av_rescale_q_rnd(pkt.pts,
                                           inStream->time_base,
                                           outStream->time_base,
                                           (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
                pkt.dts = av_rescale_q_rnd(pkt.dts,
                                           inStream->time_base,
                                           outStream->time_base,
                                           (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));;
                pkt.duration = av_rescale_q(pkt.duration, inStream->time_base, outStream->time_base);
                pkt.stream_index = 0;
                pkt.pos = -1;
                av_interleaved_write_frame(oFmtCtx, &pkt);
                av_packet_unref(&pkt);
            }
        }

        // 9.写多媒体文件到文件中
        av_write_trailer(oFmtCtx);
        // 10.将申请的资源释放掉



        _ERROR:
        if (pFmtCtx) {
            avformat_close_input(&pFmtCtx);
            pFmtCtx = nullptr;
        }
        if (oFmtCtx) {
            avformat_close_input(&oFmtCtx);
            pFmtCtx = nullptr;
            if (oFmtCtx->pb) {
                avio_close(oFmtCtx->pb);
            }
        }
        printf(" error finish");
        exit(-1);
    }

    void player::remux_base(char *srcAudioFile, char *dstAudioFile) {
        AVFormatContext *pFmtCtx = nullptr;
        AVFormatContext *oFmtCtx = nullptr;

        AVPacket pkt;
        int ret;
        int idx = 0;
        int *streamMap = nullptr;
        // 1.处理一些参数（日志级别）
        av_log_set_level(AV_LOG_DEBUG);
        // 2.打开多媒体文件
        std::cout << "打印输入流信息" << std::endl;
        ret = avformat_open_input(&pFmtCtx, srcAudioFile, nullptr, nullptr);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "%s \n", av_err2str(ret));
            exit(-1);
        }
        //首先找到输入文件流,打印输入文件信息（音频、视频）
        std::cout << "打印输入媒体信息" << std::endl;
        media_info(srcAudioFile);
        // 3.设置输出流上下文
        avformat_alloc_output_context2(&oFmtCtx, nullptr, nullptr, dstAudioFile);
        if (!oFmtCtx) {
            av_log(nullptr, AV_LOG_ERROR, "no memory  \n");
            goto _ERROR;
        }
        streamMap = (int *) av_calloc(pFmtCtx->nb_streams, sizeof(int));
        if (!streamMap) {
            av_log(nullptr, AV_LOG_ERROR, "no memory  \n");
            goto _ERROR;
        }
        //对输入流进行遍历 要把不是音频 视频 字幕 的流剔除掉
        for (int i = 0; i < pFmtCtx->nb_streams; i++) {
            AVStream *outStream;
            AVStream *inStream = pFmtCtx->streams[i];
            AVCodecParameters *inCodecPar = inStream->codecpar;
            if (inCodecPar->codec_type != AVMEDIA_TYPE_AUDIO &&
                inCodecPar->codec_type != AVMEDIA_TYPE_VIDEO &&
                inCodecPar->codec_type != AVMEDIA_TYPE_SUBTITLE
                    ) {
                streamMap[i] = -1;
                continue;
            } else {
                streamMap[i] = idx++;
                // 5.为目的文件，创建一个新的视频流
                outStream = avformat_new_stream(oFmtCtx, nullptr);
                if (!outStream) {
                    av_log(oFmtCtx, AV_LOG_ERROR, "no memory  \n");
                    goto _ERROR;
                }
                avcodec_parameters_copy(outStream->codecpar, inStream->codecpar);
                outStream->codecpar->codec_tag = 0;
            }
        }

        //绑定（输出上下文与我们的目标文件绑定在一起）
        ret = avio_open2(&oFmtCtx->pb, dstAudioFile, AVIO_FLAG_WRITE, nullptr, nullptr);
        if (ret < 0) {
            av_log(oFmtCtx, AV_LOG_ERROR, "%s", av_err2str(ret));
            goto _ERROR;
        }
        // 7.写多媒体文件头到目的文件
        ret = avformat_write_header(oFmtCtx, nullptr);
        if (ret < 0) {
            av_log(oFmtCtx, AV_LOG_ERROR, "%s", av_err2str(ret));
            goto _ERROR;
        }
        // 8.从多媒体文件汇总读取到音频数据到目的文件
        while (av_read_frame(pFmtCtx, &pkt) >= 0) {
            AVStream *outStream, *inStream;
            inStream = pFmtCtx->streams[pkt.stream_index];
            if (streamMap[pkt.stream_index] < 0) {
                av_packet_unref(&pkt);
                continue;
            }
            pkt.stream_index = streamMap[pkt.stream_index];
            outStream = oFmtCtx->streams[pkt.stream_index];
            av_packet_rescale_ts(&pkt, inStream->time_base, outStream->time_base);
            pkt.pos = -1;
            av_interleaved_write_frame(oFmtCtx, &pkt);
            av_packet_unref(&pkt);
        }



        // 9.写多媒体文件到文件中
        av_write_trailer(oFmtCtx);
        // 10.将申请的资源释放掉



        _ERROR:
        if (pFmtCtx) {
            avformat_close_input(&pFmtCtx);
            pFmtCtx = nullptr;
        }
        if (oFmtCtx) {
            avformat_close_input(&oFmtCtx);
            pFmtCtx = nullptr;
            if (oFmtCtx->pb) {
                avio_close(oFmtCtx
                                   ->pb);
            }
        }
        if (streamMap) {
            av_free(streamMap);
        }
        printf(" error finish");
        exit(-1);


    }

    void player::cut_base(char *srcAudioFile, char *dstAudioFile, double startTime, double endTime) {
        AVFormatContext *pFmtCtx = nullptr;
        AVFormatContext *oFmtCtx = nullptr;
        int64_t *dtsStartTime, *ptsStartTime;

        AVPacket pkt;
        int ret;
        int idx = 0;
        int *streamMap = nullptr;
        // 1.处理一些参数（日志级别）
        av_log_set_level(AV_LOG_DEBUG);
        // 2.打开多媒体文件
        std::cout << "打印输入流信息" << std::endl;
        ret = avformat_open_input(&pFmtCtx, srcAudioFile, nullptr, nullptr);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "%s \n", av_err2str(ret));
            exit(-1);
        }
        //首先找到输入文件流,打印输入文件信息（音频、视频）
        std::cout << "打印输入媒体信息" << std::endl;
        media_info(srcAudioFile);
        // 3.设置输出流上下文
        avformat_alloc_output_context2(&oFmtCtx, nullptr, nullptr, dstAudioFile);
        if (!oFmtCtx) {
            av_log(nullptr, AV_LOG_ERROR, "no memory  \n");
            goto _ERROR;
        }
        streamMap = (int *) av_calloc(pFmtCtx->nb_streams, sizeof(int));
        if (!streamMap) {
            av_log(nullptr, AV_LOG_ERROR, "no memory  \n");
            goto _ERROR;
        }
        //对输入流进行遍历 要把不是音频 视频 字幕 的流剔除掉
        for (int i = 0; i < pFmtCtx->nb_streams; i++) {
            AVStream *outStream;
            //获取源文件的流，索引从0开始
            AVStream *inStream = pFmtCtx->streams[i];
            AVCodecParameters *inCodecPar = inStream->codecpar;
            if (inCodecPar->codec_type != AVMEDIA_TYPE_AUDIO &&
                inCodecPar->codec_type != AVMEDIA_TYPE_VIDEO &&
                inCodecPar->codec_type != AVMEDIA_TYPE_SUBTITLE
                    ) {
                streamMap[i] = -1;
                continue;
            } else {
                //将源文件的流索引记录 ，与目标流文件做对应
                //保证目标的文件的流索引是顺序的，有可能源文件的索引不是顺序的
                streamMap[i] = idx++;
                // 5.为目的文件，创建一个新的视频流
                outStream = avformat_new_stream(oFmtCtx, nullptr);
                if (!outStream) {
                    av_log(oFmtCtx, AV_LOG_ERROR, "no memory  \n");
                    goto _ERROR;
                }
                //源文件的参数拷贝到目标文件
                avcodec_parameters_copy(outStream->codecpar, inStream->codecpar);
                outStream->codecpar->codec_tag = 0;
            }
        }

        //绑定（输出上下文与我们的目标文件绑定在一起）
        ret = avio_open2(&oFmtCtx->pb, dstAudioFile, AVIO_FLAG_WRITE, nullptr, nullptr);
        if (ret < 0) {
            av_log(oFmtCtx, AV_LOG_ERROR, "%s", av_err2str(ret));
            goto _ERROR;
        }
        // 7.写多媒体文件头到目的文件
        ret = avformat_write_header(oFmtCtx, nullptr);
        if (ret < 0) {
            av_log(oFmtCtx, AV_LOG_ERROR, "%s", av_err2str(ret));
            goto _ERROR;
        }
        //先跳到指定的地方
        ret = av_seek_frame(pFmtCtx, -1, startTime * AV_TIME_BASE, AVSEEK_FLAG_BACKWARD);
        if (ret < 0) {
            av_log(oFmtCtx, AV_LOG_ERROR, "%s", av_err2str(ret));
            goto _ERROR;
        }

        dtsStartTime = (int64_t *) av_calloc(pFmtCtx->nb_streams, sizeof(int64_t));
        for (int i = 0; i < pFmtCtx->nb_streams; ++i) {
            dtsStartTime[i] = -1;
        }
        ptsStartTime = (int64_t *) av_calloc(pFmtCtx->nb_streams, sizeof(int64_t));
        for (int i = 0; i < pFmtCtx->nb_streams; ++i) {
            ptsStartTime[i] = -1;
        }
        // 8.从多媒体文件汇总读取到音频数据到目的文件
        while (av_read_frame(pFmtCtx, &pkt) >= 0) {
            AVStream *outStream, *inStream;
            if (dtsStartTime[pkt.stream_index] == -1 && pkt.dts > 0) {
                dtsStartTime[pkt.stream_index] = pkt.dts;
            }
            if (ptsStartTime[pkt.stream_index] == -1 && pkt.pts > 0) {
                ptsStartTime[pkt.stream_index] = pkt.pts;
            }
            inStream = pFmtCtx->streams[pkt.stream_index];
            if (av_q2d(inStream->time_base) * pkt.pts > endTime) {
                av_log(oFmtCtx, AV_LOG_INFO, "success !\n");
                break;
            }
            if (streamMap[pkt.stream_index] < 0) {
                av_packet_unref(&pkt);
                continue;
            }

            pkt.pts = pkt.pts - ptsStartTime[pkt.stream_index];
            pkt.dts = pkt.dts - dtsStartTime[pkt.stream_index];
            if (pkt.dts > pkt.pts) {
                pkt.pts = pkt.dts;
            }

            pkt.stream_index = streamMap[pkt.stream_index];
            outStream = oFmtCtx->streams[pkt.stream_index];
            av_packet_rescale_ts(&pkt, inStream->time_base, outStream->time_base);
            pkt.pos = -1;
            av_interleaved_write_frame(oFmtCtx, &pkt);
            av_packet_unref(&pkt);
        }



        // 9.写多媒体文件到文件中
        av_write_trailer(oFmtCtx);
        // 10.将申请的资源释放掉


        _ERROR:
        if (pFmtCtx) {
            avformat_close_input(&pFmtCtx);
            pFmtCtx = nullptr;
        }
        if (oFmtCtx) {
            avformat_close_input(&oFmtCtx);
            pFmtCtx = nullptr;
            if (oFmtCtx->pb) {
                avio_close(oFmtCtx->pb);
            }
        }
        if (streamMap) {
            av_free(streamMap);
        }
        if (dtsStartTime) {
            av_free(dtsStartTime);
        }
        if (ptsStartTime) {
            av_free(ptsStartTime);
        }
        printf(" error finish");
        exit(-1);

    }

    void player::av_merge_base(char *srcAudioFile, char *srcVideoFile, char *dstFile) {
        AVFormatContext *audioFmtCtx = nullptr;
        AVStream *audioInStream, *audioOutStream;

        AVFormatContext *videoFmtCtx = nullptr;
        AVStream *videoInStream, *videoOutStream;

        AVFormatContext *mergeFmtCtx;
        const AVOutputFormat *mergeOutPutFmt;


        AVPacket pkt;

        int ret, audioStreamId, videoStreamId;
        av_log_set_level(AV_LOG_DEBUG);

        //1. 抽取音频
        ret = avformat_open_input(&audioFmtCtx, srcAudioFile, nullptr, nullptr);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "%s \n", av_err2str(ret));
            exit(-1);
        }
        // 查找音频流
        audioStreamId = av_find_best_stream(audioFmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
        if (audioStreamId < 0) {
            av_log(audioFmtCtx, AV_LOG_ERROR, "does not include audio stream  \n");
        }

        //2. 抽取视频
        ret = avformat_open_input(&videoFmtCtx, srcVideoFile, nullptr, nullptr);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "%s \n", av_err2str(ret));
            exit(-1);
        }
        // 查找视频流
        videoStreamId = av_find_best_stream(videoFmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
        if (videoStreamId < 0) {
            av_log(audioFmtCtx, AV_LOG_ERROR, "does not include audio stream  \n");
        }

        // 3.打开dstFile目的文件上下文,设置一些参数
        mergeFmtCtx = avformat_alloc_context();
        if (!mergeFmtCtx) {
            av_log(nullptr, AV_LOG_ERROR, "no memory \n");
            goto _ERROR;
        }
        mergeOutPutFmt = av_guess_format(nullptr, dstFile, nullptr);
        //将输出目标文件的信息 设置给了输出上下文
        mergeFmtCtx->oformat = mergeOutPutFmt;


        // 为目的文件，创建一个新的视频流
        videoOutStream = avformat_new_stream(mergeFmtCtx, nullptr);
        // 为新的输出视频流设置参数
        videoInStream = videoFmtCtx->streams[videoStreamId];
        //假设输入流的音频参数（编解码器） 与输出音频流的音频参数（编解码器）一致，那么直接copy
        avcodec_parameters_copy(videoOutStream->codecpar, videoInStream->codecpar);
        //根据多媒体文件自动适配编解码器
        videoOutStream->codecpar->codec_tag = 0;


        // 为目的文件，创建一个新的音频流
        audioOutStream = avformat_new_stream(mergeFmtCtx, nullptr);
        // 为新的输出音频流设置参数
        audioInStream = audioFmtCtx->streams[audioStreamId];
        // 假设输入流的音频参数（编解码器） 与输出音频流的音频参数（编解码器）一致，那么直接copy
        avcodec_parameters_copy(audioOutStream->codecpar, audioInStream->codecpar);
        // 根据多媒体文件自动适配编解码器
        audioOutStream->codecpar->codec_tag = 0;


        //绑定（输出上下文与我们的目标文件绑定在一起）
        ret = avio_open2(&mergeFmtCtx->pb, dstFile, AVIO_FLAG_WRITE, nullptr, nullptr);
        if (ret < 0) {
            av_log(mergeFmtCtx, AV_LOG_ERROR, "%s", av_err2str(ret));
            goto _ERROR;
        }

        // 4.写多媒体文件头到目的文件
        ret = avformat_write_header(mergeFmtCtx, nullptr);
        if (ret < 0) {
            av_log(mergeFmtCtx, AV_LOG_ERROR, "%s", av_err2str(ret));
            goto _ERROR;
        }

        av_log(nullptr, AV_LOG_INFO, "开始视频抽取\n");
        // 5.从多媒体文件汇总读取到音频数据到目的文件
        while (av_read_frame(videoFmtCtx, &pkt) >= 0) {
            //读取到的流的index 是我们想要读取的idx
            if (pkt.stream_index == videoStreamId) {
                //将读取到的音频包进行设置,然后写入目标输出文件的上下文中
                pkt.pts = av_rescale_q_rnd(pkt.pts,
                                           videoInStream->time_base,
                                           videoOutStream->time_base,
                                           (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
                pkt.dts = av_rescale_q_rnd(pkt.dts,
                                           videoInStream->time_base,
                                           videoOutStream->time_base,
                                           (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));;
                pkt.duration = av_rescale_q(pkt.duration, videoInStream->time_base, videoOutStream->time_base);
                pkt.stream_index = videoStreamId;
                pkt.pos = -1;
                av_interleaved_write_frame(mergeFmtCtx, &pkt);
                av_packet_unref(&pkt);
            }
        }


        av_log(nullptr, AV_LOG_INFO, "开始音频抽取\n");
        // 8.从多媒体文件汇总读取到音频数据到目的文件
        while (av_read_frame(audioFmtCtx, &pkt) >= 0) {
            //读取到的流的index 是我们想要读取的idx
            if (pkt.stream_index == audioStreamId) {
                //将读取到的音频包进行设置,然后写入目标输出文件的上下文中
                pkt.pts = av_rescale_q_rnd(pkt.pts,
                                           audioInStream->time_base,
                                           audioOutStream->time_base,
                                           (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
                pkt.dts = pkt.pts;
                pkt.duration = av_rescale_q(pkt.duration, audioInStream->time_base, audioOutStream->time_base);
                pkt.stream_index = audioStreamId;
                pkt.pos = -1;
                av_interleaved_write_frame(mergeFmtCtx, &pkt);
                av_packet_unref(&pkt);
            }
        }

        // 9.写多媒体文件到文件中
        av_write_trailer(mergeFmtCtx);

        _ERROR:
        if (audioFmtCtx) {
            avformat_close_input(&audioFmtCtx);
            audioFmtCtx = nullptr;
        }
        printf(" error finish");
        exit(-1);
        //组成新的文件
    }

    void player::encode_video(char *codecName, char *dstFile) {

        int ret;
        FILE *file = nullptr;
        AVPacket *packet = nullptr;
        AVFrame *frame = nullptr;
        const AVCodec *codec = NULL;
        AVCodecContext *enc_ctx;
        //1.输入参数
        av_log_set_level(AV_LOG_DEBUG);
        //2.查找编码器
        codec = avcodec_find_encoder_by_name(codecName);
        if (!codec) {
            printf("codec libx264 not found \n");
            exit(-1);
        }
        enc_ctx = avcodec_alloc_context3(codec);
        if (!enc_ctx) {
            printf("avcodec_alloc_context3 failed \n");
            exit(-1);
        }

        //4.设置编码器参数
        enc_ctx->width = 480;
        enc_ctx->height = 640;
        //码率
        enc_ctx->bit_rate = 500000;
        //时间积：25帧/s
        enc_ctx->time_base = {1, 25};
        //帧率：25帧
        enc_ctx->framerate = {25, 1};
        //每10帧为一组
        enc_ctx->gop_size = 10;
        //最大支持多少个b帧，一般超过3帧
        enc_ctx->max_b_frames = 1;
        //视频源的类型
        enc_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
        //设置h264编码器私有属性
        if (codec->id == AV_CODEC_ID_H264) {
            av_opt_set(enc_ctx->priv_data, "preset", "slow", 0);
        }
        //5.编码器与编码器上下文绑定到一起
        ret = avcodec_open2(enc_ctx, codec, nullptr);
        if (ret < 0) {
            av_log(enc_ctx, AV_LOG_ERROR, "dont open codec %s \n", av_err2str(ret));
            goto _ERROR;
        }
        //6.创建输出文件
        file = fopen(dstFile, "wb");
        if (!file) {
            av_log(nullptr, AV_LOG_ERROR, "dont open dst file %s \n", av_err2str(ret));
            goto _ERROR;
        }
        //7.创建AVFrame
        frame = av_frame_alloc();
        if (!frame) {
            av_log(nullptr, AV_LOG_ERROR, " no memory  \n");
            goto _ERROR;
        }
        frame->width = enc_ctx->width;
        frame->height = enc_ctx->height;
        frame->format = enc_ctx->pix_fmt;

        ret = av_frame_get_buffer(frame, 0);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, " could not allocate the video frame  \n");
            goto _ERROR;
        }
        //8.创建avpacket
        packet = av_packet_alloc();
        if (!packet) {
            av_log(nullptr, AV_LOG_ERROR, " no memory  \n");
            goto _ERROR;
        }
        //9.生成视频内容
        for (int i = 0; i < 25; ++i) {
            ret = av_frame_make_writable(frame);
            if (ret < 0) {
                break;
            }
            //y分量
            for (int y = 0; y < enc_ctx->height; y++) {
                for (int x = 0; x < enc_ctx->width; x++) {
                    frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
                }
            }
            //uv分量
            for (int y = 0; y < enc_ctx->height; y++) {
                for (int x = 0; x < enc_ctx->width; x++) {
                    //u
                    frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
                    //v
                    frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
                }
            }

            frame->pts = i;
            //10.编码
            ret = encode(enc_ctx, frame, packet, file);
            if (ret == -1) {
                goto _ERROR;
            }
        }
        encode(enc_ctx, nullptr, packet, file);


        _ERROR:
        if (enc_ctx) {
            avcodec_free_context(&enc_ctx);
        }
        if (frame) {
            av_frame_free(&frame);
        }
        if (packet) {
            av_packet_free(&packet);
        }
        if (file) {
            fclose(file);
        }
    }

    void player::encode_audio(char *codecName, char *dstFile) {

        int ret;
        float t, tincr;
        uint8_t *samples = nullptr;
        FILE *file = nullptr;
        AVPacket *packet = nullptr;
        AVFrame *frame = nullptr;
        const AVCodec *codec = nullptr;
        AVCodecContext *enc_ctx;
        //1.输入参数
        av_log_set_level(AV_LOG_DEBUG);
        //2.查找编码器
        codec = avcodec_find_encoder_by_name(codecName);
        if (!codec) {
            printf("codec not found \n");
            exit(-1);
        }
        enc_ctx = avcodec_alloc_context3(codec);
        if (!enc_ctx) {
            printf("avcodec_alloc_context3 failed \n");
            exit(-1);
        }

        //音频码率：16k 32k 64k 128k
        enc_ctx->bit_rate = 64000;
        //采样大小
        enc_ctx->sample_fmt = AV_SAMPLE_FMT_S16;
        ret = check_sample_fmt(codec, enc_ctx->sample_fmt);
        if (ret == 0) {
            av_log(enc_ctx, AV_LOG_ERROR, "encoder dose not support sample %s \n", av_err2str(ret));
            goto _ERROR;
        }
        /* select other audio parameters supported by the encoder */
        enc_ctx->sample_rate = select_best_sample_rate(codec);
        enc_ctx->channel_layout = select_channel_layout(codec);
        enc_ctx->channels = av_get_channel_layout_nb_channels(enc_ctx->channel_layout);

        //5.编码器与编码器上下文绑定到一起
        ret = avcodec_open2(enc_ctx, codec, nullptr);
        if (ret < 0) {
            av_log(enc_ctx, AV_LOG_ERROR, "dont open codec %s \n", av_err2str(ret));
            goto _ERROR;
        }
        //6.创建输出文件
        file = fopen(dstFile, "wb");
        if (!file) {
            av_log(nullptr, AV_LOG_ERROR, "dont open dst file %s \n", av_err2str(ret));
            goto _ERROR;
        }
        //7.创建AVFrame
        frame = av_frame_alloc();
        if (!frame) {
            av_log(nullptr, AV_LOG_ERROR, " no memory  \n");
            goto _ERROR;
        }

        //采样个数
        frame->nb_samples = enc_ctx->frame_size;
        //格式
        frame->format = enc_ctx->sample_fmt;
        //通道类型
        frame->channel_layout = enc_ctx->channel_layout;

        ret = av_frame_get_buffer(frame, 0);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, " could not allocate the video frame  \n");
            goto _ERROR;
        }
        //8.创建avpacket
        packet = av_packet_alloc();
        if (!packet) {
            av_log(nullptr, AV_LOG_ERROR, " no memory  \n");
            goto _ERROR;
        }

        //9.生成音频内容
        /* encode a single tone sound */
        t = 0;
        tincr = 2 * M_PI * 440.0 / enc_ctx->sample_rate;
        for (int i = 0; i < 200; i++) {
            /* make sure the frame is writable -- makes a copy if the encoder
             * kept a reference internally */
            ret = av_frame_make_writable(frame);
            if (ret < 0) {
                av_log(nullptr, AV_LOG_ERROR, " could not allocate the buffer  \n");
                goto _ERROR;
            }
            samples = frame->data[0];
            for (int j = 0; j < enc_ctx->frame_size; j++) {
                samples[2 * j] = (int) (sin(t) * 10000);
                for (int k = 1; k < enc_ctx->channels; k++)
                    samples[2 * j + k] = samples[2 * j];
                t += tincr;
            }
            ret = encode(enc_ctx, frame, packet, file);
            if (ret == -1) {
                goto _ERROR;
            }
        }
        encode(enc_ctx, nullptr, packet, file);

        _ERROR:
        if (enc_ctx) {
            avcodec_free_context(&enc_ctx);
        }
        if (frame) {
            av_frame_free(&frame);
        }
        if (packet) {
            av_packet_free(&packet);
        }
        if (file) {
            fclose(file);
        }
    }

    void player::gen_pic(char *srcAudioFile, char *dstAudioFile) {
        AVFormatContext *pFmtCtx = nullptr;
        AVFormatContext *oFmtCtx = nullptr;
        const AVOutputFormat *outFmt;
        AVStream *outStream;
        AVStream *inStream;
        AVPacket pkt;
        // 1.处理一些参数（日志级别）
        int ret;
        int idx;
        av_log_set_level(AV_LOG_DEBUG);
        // 2.打开多媒体文件
//        AVInputFormat: 如果不指定音频格式，那么它会按照src音频文件的后缀名称进行音频解析，如果指定可以使用如下参数：
//        在FFmpeg的AVInputFormat结构中，音频格式的常量枚举值有很多种。以下是一些常见的音频格式常量：
//        AV_INPUT_FORMAT_NAME_AUTO：自动选择正确的输入格式。
//        AV_INPUT_FORMAT_NAME_MPEG：MPEG音频格式。
//        AV_INPUT_FORMAT_NAME_MP3：MP3音频格式。
//        AV_INPUT_FORMAT_NAME_WAV：WAV音频格式。
//        AV_INPUT_FORMAT_NAME_PCM：PCM音频格式。
//        AV_INPUT_FORMAT_NAME_AAC：AAC音频格式。
//        AV_INPUT_FORMAT_NAME_AC3：AC-3音频格式。
//        AV_INPUT_FORMAT_NAME_DTS：DTS音频格式。
//        AV_INPUT_FORMAT_NAME_VORBIS：Vorbis音频格式。
//        AV_INPUT_FORMAT_NAME_FLAC：FLAC音频格式。
//        AV_INPUT_FORMAT_NAME_ADPCM_EA：ADPCM EA音频格式。
//        AV_INPUT_FORMAT_NAME_ADPCM_IMA_EA：ADPCM IMA EA音频格式。
//        AV_INPUT_FORMAT_NAME_ADPCM_MS：ADPCM MS音频格式。
//        AV_INPUT_FORMAT_NAME_ALAC：ALAC音频格式。
//        AV_INPUT_FORMAT_NAME_AMR_NB：AMR-NB音频格式。
//        AV_INPUT_FORMAT_NAME_AMR_WB：AMR-WB音频格式。
//        AV_INPUT_FORMAT_NAME_FLV：FLV音频格式。
//        AV_INPUT_FORMAT_NAME_G722：G.722音频格式。
//        AV_INPUT_FORMAT_NAME_G723_1：G.723.1音频格式。
//        AV_INPUT_FORMAT_NAME_G726：G.726音频格式。
//        AVInputFormat结构体中还有很多其他的音频格式常量，
//        这里只列举了一些常见的例子。你可以查阅FFmpeg的文档或源代码以获取更全面的列表。
        std::cout << "打印输入流信息" << std::endl;
        ret = avformat_open_input(&pFmtCtx, srcAudioFile, nullptr, nullptr);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "%s \n", av_err2str(ret));
            exit(-1);
        }
        //首先找到输入文件流,打印输入文件信息（音频、视频）
        std::cout << "打印输入媒体信息" << std::endl;
        media_info(srcAudioFile);
        // 3.从多媒体文件中找到视频
        // wanted_stream_nb:想查找的流id，如果是-1那么返回找到的第一个音频流并发索引序号返回
        idx = av_find_best_stream(pFmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
        if (idx < 0) {
            av_log(pFmtCtx, AV_LOG_ERROR, "does not include audio stream  \n");
            goto _ERROR;
        }

        // 4.从多媒体文件汇总读取到音频数据到目的文件
        while (av_read_frame(pFmtCtx, &pkt) >= 0) {
            //读取到的流的index 是我们想要读取的idx
            if (pkt.stream_index == idx) {
                //将读取到的音频包进行设置,然后写入目标输出文件的上下文中

            }
        }

        // 5.将申请的资源释放掉

        _ERROR:
        if (pFmtCtx) {
            avformat_close_input(&pFmtCtx);
            pFmtCtx = nullptr;
        }
        printf(" error finish");
        exit(-1);
    }
}