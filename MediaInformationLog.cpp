//
// Created by 李兵 on 2019-04-09.
//
/**
 * 打印视频相关信息
 */
#include "iostream"

extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/dict.h"
#include "libavcodec/avcodec.h"
}
using namespace std;

int main() {
    AVFormatContext *avFormatContext = nullptr;
    AVCodecContext *avCodecContext = nullptr;
    AVCodec *avCodec = nullptr;
    AVDictionaryEntry *dict = nullptr;

    int videoIndex = -1, audioIndex = -1;
    int iHour, iMinute, iSecond, iTotalSeconds;//HH:MM:SS
    const char *fileName = "test.mp4";

    av_register_all();
    if (avformat_open_input(&avFormatContext, fileName, nullptr, nullptr) != 0) {
        cout << "Couldn't open input stream" << endl;
        exit(-1);
    }
    if (avformat_find_stream_info(avFormatContext, nullptr) < 0) {
        cout << "Couldn't open stream information" << endl;
    }

    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;
        }
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioIndex = i;
        }
    }
    if (videoIndex == -1) {
        cout << "Couldn't find a video stream" << endl;
        exit(-1);
    }
//    avCodecContext = avFormatContext->streams[videoIndex]->codec;
    avCodecContext = avcodec_alloc_context3(nullptr);
    if (avCodecContext == nullptr) {
        cout << "Could not allocate AVCodecContext" << endl;
    }
    avcodec_parameters_to_context(avCodecContext, avFormatContext->streams[videoIndex]->codecpar);

    avCodec = avcodec_find_decoder(avCodecContext->codec_id);
    if (avCodec == nullptr) {
        cout << "Codec couldn't find" << endl;
        exit(-1);
    }

    if (avcodec_open2(avCodecContext, avCodec, nullptr) < 0) {
        cout << "Codec couldn't open" << endl;
        exit(-1);
    }

    if (audioIndex == -1) {
        printf("Couldn't find a audio stream.\n");
        exit(-1);
    }

    puts("AVFormatContext信息：");
    puts("---------------------------------------------");
    printf("文件名：%s\n", avFormatContext->url);
    iTotalSeconds = (int) avFormatContext->duration/*微秒*/ / 1000000;
    iHour = iTotalSeconds / 3600;//小时
    iMinute = iTotalSeconds % 3600 / 60;//分钟
    iSecond = iTotalSeconds % 60;//秒
    printf("持续时间：%02d:%02d:%02d\n", iHour, iMinute, iSecond);

    printf("平均混合码率：%lld kb/s\n", avFormatContext->bit_rate / 1000);
    printf("视音频个数：%d\n", avFormatContext->nb_streams);
    puts("---------------------------------------------");

    puts("AVInputFormat信息:");

    puts("---------------------------------------------");
    printf("封装格式名称：%s\n", avFormatContext->iformat->name);
    printf("封装格式长名称：%s\n", avFormatContext->iformat->long_name);
    printf("封装格式扩展名：%s\n", avFormatContext->iformat->extensions);
    printf("封装格式ID：%d\n", avFormatContext->iformat->raw_codec_id);
    puts("---------------------------------------------");

    puts("AVStream信息:");
    puts("---------------------------------------------");
    printf("视频流标识符：%d\n", avFormatContext->streams[videoIndex]->index);
    printf("音频流标识符：%d\n", avFormatContext->streams[audioIndex]->index);
    printf("视频流长度：%lld微秒\n", avFormatContext->streams[videoIndex]->duration);
    printf("音频流长度：%lld微秒\n", avFormatContext->streams[audioIndex]->duration);
    printf("音频采样率：%d\n", avFormatContext->streams[audioIndex]->codecpar->sample_rate);
    printf("音频信道数目：%d\n", avFormatContext->streams[audioIndex]->codecpar->channels);
    puts("---------------------------------------------");

    puts("AVCodecContext信息:");
    puts("---------------------------------------------");
    printf("视频码率：%lld kb/s\n", avCodecContext->bit_rate / 1000);
    printf("视频大小：%d * %d\n", avCodecContext->width, avCodecContext->height);
    puts("---------------------------------------------");

    puts("AVCodec信息:");
    puts("---------------------------------------------");
    printf("视频编码格式：%s\n", avCodec->name);
    printf("视频编码详细格式：%s\n", avCodec->long_name);
    puts("---------------------------------------------");

    puts("AVFormatContext元数据：");
    puts("---------------------------------------------");
    while (dict = av_dict_get(avFormatContext->metadata, "", dict, AV_DICT_IGNORE_SUFFIX)) {
        printf("[%s] = %s\n", dict->key, dict->value);
    }
    puts("---------------------------------------------");

    puts("AVStream视频元数据：");
    puts("---------------------------------------------");
    dict = nullptr;
    while (dict = av_dict_get(avFormatContext->streams[videoIndex]->metadata, "", dict, AV_DICT_IGNORE_SUFFIX)) {
        printf("[%s] = %s\n", dict->key, dict->value);
    }
    puts("---------------------------------------------");

    puts("AVStream音频元数据：");
    puts("---------------------------------------------");
    dict = nullptr;
    while (dict = av_dict_get(avFormatContext->streams[audioIndex]->metadata, "", dict, AV_DICT_IGNORE_SUFFIX)) {
        printf("[%s] = %s\n", dict->key, dict->value);
    }
    puts("---------------------------------------------");
    av_dump_format(avFormatContext, -1, fileName, 0);
    printf("\n\n编译信息：\n%s\n\n", avcodec_configuration());


    avcodec_close(avCodecContext);
    avformat_close_input(&avFormatContext);
}