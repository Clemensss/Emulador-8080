#ifndef _SCREEN_H
#define  _SCREEN_H

#include <unistd.h>
#include <string.h>
#include <time.h> 

#include <SDL2/SDL.h>
#include "newCpu.h" 

#define BASE_SCR_WIDTH  224
#define BASE_SCR_HEIGHT 256

struct screen_t
{
    SDL_Window *window;   
    SDL_Renderer *rend;   
};

struct screen_t* init_screen(int width, int height);
void prepare_scene(struct screen_t *screen);
void draw_space(uint8_t *arr, struct screen_t*, int startX, int endX);

#endif
