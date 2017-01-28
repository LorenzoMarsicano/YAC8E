#include <stdio.h>
#include <SDL2.h>
#include "chip8.h"

const int SCREEN_HEIGHT = 64;
const int SCREEN_WIDTH = 32;

SDL_Window *window = NULL;
SDL_Surface *screenSurface = NULL;

void initGraphics();

int main(int arg, char **args){

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

    }
}
