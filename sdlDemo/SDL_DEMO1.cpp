//
// Created by 李兵 on 2019-04-17.

//
#include <iostream>

using namespace std;
extern "C" {
#include "SDL2/SDL.h"
#include "SDL2/SDL_main.h"


int main() {
//    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *win = nullptr;
    win = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 600, SDL_WINDOW_OPENGL);
    if (win == nullptr) {
        std::cout << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Renderer *ren = nullptr;
    ren = SDL_CreateRenderer(win, -1, 0);
    if (ren == nullptr) {
        std::cout << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Surface *bmp = nullptr;
    bmp = SDL_LoadBMP("demo.bmp");
    if (bmp == nullptr) {
        std::cout << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Texture *tex = nullptr;
//    tex = SDL_CreateTextureFromSurface(ren, bmp);
//    SDL_FreeSurface(bmp);


    tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, 600, 600);


    SDL_Rect rect;
    bool quit = true;
    SDL_Event event;
    do {
        SDL_PollEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                quit = false;
                break;
            default:
//                SDL_log("event type is %d", event.type);
                break;
        }

        rect.w = 30;
        rect.h = 30;
        rect.x = rand() % 600;
        rect.y = rand() % 600;
        SDL_SetRenderTarget(ren, tex);
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
        SDL_RenderClear(ren);
        SDL_RenderDrawRect(ren, &rect);
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 0);
        SDL_RenderFillRect(ren, &rect);

        SDL_SetRenderTarget(ren, nullptr);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);
        SDL_Delay(50);
    } while (quit);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;

}
}


