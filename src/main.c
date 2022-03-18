#include <stdio.h> 
#include <stdbool.h>
#include "screen.h"

#define DEB printf("Working\n")

#define MID_SCREEN_ADDR 
#define END_SCREEN_ADDR

#define TIME_SCREEN_INTR 8
#define TIME_INST_BURST  2

#define TIC (1000.0 / 60.0)  // Milliseconds per tic
#define CYCLES_PER_MS 2000  // 8080 runs at 2 Mhz
#define CYCLES_PER_TIC (CYCLES_PER_MS * TIC)
#define MAX_CYCLES 16000

void emulator_loop(cpu *cpu);
void space_invaders_loop(cpu *cpu, struct screen_t *screen);
int cpu_loop(cpu *cpu, int cycles);
void display_intr(cpu *cpu, uint8_t *toggle);

int timer_intr(int *milisec, int trigger, clock_t *before, clock_t *diff);
void key_input(cpu *cpu, SDL_Event event);

int main(int argc, char *argv[])
{
    struct cpu_t    *cpu;
    struct screen_t *screen;


    /* 
     * defines a clear difference between raw cpu
     * emulation and space invaders 
     */
	if(argc != 3) 
	{
		printf("BURRO\n");
		return 1; 
	}
	else
    cpu = init_cpu(argv[2], 0x10000);

	{
		if(!strcmp(argv[1], "-e"))
		{
			emulator_loop(cpu);
		}

		else if(!strcmp(argv[1], "-i"))
		{
			screen = init_screen(BASE_SCR_WIDTH, BASE_SCR_HEIGHT);

			prepare_scene(screen);

			space_invaders_loop(cpu, screen);
			free(screen);
		}
		else
			printf("Emu type not specified\n");

		free(cpu);
	}
    return 0;
}

void space_invaders_loop(cpu *cpu, struct screen_t *screen)
{
    uint8_t toggle = 1;
	uint32_t last_tic = SDL_GetTicks();

    while(!cpu->halt)
    {
		if ((SDL_GetTicks() - last_tic) >= TIC)
		{
            last_tic = SDL_GetTicks();

			/*Basically it should burst a couple of insts 
			for each while loop and when it reaches a 
			max it simply stops*/

			cpu_loop(cpu, CYCLES_PER_TIC/2);
			display_intr(cpu, &toggle);


			cpu_loop(cpu, CYCLES_PER_TIC/2);
			draw_space(mem_ptr_out(cpu, 9216), screen, 0, 224);

			display_intr(cpu, &toggle);


		}
    }
}

void emulator_loop(cpu *cpu)
{
    while(!cpu->halt)
    {
	cpu_loop(cpu, MAX_CYCLES);
    }
}

//Something is wrong
int timer_intr(int *milisec, int trigger, clock_t *before, clock_t *diff)
{
    if(*milisec >= trigger)
    {
		*milisec = 0; 
		*before = clock();
		return 1;
    }

    *diff = clock() - *before;
    *milisec = (*diff) * 1000 / CLOCKS_PER_SEC;
    return 0;
}

void display_intr(cpu *cpu, uint8_t *toggle)
{
    int intr    = (*toggle) ? 0xcf :  0xd7;
    generate_intr(cpu, intr);

    *toggle = !(*toggle);
}

int cpu_loop(cpu *cpu, int cycles)
{
    int i = 0; 
    while(i < cycles)
    {
		i += inst_process(cpu); 
		
		if(cpu->halt) exit(1);
    }

    return cycles;
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
