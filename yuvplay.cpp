//
// Created by 李兵 on 2019-04-10.
//


#include <iostream>

extern "C" {
#include "SDL2/SDL.h"
}

///bpp 指的是 byte per point，也就是一个像素点几个bit。
/// 由于使用的是 YUV 格式，其中每个像素都有一个 Y 值，而 U 和 V 的数量均是 Y 的 1/4。Y、U、V 各是一个字节，
/// 一个字节是 8 bit，结果就是 1 + 1/4 + 1/4 = 1.5 个字节 = 1.5 * 8 = 12 bit
const int bpp = 12;

//如果播放器的宽高比和视频的宽高比不一致视频会进行拉伸
int screen_w = 960, screen_h = 540;
const int pixel_w = 960, pixel_h = 540;

unsigned char buffer[pixel_w * pixel_h * bpp / 8];


//Refresh Event
#define REFRESH_EVENT  (SDL_USEREVENT + 1)

#define BREAK_EVENT  (SDL_USEREVENT + 2)

int thread_exit = 0;

int refresh_video(void *opaque) {
    thread_exit = 0;
    while (!thread_exit) {
        SDL_Event event;
        event.type = REFRESH_EVENT;
        SDL_PushEvent(&event);
        SDL_Delay(40);
    }
    thread_exit = 0;
    //Break
    SDL_Event event;
    event.type = BREAK_EVENT;
    SDL_PushEvent(&event);

    return 0;
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        printf("Could not initialize SDL - %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window *screen;
    //SDL 2.0 Support for multiple windows
    screen = SDL_CreateWindow("Simplest Video Play SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_w, screen_h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!screen) {
        printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
        return -1;
    }
    SDL_Renderer *sdlRenderer = SDL_CreateRenderer(screen, -1, 0);

    Uint32 pixformat = 0;

    //IYUV: Y + U + V  (3 planes)
    //YV12: Y + V + U  (3 planes)
    pixformat = SDL_PIXELFORMAT_IYUV;

    SDL_Texture *sdlTexture = SDL_CreateTexture(sdlRenderer, pixformat, SDL_TEXTUREACCESS_STREAMING, pixel_w, pixel_h);

    FILE *fp = nullptr;
    fp = fopen("output.yuv", "rb+");

    if (fp == nullptr) {
        printf("cannot open this file\n");
        return -1;
    }

    SDL_Rect sdlRect;

    SDL_Thread *refresh_thread = SDL_CreateThread(refresh_video, nullptr, nullptr);
    SDL_Event event;
    while (true) {
        //Wait
        SDL_WaitEvent(&event);
        if (event.type == REFRESH_EVENT) {
            if (fread(buffer, 1, pixel_w * pixel_h * bpp / 8, fp) != pixel_w * pixel_h * bpp / 8) {
                // Loop
                fseek(fp, 0, SEEK_SET);
                fread(buffer, 1, pixel_w * pixel_h * bpp / 8, fp);
            }
            SDL_UpdateTexture(sdlTexture, nullptr, buffer, pixel_w);

            //FIX: If window is resize
            sdlRect.x = 0;
            sdlRect.y = 0;
            sdlRect.w = screen_w;
            sdlRect.h = screen_h;

            SDL_RenderClear(sdlRenderer);
            SDL_RenderCopy(sdlRenderer, sdlTexture, nullptr, &sdlRect);
            SDL_RenderPresent(sdlRenderer);

        } else if (event.type == SDL_WINDOWEVENT) {
            //If Resize
            SDL_GetWindowSize(screen, &screen_w, &screen_h);
        } else if (event.type == SDL_QUIT) {
            thread_exit = 1;
        } else if (event.type == BREAK_EVENT) {
            break;
        }
    }
    SDL_Quit();
    return 0;
}