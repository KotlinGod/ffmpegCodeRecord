
/**
 * 最简单的基于FFmpeg的图像编码器
 * Simplest FFmpeg Picture Encoder
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * 本程序实现了YUV420P像素数据编码为JPEG图片。是最简单的FFmpeg编码方面的教程。
 * 通过学习本例子可以了解FFmpeg的编码流程。
 */

#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}


int main() {
    AVFormatContext *outputFormatContext = nullptr;
    AVStream *videoStream = nullptr;
    AVCodecContext *pCodecCtx = nullptr;
    AVCodec *pCodec = nullptr;

    AVFrame *pictureFrame = nullptr;
    AVPacket avPacket;
    int y_size;
    int got_picture = 0;

    int ret = 0;

    FILE *in_file = nullptr;                            //YUV source
    int imageWidth = 1920, imageHeight = 1080;                           //YUV's width and height

    in_file = fopen("test2.yuv", "rb");

    pCodecCtx = avcodec_alloc_context3(nullptr);
    pCodecCtx->codec_id = AV_CODEC_ID_MJPEG;
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;
    pCodecCtx->width = imageWidth;
    pCodecCtx->height = imageHeight;
    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = 30;


    pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
    if (!pCodec) {
        printf("Codec not found.");
        return -1;
    }
    if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) {
        printf("Could not open codec.");
        return -1;
    }

    pictureFrame = av_frame_alloc();
    pictureFrame->format = pCodecCtx->pix_fmt;
    pictureFrame->width = pCodecCtx->width;
    pictureFrame->height = pCodecCtx->height;


    av_image_alloc(pictureFrame->data, pictureFrame->linesize, pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, 1);

//    size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
//    picture_buf = (uint8_t *) av_malloc(size);
//    if (!picture_buf) {
//        return -1;
//    }

//    avpicture_fill((AVPicture *) pictureFrame, picture_buf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
    //首先读取每一帧的数据,生成图片
    int i = 0;
    while (true) {
        outputFormatContext = avformat_alloc_context();
        char out_file[1024];    //Output file
        sprintf(out_file, "demo/cuc_view_encode%d.jpg", i);

        //    //Guess format
//    outputFormat = av_guess_format("mjpeg", nullptr, nullptr);
//    outputFormatContext->oformat = outputFormat;
//    if (avio_open(&outputFormatContext->pb, out_file, AVIO_FLAG_READ_WRITE) < 0) {
//        printf("Couldn't open output file.");
//        return -1;
//    }

//      Method 2. More simple
        avformat_alloc_output_context2(&outputFormatContext, nullptr, nullptr, out_file);
        videoStream = avformat_new_stream(outputFormatContext, 0);
        if (videoStream == nullptr) {
            return -1;
        }
        //Write Header
        avformat_write_header(outputFormatContext, nullptr);
        y_size = pCodecCtx->width * pCodecCtx->height;
        av_new_packet(&avPacket, y_size * 3);
        //Read YUV
        if (fread(pictureFrame->data[0], 1, y_size, in_file) <= 0 ||        // Y
            fread(pictureFrame->data[1], 1, y_size / 4, in_file) <= 0 ||    // U
            fread(pictureFrame->data[2], 1, y_size / 4, in_file) <= 0) {    // V
            return -1;
        } else if (feof(in_file)) {
            break;
        }

        //Encode
        ret = avcodec_encode_video2(pCodecCtx, &avPacket, pictureFrame, &got_picture);
        if (ret < 0) {
            printf("Encode Error.\n");
            return -1;
        }
        if (got_picture == 1) {
            avPacket.stream_index = videoStream->index;
            ret = av_write_frame(outputFormatContext, &avPacket);
        }

        av_packet_unref(&avPacket);
        //Write Trailer
        av_write_trailer(outputFormatContext);

        printf("Encode Successful.\n");
        if (!got_picture) {
            continue;
        }
        ++i;
    }

    if (videoStream) {
        avcodec_close(videoStream->codec);
        av_free(pictureFrame);
    }
    avio_close(outputFormatContext->pb);
    avformat_free_context(outputFormatContext);
    fclose(in_file);
    return 0;
}