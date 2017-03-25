#include <stdio.h>
#ifdef _WIN32
    #include <SDL.h>
#else
    #include <SDL2/SDL.h>
#endif
#include "chip8.h"

#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 64

SDL_Window *window = NULL;
SDL_Surface *screenSurface = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture * texture = NULL;
SDL_Event e;

int modifier = 1;
int display_height;
int display_width;
int quit;

void initGraphics();
void drawGraphics();
void handleInput();

int main(int argc, char **args){
    quit = 0;

    if(argc < 2){
        printf("No ROM selected.\nUsage: %s [application name]\n", args[0]);
        return 0;
    }

    if(!CHIP8_LoadGame(args[1]))
        return 0;

    initGraphics();

    while(!quit){
        CHIP8_EmulateCycle();
        handleInput();

        if(chip8_drawFlag){
            drawGraphics();
            SDL_RenderPresent(renderer);
        }
    }
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
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

void handleInput(){
    if(SDL_PollEvent(&e)){
        if(e.type == SDL_QUIT)
            quit = 1;
        else if(e.type == SDL_KEYDOWN){
            switch(e.key.keysym.sym){
                case SDLK_1:
                    KEY[0x1] = 1;
                break;
                case SDLK_2:
                    KEY[0x2] = 1;
                break;
                case SDLK_3:
                    KEY[0x3] = 1;
                break;
                case SDLK_4:
                    KEY[0xc] = 1;
                break;
                case SDLK_q:
                    KEY[0x4] = 1;
                break;
                case SDLK_w:
                    KEY[0x5] = 1;
                break;
                case SDLK_e:
                    KEY[0x6] = 1;
                break;
                case SDLK_r:
                    KEY[0xD] = 1;
                break;
                case SDLK_a:
                    KEY[0x7] = 1;
                break;
                case SDLK_s:
                    KEY[0x8] = 1;
                break;
                case SDLK_d:
                    KEY[0x9] = 1;
                break;
                case SDLK_f:
                    KEY[0xe] = 1;
                break;
                case SDLK_z:
                    KEY[0xa] = 1;
                break;
                case SDLK_x:
                    KEY[0x0] = 1;
                break;
                case SDLK_c:
                    KEY[0xb] = 1;
                break;
                case SDLK_v:
                    KEY[0xf] = 1;
                break;

                default:
                    printf("Not in the default keys\n");
                break;
            }
        }
        else if(e.type == SDL_KEYUP){
            switch(e.key.keysym.sym){
                case SDLK_1:
                    KEY[0x1] = 0;
                break;
                case SDLK_2:
                    KEY[0x2] = 0;
                break;
                case SDLK_3:
                    KEY[0x3] = 0;
                break;
                case SDLK_4:
                    KEY[0xc] = 0;
                break;
                case SDLK_q:
                    KEY[0x4] = 0;
                break;
                case SDLK_w:
                    KEY[0x5] = 0;
                break;
                case SDLK_e:
                    KEY[0x6] = 0;
                break;
                case SDLK_r:
                    KEY[0xD] = 0;
                break;
                case SDLK_a:
                    KEY[0x7] = 0;
                break;
                case SDLK_s:
                    KEY[0x8] = 0;
                break;
                case SDLK_d:
                    KEY[0x9] = 0;
                break;
                case SDLK_f:
                    KEY[0xe] = 0;
                break;
                case SDLK_z:
                    KEY[0xa] = 0;
                break;
                case SDLK_x:
                    KEY[0x0] = 0;
                break;
                case SDLK_c:
                    KEY[0xb] = 0;
                break;
                case SDLK_v:
                    KEY[0xf] = 0;
                break;

                default:
                    printf("Not in the default keys\n");
                break;
            }
        }
    }
}

void drawGraphics(){
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, display_width, display_height);

   if (!texture){
       printf("Texture error: %s\n", SDL_GetError());
       return NULL;
   }
   if (SDL_SetRenderTarget(renderer, texture) != 0){
       printf("SDL error: %s\n", SDL_GetError());
       return NULL;
   }

   int i = 0;
   for (int x = 0; x < display_width; ++x)
   {
       for (int y = 0; y < display_height; y++)
       {
           if (SCREEN[i] == 0){
               SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
           }
           else if (SCREEN[i] == 1){
               SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
           }
           SDL_RenderDrawPoint(renderer, x, y);
           i++;
       }
   }
   SDL_SetRenderTarget(renderer, NULL);
   SDL_Rect texture_rect;
   texture_rect.x = 0;
   texture_rect.y = 0;
   texture_rect.w = 64;
   texture_rect.h = 32;
   SDL_RenderCopy(renderer,texture,NULL, &texture_rect);
}
