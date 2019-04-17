cmake_minimum_required(VERSION 3.13)

project(untitled)

set(CMAKE_CXX_STANDARD 14)

include_directories(../untitled)

link_directories(/usr/local/Cellar/ffmpeg/4.1.2/lib/)
link_directories(/usr/local/Cellar/sdl2/2.0.9_1/lib/)

#set(SOURCE_FILES SaveFrameToImage.cpp)
#set(SOURCE_FILES PlayVideoBySDL2.cpp)
#set(SOURCE_FILES MediaInformationLog.cpp)
#set(SOURCE_FILES FFMpegInformationLog.cpp)
#set(SOURCE_FILES VideoDecodeToGetYUV.cpp)
#set(SOURCE_FILES PlayYUVBySDL2.cpp)
#set(SOURCE_FILES PlayAudioBySDL2.cpp)

add_executable(untitled sdlDemo/SDL_DEMO1.cpp)

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

