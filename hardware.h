#ifndef HARDWARE_H_
#define HARDWARE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h> 

#include <SDL2/SDL.h>
#include "emulador.h" 
#include "display.h"
//porting
port* init_port();
void generate_interrupt(state8080 *state, port *p, SDL_Renderer **rend);
uint8_t read_i_port(port *i, uint8_t port);
uint16_t write_o_port(port *o, uint8_t port, uint8_t data);
void key_input(SDL_Event event, state8080 *state, port *p);

//display
void draw_space(uint8_t *arr, SDL_Renderer **rend, int y, int d);
int initdisplay(SDL_Window **window, SDL_Renderer **rend);
void prepare_scene(SDL_Renderer **rend);
void stop_sdl(SDL_Window **wind, SDL_Renderer **rend);


//machine
void command_maker(state8080 *state, port *p);
void print_ram(state8080 *state);
#endif 
