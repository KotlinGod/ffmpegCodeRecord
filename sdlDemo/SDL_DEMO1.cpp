//
// Created by 李兵 on 2019-04-17.
//

#include "SDL2/SDL.h"
#include "SDL2/SDL_test_images.h"
#include "iostream"

using namespace std;
int main() {

    SDL_Window *win = nullptr;
    win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (win == nullptr){
        std::cout << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Renderer *ren = nullptr;
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == nullptr){
        std::cout << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Surface *bmp = nullptr;
    bmp = SDL_LoadBMP("demo.bmp");
    if (bmp == nullptr){
        std::cout << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Texture *tex = nullptr;
    tex = SDL_CreateTextureFromSurface(ren, bmp);
    SDL_FreeSurface(bmp);
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, tex, NULL, NULL);
    SDL_RenderPresent(ren);
    SDL_Delay(2000);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;

}