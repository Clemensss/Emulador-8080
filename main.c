#include <stdio.h> 
#include "screen.h"
#define DEB printf("Working\n")

#define MID_SCREEN_ADDR 
#define END_SCREEN_ADDR
#define MAX_CYCLES 10000

void emulator_loop(cpu *cpu, struct screen_t *screen);
int cpu_loop(cpu *cpu, int max_cycles);
void display_intr(cpu *cpu, struct screen_t *screen, uint8_t *toggle);
int space_invaders_intr(int *milisec, int trigger, clock_t *before, clock_t *diff);

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

    //stop_sdl(&window, &rend);

    return 0;
}

void emulator_loop(cpu *cpu, struct screen_t *screen)
{
    SDL_Event event;
    int    milisec = 0, 
	   trigger = 8; 
    uint8_t toggle = 1;
    clock_t before = clock();
    clock_t diff;
   
    int max_total_cycles = 0;

    while(!cpu->halt)
    {
	//MAKE THIS WORK
	/*Basically it should burst a couple of insts 
	  for each while loop and when it reaches a 
	  max it simply stops*/

	if(max_total_cycles < MAX_CYCLES)
	    max_total_cycles += cpu_loop(cpu, 16666);


	//why is this here?
	if(cpu->halt) break;

#ifndef CPUDIAG

	//MAKE THIS WORK
	if(space_invaders_intr(&milisec, trigger, &before, &diff))
	{
	    display_intr(cpu, screen, &toggle);
	    max_total_cycles = 0;
	}
	
	//FIGURE WHAT THE FUCK IS GOING ON HERE
	/*if(SDL_PollEvent(&event)) 
	{
	    key_input(event, state, p);
	}
	else
	{
	    //p = init_port();
	}*/
#endif

    }
}

int space_invaders_intr(int *milisec, int trigger, clock_t *before, clock_t *diff)
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
    if(*toggle)
    {
	draw_space(mem_ptr_out(cpu, 9216), screen, 0, 128);
	generate_intr(cpu, 0xcf);
	printf("DEBUG1\n");
    }
    else
    {
	draw_space(mem_ptr_out(cpu, 13312), screen, 128, 224);
	generate_intr(cpu, 0xd7);
    }

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


