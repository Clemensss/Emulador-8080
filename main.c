#include <stdio.h> 
#include <stdbool.h>

#include "screen.h"
#define DEB printf("Working\n")

#define MID_SCREEN_ADDR 
#define END_SCREEN_ADDR

#define TIME_SCREEN_INTR 8
#define TIME_INST_BURST  1
#define MAX_CYCLES 10000

void emulator_loop(cpu *cpu, struct screen_t *screen);
int cpu_loop(cpu *cpu, int max_cycles);
void display_intr(cpu *cpu, struct screen_t *screen, uint8_t *toggle);
int timer_intr(int *milisec, int trigger, clock_t *before, clock_t *diff);
void key_input(cpu *cpu, SDL_Event event);

int main(int argc, char *argv[])
{
    /*cpu *cpu = init_cpu(argv[1], 0x10000);
    emulator_loop(cpu);*/

    struct cpu_t    *cpu;
    struct screen_t *screen;

    cpu = init_cpu(argv[1], 0x10000);
    screen = init_screen(BASE_SCR_WIDTH, BASE_SCR_HEIGHT);

    prepare_scene(screen);

    emulator_loop(cpu, screen);

    free(cpu);
    free(screen);

    return 0;
}

void emulator_loop(cpu *cpu, struct screen_t *screen)
{
    SDL_Event event;
    uint8_t toggle = 1;

    int screen_milisec = 0,
        inst_milisec   = 0;

    clock_t screen_before = clock(),
	    inst_before   = clock();

    clock_t screen_diff,
	    inst_diff;
   
    uint8_t burst = 1;

    while(!cpu->halt)
    {
	//MAKE THIS WORK
	/*Basically it should burst a couple of insts 
	  for each while loop and when it reaches a 
	  max it simply stops*/

	if(burst)
	{
	     cpu_loop(cpu, 5000);
	     burst = 0;
	}

	//if(cpu->halt) break;

#ifndef CPUDIAG

	//MAKE THIS WORK
	if(timer_intr(&inst_milisec, TIME_INST_BURST, &inst_before, &inst_diff))
	    burst = 1;

	if(timer_intr(&screen_milisec, TIME_SCREEN_INTR, &screen_before, &screen_diff))
	    display_intr(cpu, screen, &toggle);
	
	key_input(cpu, event);
#endif

    }
}

int timer_intr(int *milisec, int trigger, clock_t *before, clock_t *diff)
{
    if(*milisec > trigger)
    {
	*milisec = 0; 
	*before = clock();
	return 1;
    }

    *diff = clock() - *before;
    *milisec = (*diff) * 1000 / CLOCKS_PER_SEC;
    return 0;
}

void display_intr(cpu *cpu, struct screen_t *screen, uint8_t *toggle)
{
    int addr    = (*toggle) ? 9216 : 13312;
    int intr    = (*toggle) ? 0xcf :  0xd7;
    int start_x = (*toggle) ? 0    :   128;
    int end_x   = (*toggle) ? 128  :   224;

    draw_space(mem_ptr_out(cpu, addr), screen, start_x, end_x);
    generate_intr(cpu, intr);

    *toggle = !(*toggle);
}

int cpu_loop(cpu *cpu, int max_cycles)
{
    int cycles = 0; 
    while(cycles < max_cycles && !cpu->halt)
    {
	cycles += inst_process(cpu);
    }

    return max_cycles;
}

void key_input(cpu *cpu, SDL_Event event)
{
    while(SDL_PollEvent(&event)) 
    {
	switch(event.type)
	{
	    case SDL_KEYDOWN:
		switch(event.key.keysym.sym)
		{
		    case SDLK_q:

			exit(1);
			break;

		    case SDLK_p:

			break;

		    case SDLK_c :
			
			break;

		    case SDLK_SPACE:

			break;

		    case SDLK_LEFT:

			break;

		    case SDLK_RIGHT:

			break;
		}

	}
    }
}
