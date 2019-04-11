cmake_minimum_required(VERSION 3.13)

project(untitled)

set(CMAKE_CXX_STANDARD 14)

include_directories(../untitled)

link_directories(/usr/local/Cellar/ffmpeg/4.1.2/lib/)
link_directories(/usr/local/Cellar/sdl2/2.0.9_1/lib/)

set(SOURCE_FILES main.cpp)
set(SOURCE_FILES simplest_ffmpeg_player.cpp)
set(SOURCE_FILES demo2.cpp)
set(SOURCE_FILES demo.cpp)
set(SOURCE_FILES decoderDemo.cpp)
set(SOURCE_FILES yuvplay.cpp)
set(SOURCE_FILES audioPlay.cpp)

add_executable(untitled  audioPlay.cpp)

target_link_libraries( # Specifies the target library.
        untitled
        avcodec
        avdevice
        avfilter
        avformat
        avresample
        avutil
        postproc
        swresample
        swscale
        SDL2)