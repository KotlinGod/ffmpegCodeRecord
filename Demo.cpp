
extern "C"{
#include "libavcodec/avcodec.h"
#include <libavformat/avformat.h>

}


//
// Created by 李兵 on 2019-04-16.
//
int main(){
    av_frame_alloc();
    av_frame_free();
    avcodec_alloc_context3();
    avcodec_free_context();
    avcodec_find_decoder();/avcodec_find_decoder_by_name();
    avcodec_open2();
    av_read_frame()
    avcodec_decode_video2();









    avcodec_find_encoder();/avcodec_find_encoder_by_name();
    avcodec_alloc_context3();
    avcodec_open2();
    avcodec_encode_video2();

}
