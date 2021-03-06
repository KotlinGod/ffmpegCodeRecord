
/**
 * 最简单的基于FFmpeg的视音频分离器（简化版）
 * Simplest FFmpeg Demuxer Simple
 *
 *
 * 本程序可以将封装格式中的视频码流数据和音频码流数据分离出来。
 * 在该例子中， 将FLV的文件分离得到H.264视频码流文件和MP3
 * 音频码流文件。
 *
 * 注意：
 * 这个是简化版的视音频分离器。与原版的不同在于，没有初始化输出
 * 视频流和音频流的AVFormatContext。而是直接将解码后的得到的
 * AVPacket中的的数据通过fwrite()写入文件。这样做的好处是流程比
 * 较简单。坏处是对一些格式的视音频码流是不适用的，比如说
 * FLV/MP4/MKV等格式中的AAC码流（上述封装格式中的AAC的AVPacket中
 * 的数据缺失了7字节的ADTS文件头）。
 *
 */

#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
}


#define USE_H264BSF 1

int main() {
    AVFormatContext *ifmt_ctx = NULL;
    AVPacket pkt;
    int ret, i;
    int videoindex = -1, audioindex = -1;
    const char *in_filename = "test.mp4";//Input file URL
    const char *out_filename_v = "cuc_ieschool.h264";//Output file URL
    const char *out_filename_a = "cuc_ieschool.aac";

    av_register_all();
    //Input
    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
        printf("Could not open input file.");
        return -1;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        printf("Failed to retrieve input stream information");
        return -1;
    }

    videoindex = -1;
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
        } else if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioindex = i;
        }
    }
    //Dump Format------------------
    printf("\nInput Video===========================\n");
    av_dump_format(ifmt_ctx, 0, in_filename, 0);
    printf("\n======================================\n");

    FILE *fp_audio = fopen(out_filename_a, "wb+");
    FILE *fp_video = fopen(out_filename_v, "wb+");

    /*
    FIX: H.264 in some container format (FLV, MP4, MKV etc.) need
    "h264_mp4toannexb" bitstream filter (BSF)
      *Add SPS,PPS in front of IDR frame
      *Add start code ("0,0,0,1") in front of NALU
    H.264 in some container (MPEG2TS) don't need this BSF.
    */
#if USE_H264BSF
    AVBitStreamFilterContext *h264bsfc = av_bitstream_filter_init("h264_mp4toannexb");
#endif

    while (av_read_frame(ifmt_ctx, &pkt) >= 0) {
        if (pkt.stream_index == videoindex) {
#if USE_H264BSF
            av_bitstream_filter_filter(h264bsfc, ifmt_ctx->streams[videoindex]->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif
            printf("Write Video Packet. size:%d\tpts:%lld\n", pkt.size, pkt.pts);
            fwrite(pkt.data, 1, pkt.size, fp_video);
        } else if (pkt.stream_index == audioindex) {
            /*
            AAC in some container format (FLV, MP4, MKV etc.) need to add 7 Bytes
            ADTS Header in front of AVPacket data manually.
            Other Audio Codec (MP3...) works well.
            */
            printf("Write Audio Packet. size:%d\tpts:%lld\n", pkt.size, pkt.pts);
            fwrite(pkt.data, 1, pkt.size, fp_audio);
        }
        av_free_packet(&pkt);
    }

#if USE_H264BSF
    av_bitstream_filter_close(h264bsfc);
#endif

    fclose(fp_video);
    fclose(fp_audio);

    avformat_close_input(&ifmt_ctx);

    if (ret < 0 && ret != AVERROR_EOF) {
        printf("Error occurred.\n");
        return -1;
    }
    return 0;
}