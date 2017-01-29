#include <stdio.h>
#ifdef _WIN32
    #include <SDL.h>
#else
    #include <SDL2/SDL.h>
#endif
#include "chip8.h"

#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 64
#define true 1
#define false 0

SDL_Window *window = NULL;
SDL_Surface *screenSurface = NULL;
SDL_Renderer *renderer = NULL;
int modifier = 10;
int display_height;
int display_width;

void initGraphics();

int main(int arg, char **args){
    int quit = false;

    CHIP8_Init();
    initGraphics();
    SDL_Event e;

    while(!quit){
        CHIP8_EmulateCycle();
        while(SDL_PollEvent(&e) != 0)
            if(e.type == SDL_QUIT)
                quit = true;
    }

    return 0;
}

void initGraphics(){
    display_height = SCREEN_HEIGHT * modifier;
    display_width = SCREEN_WIDTH * modifier;
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        printf("Error initializing SDL: %s\n", SDL_GetError());
    else{
        window = SDL_CreateWindow("YAC8E", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, display_width, display_height, SDL_WINDOW_SHOWN);
        if(window == NULL)
            printf("Window could not be created: %s\n", SDL_GetError());
        else{
            renderer = SDL_CreateRenderer(window, -1, 0);
            if(renderer == NULL)
                printf("Renderer could not be created: %s\n", SDL_GetError());
        }

    }
}
