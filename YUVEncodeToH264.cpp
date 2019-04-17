#include <iostream>

extern "C" {
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}


int flushEncoder(AVFormatContext *avFormatContext, unsigned int streamIndex) {
    int ret;
    int got_frame;
    AVPacket *avPacket;
    avPacket = av_packet_alloc();
    if (!(avFormatContext->streams[streamIndex]->codec->codec->capabilities & AV_CODEC_CAP_DELAY))
        return 0;
    while (true) {
        avPacket->data = nullptr;
        avPacket->size = 0;
        av_init_packet(avPacket);
        ret = avcodec_encode_video2(avFormatContext->streams[streamIndex]->codec, avPacket, nullptr, &got_frame);
        av_frame_free(nullptr);
        if (ret < 0)
            break;
        if (!got_frame) {
            ret = 0;
            break;
        }
        printf("Flush Encoder: Succeed to encode 1 frame!\tsize:%5d\n", avPacket->size);
        /* mux encoded frame */
        ret = av_write_frame(avFormatContext, avPacket);
        if (ret < 0)
            break;
    }
    return ret;
}

int main() {
    AVFormatContext *pFormatCtx = nullptr;
    AVOutputFormat *fmt = nullptr;
    AVStream *videoStream = nullptr;
    AVCodecContext *pCodecCtx = nullptr;
    AVCodec *pCodec = nullptr;
    AVPacket *avPacket = nullptr;
//    uint8_t *outBuffer = nullptr;
    AVFrame *pFrame = nullptr;
    int framecnt = 0;
    //FILE *in_file = fopen("src01_480x272.yuv", "rb");	//Input raw YUV data
    FILE *in_file = fopen("test2.yuv", "rb");   //Input raw YUV data
    int in_w = 1920, in_h = 1080;                              //Input data's width and height
    //Output Filepath 输出文件路径
//    const char* out_file = "src01.ts";
//    const char* out_file = "src01.hevc";
    const char *out_file = "ds.h265";
//    const char *out_file = "ds.h264";

    av_register_all();
    //Method1.
    pFormatCtx = avformat_alloc_context();
    //Guess Format 根据文件名猜出格式
    fmt = av_guess_format(nullptr, out_file, nullptr);
    pFormatCtx->oformat = fmt;
    //Method 2. 方法二更加自动化一些
//    avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, out_file);
//    fmt = pFormatCtx->oformat;


    //Open output URL
    if (avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0) {
        printf("Failed to open output file! 输出文件打开失败\n");
        return -1;
    }
    //创建输出码流->创建了一块内存空间->并不知道他是什么类型流->希望他是视频流
    videoStream = avformat_new_stream(pFormatCtx, nullptr);
    if (videoStream == nullptr) {
        return -1;
    }
    //1、获取编码器上下文
    pCodecCtx = avcodec_alloc_context3(nullptr);
    AVCodecParameters *avCodecParameters = videoStream->codecpar;
    avcodec_parameters_to_context(pCodecCtx, avCodecParameters);
    //2、设置编解码器上下文参数->必需设置->不可少
    //目标：设置为是一个视频编码器上下文->指定的是视频编码器
    //上下文种类：视频解码器、视频编码器、音频解码器、音频编码器
    //2.1 设置视频编码器ID
    //pCodecCtx->codec_id =AV_CODEC_ID_HEVC;
    pCodecCtx->codec_id = fmt->video_codec;
    //2.2 设置编码器类型->视频编码器
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    //2.3 设置读取像素数据格式->编码的是像素数据格式->视频像素数据格式->YUV420P(YUV422P、YUV444P等等...)
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    //2.4 设置视频宽高->视频尺寸
    pCodecCtx->width = in_w;
    pCodecCtx->height = in_h;
    //2.5 设置帧率->表示每秒30帧
    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = 30;
    //2.6 设置码率
    pCodecCtx->bit_rate = avCodecParameters->bit_rate;
    //2.7 设置GOP->影响到视频质量问题->画面组->一组连续画面
    //MPEG格式画面类型：3种类型->分为->I帧、P帧、B帧    //I帧->内部编码帧->原始帧(原始视频数据)    //    完整画面->关键帧(必需的有，如果没有I，那么你无法进行编码，解码)    //    视频第1帧->视频序列中的第一个帧始终都是I帧，因为它是关键帧    //P帧->向前预测帧->预测前面的一帧类型，处理数据(前面->I帧、B帧)   
    //     P帧数据->根据前面的一帧数据->进行处理->得到了P帧   
    // B帧->前后预测帧(双向预测帧)->前面一帧和后面一帧   
    //     B帧压缩率高，但是对解码性能要求较高。   
    // 总结：I只需要考虑自己 = 1帧，P帧考虑自己+前面一帧 = 2帧，B帧考虑自己+前后帧 = 3帧   
    //     说白了->P帧和B帧是对I帧压缩   
    // 每250帧，插入1个I帧，I帧越少，视频越小->默认值->视频不一样
    pCodecCtx->gop_size = 250;
    //2.8 设置量化参数->数学算法(高级算法)->不讲解了
    //总结：量化系数越小，视频越是清晰
    //一般情况下都是默认值，最小量化系数默认值是10，最大量化系数默认值是51
    pCodecCtx->qmin = 10;
    pCodecCtx->qmax = 51;
    //2.9 设置b帧最大值->设置m每组三个B帧
    pCodecCtx->max_b_frames = 3;

    //H264
//    pCodecCtx->me_range = 16;
//    pCodecCtx->max_qdiff = 4;
    //pCodecCtx->qcompress = 0.6;


    //编码选项->编码设置
    AVDictionary *param = nullptr;
    //H.264
    if (pCodecCtx->codec_id == AV_CODEC_ID_H264) {
        //第一个值：预备参数
        //key: preset
        //value: slow->慢
        //value: superfast->超快
        av_dict_set(&param, "preset", "slow", 0);
        //第二个值：调优
        //key: tune->调优
        //value: zerolatency->零延迟
        av_dict_set(&param, "tune", "zerolatency", 0);
        //av_dict_set(&param, "profile", "main", 0);
    }
    //H.265
    if (pCodecCtx->codec_id == AV_CODEC_ID_H265) {
        av_dict_set(&param, "x265-params", "qp=20", 0);
        av_dict_set(&param, "preset", "ultrafast", 0);
        av_dict_set(&param, "tune", "zero-latency", 0);
    }

    //Show some Information
    av_dump_format(pFormatCtx, 0, out_file, 1);
    pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
    if (!pCodec) {
        printf("Can not find encoder! 没有找到合适的编码器！\n");
        return -1;
    }
    if (avcodec_open2(pCodecCtx, pCodec, &param) < 0) {
        printf("Failed to open encoder! 编码器打开失败！\n");
        return -1;
    }

    pFrame = av_frame_alloc();
    pFrame->format = pCodecCtx->pix_fmt;
    pFrame->width = pCodecCtx->width;
    pFrame->height = pCodecCtx->height;


    //通过这个方法就可以完成（a)计算所需内存大小av_image_get_bufferz_size() -->
    // (b) 按计算的内存大小申请所需内存 av_malloc()  -->
    // (c) 对申请的内存进行格式化 av_image_fill_arrays();
    //这几个步骤了，不用再创建一个buffer控件，data的内存就已经分配好了。
    int ret = av_image_alloc(pFrame->data, pFrame->linesize, pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, 1);
    if (ret < 0) {
        printf("Could not allocate raw picture buffer\n");
        return -1;
    }

    //之前是这样写的
//    int bufferSize = av_image_get_buffer_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, 1);
//    outBuffer = (uint8_t *) av_malloc(bufferSize);
//    av_image_fill_arrays(pFrame->data, pFrame->linesize, outBuffer, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, 1);

    //写入文件头信息
    avformat_write_header(pFormatCtx, nullptr);
    int y_size = pCodecCtx->width * pCodecCtx->height;

    int i = 0;
    while (true) {
        //Read raw YUV data
        if (fread(pFrame->data[0], 1, y_size, in_file) <= 0 ||        // Y
            fread(pFrame->data[1], 1, y_size / 4, in_file) <= 0 ||    // U
            fread(pFrame->data[2], 1, y_size / 4, in_file) <= 0) {    // V
            return -1;
        } else if (feof(in_file)) {
            break;
        }
        //PTS
        pFrame->pts = i;
        ++i;
//        pFrame->pts = i * (videoStream->time_base.den) / ((videoStream->time_base.num) * 25);

//        int got_picture = 0;
//        //Encode
//        int ret = avcodec_encode_video2(pCodecCtx, avPacket, pFrame, &got_picture);
//        if (ret < 0) {
//            printf("Failed to encode! \n");
//            return -1;
//        }
//        if (got_picture == 1) {
//            printf("Succeed to encode frame: %5d\tsize:%5d\n", framecnt, avPacket->size);
//            framecnt++;
//            avPacket->stream_index = videoStream->index;
//            ret = av_write_frame(pFormatCtx, avPacket);
//            av_packet_free(&avPacket);
//        }

        avPacket = av_packet_alloc();
        av_init_packet(avPacket);
        //第9步：视频编码处理
        //9.1 发送一帧视频像素数据
        avcodec_send_frame(pCodecCtx, pFrame);
        //9.2 接收一帧视频像素数据->编码为->视频压缩数据格式
        int ret = avcodec_receive_packet(pCodecCtx, avPacket);
        //9.3 判定是否编码成功
        if (ret == 0) {
            //编码成功         
            // 第10步：将视频压缩数据->写入到输出文件中->outFilePath           
            printf("Succeed to encode frame: %5d\tsize:%5d\n", framecnt, avPacket->size);
            framecnt++;
            avPacket->stream_index = videoStream->index;
            ret = av_write_frame(pFormatCtx, avPacket);
        }
        av_packet_unref(avPacket);
    }
    //Flush Encoder
     ret = flushEncoder(pFormatCtx, 0);
    if (ret < 0) {
        printf("Flushing encoder failed\n");
        return -1;
    }

    //Write file trailer 写文件尾
    av_write_trailer(pFormatCtx);

    //Clean
    avcodec_close(pCodecCtx);
    av_free(pFrame);
//    av_free(outBuffer);
    avio_close(pFormatCtx->pb);
    avformat_free_context(pFormatCtx);
    fclose(in_file);
    return 0;
}