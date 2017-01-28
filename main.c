#include <stdio.h>
#include <SDL2/SDL.h>
#include "chip8.h"

const int SCREEN_HEIGHT = 64;
const int SCREEN_WIDTH = 32;

int main(int arg, char **args){
    SDL_Window *window = NULL;
    SDL_Surface *screenSurface = NULL;
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        printf("Error initializing SDL: %s\n", SDL_GetError());
    else{

    
    }
    init();

    return 0;
}
