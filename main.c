#include <stdio.h>
#ifdef _WIN32
    #include <SDL.h>
#elif _unix
    #include <SDL/SDL2.h>
#endif
#include "chip8.h"

const int SCREEN_HEIGHT = 64;
const int SCREEN_WIDTH = 32;

SDL_Window *window = NULL;
SDL_Surface *screenSurface = NULL;

void initGraphics();

int main(int arg, char **args){
    initGraphics();

    CHIP8_Init();

    return 0;
}

void initGraphics(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        printf("Error initializing SDL: %s\n", SDL_GetError());
    else{
        window = SDL_CreateWindow("YAC8E", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(window == NULL)
            printf("Window could not be created: %s\n", SDL_GetError());
        else{
            screenSurface = SDL_GetWindowSurface(window);
            SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
            SDL_UpdateWindowSurface(window);
            SDL_Delay(2000);
        }

    }
}
