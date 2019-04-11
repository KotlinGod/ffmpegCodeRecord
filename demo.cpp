
#include<iostream>
#include "valarray"

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"

}

using namespace std;

char *protocolInfo() {
    char info[40000];
    for (int i = 0; i < 1000; ++i) {
        info[i] = 89;
    }
    for (int i = 0; i < 1000; ++i) {
        cout << info[i];
    }
    av_register_all();
    void *pp = nullptr;
    void **p_temp = &pp;
    const char *str = avio_enum_protocols(p_temp, 0);
    while ((*p_temp) != nullptr) {
        sprintf(info, "%s[protocolIn ][%10s]\n", info, str);
        str = avio_enum_protocols(p_temp, 0);
    }

    sprintf(info, "%s======================================================\n", info, str);
    str = avio_enum_protocols(p_temp, 1);
    while ((*p_temp) != nullptr) {
        sprintf(info, "%s[protocolOut][%10s]\n", info, str);
        str = avio_enum_protocols(p_temp, 1);
    }
    return info;
}


char *avformatInfo() {
    char info[40000]{0};
    av_register_all();

    AVInputFormat *informat = av_iformat_next(nullptr);
    AVOutputFormat *outFormat = av_oformat_next(nullptr);
    while (informat != nullptr) {
        sprintf(info, "%s[formatIn] %10s\n", info, informat->name);
        informat = informat->next;
    }
    while (outFormat != nullptr) {
        sprintf(info, "%s[formatOut] %10s\n", info, outFormat->name);
        outFormat = outFormat->next;
    }
    return info;
}

char *avcodecInfo() {
    char info[40000]{0};
    av_register_all();
    AVCodec *avCodec = av_codec_next(nullptr);
    while (avCodec != nullptr) {
        if (avCodec->decode != nullptr) {
            sprintf(info, "%s[Dec]", info);
        } else {
            sprintf(info, "%s[Enc]", info);
        }
        switch (avCodec->type) {
            case AVMEDIA_TYPE_AUDIO:
                sprintf(info, "%s[Audio]", info);
                break;
            case AVMEDIA_TYPE_VIDEO:
                sprintf(info, "%s[Video]", info);
                break;
            default:
                sprintf(info, "%s[Other]", info);
                break;
        }
        sprintf(info, "%s %10s\n", info, avCodec->name);
        avCodec = avCodec->next;
    }
    return info;
}


char *configurationinfo() {
    char info[40000]{0};
    av_register_all();
    sprintf(info, "%s\n", avcodec_configuration());
    return info;
}

int main() {
    char *info;
    info = protocolInfo();
    cout << info << endl;
    cout << "+++++++++++++++++++++++++++++++" << endl;
    info = avformatInfo();
    cout << info << endl;
    cout << "+++++++++++++++++++++++++++++++" << endl;
    info = avcodecInfo();
    cout << info << endl;
    cout << "+++++++++++++++++++++++++++++++" << endl;
    info = configurationinfo();
    cout << info << endl;
}
