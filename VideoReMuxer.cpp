//
// Created by 李兵 on 2019-04-12.
//视频封装格式转换
//


#include "iostream"

extern "C" {
#include "libavformat/avformat.h"
}
using namespace std;

int main() {
    AVOutputFormat *ofmt = nullptr;
    //输入对应一个AVFormatContext，输出对应一个AVFormatContext
    //（Input AVFormatContext and Output AVFormatContext）
    AVFormatContext *ifmt_ctx = nullptr, *ofmt_ctx = nullptr;
    AVPacket pkt;
    const char *in_filename, *out_filename;
    int ret, i;
    int frame_index = 0;

    in_filename = "test.mp4";//输入文件名（Input file URL）
    out_filename = "fffffffffffff.flv";//输出文件名（Output file URL）

    av_register_all();

    AVBitStreamFilterContext *filter = av_bitstream_filter_init("h264_mp4toannexb");
    if (NULL == filter) {
        cout << "filter init fail" << endl;
    }
    av_register_bitstream_filter(const_cast<AVBitStreamFilter *>(filter->filter));
    //输入（Input）
    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, nullptr, nullptr)) < 0) {
        printf("Could not open input file.");
        goto end;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx, nullptr)) < 0) {
        printf("Failed to retrieve input stream information");
        goto end;
    }
    av_dump_format(ifmt_ctx, 0, in_filename, 0);
    //输出（Output）
    avformat_alloc_output_context2(&ofmt_ctx, nullptr, nullptr, out_filename);
    if (!ofmt_ctx) {
        printf("Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        goto end;

    }
    ofmt = ofmt_ctx->oformat;
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        //根据输入流创建输出流（Create output AVStream according to input AVStream）
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
        if (!out_stream) {
            printf("Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
            goto end;

        }
        //复制AVCodecContext的设置（Copy the settings of AVCodecContext）
        ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
        if (ret < 0) {
            printf("Failed to copy context from input to output stream codec context\n");
            goto end;

        }
        out_stream->codec->codec_tag = 0;
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    }
    //输出一下格式------------------
    av_dump_format(ofmt_ctx, 0, out_filename, 1);
    //打开输出文件（Open output file）
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            printf("Could not open output file '%s'", out_filename);
            goto end;

        }
    }
    //写文件头（Write file header）
    ret = avformat_write_header(ofmt_ctx, nullptr);
    if (ret < 0) {
        printf("Error occurred when opening output file\n");
        goto end;

    }

    while (true) {
        AVStream *in_stream, *out_stream;
        //获取一个AVPacket（Get an AVPacket）
        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0)
            break;
        in_stream = ifmt_ctx->streams[pkt.stream_index];
        out_stream = ofmt_ctx->streams[pkt.stream_index];
        /* copy packet */
        //转换PTS/DTS（Convert PTS/DTS）
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;

        //mp4 mkv flv等格式转换成avi，需要添加以下的信息,如果是avi转mp4等格式就不用加了。不过avi转mp4还需要提供pts。代码里面没有写
        if (in_stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            AVPacket fpkt = pkt;
            int a = av_bitstream_filter_filter(filter,
                                               out_stream->codec, nullptr, &fpkt.data, &fpkt.size,
                                               pkt.data, pkt.size, pkt.flags & AV_PKT_FLAG_KEY);
            pkt.data = fpkt.data;
            pkt.size = fpkt.size;
        }
        //写入（Write）
        ret = av_interleaved_write_frame(ofmt_ctx, &pkt);
        if (ret < 0) {
            av_packet_unref(&pkt);
            printf("Error muxing packet\n");
            continue;
        }
        printf("Write %8d frames to output file\n", frame_index);
        av_packet_unref(&pkt);
        frame_index++;

    }
    //写文件尾（Write file trailer）
    av_write_trailer(ofmt_ctx);
    end:
    avformat_close_input(&ifmt_ctx);
    av_bitstream_filter_close(filter);
    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_close(ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);
    if (ret < 0 && ret != AVERROR_EOF) {
        printf("Error occurred.\n");
        return -1;

    }
    return 0;
}