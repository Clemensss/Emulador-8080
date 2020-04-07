#include "general.h"
#include <stdlib.h>

/*
    change this shit
*/

void delay(int milliseconds);

void machine_loop(state8080 *state, port *p, SDL_Window **window, SDL_Renderer **rend);
void load_rom(state8080 *state, char *file_name);
uint8_t* rom_reader(char *file_name, int *file_size);
void timer();

int main(int argc, char *argv[])
{
    if(argc != 2) 
    {
	printf("ERROR inputs DUMBASS\n");
	return 1;
    }

    SDL_Event event;
    state8080 *state;

    state = init_machine();
    load_rom(state, argv[1]);

    port *p = init_port();
    SDL_Window *window; 
    SDL_Renderer *rend;
    
    #ifndef DEBUG
    REG->SP = 0x2400;
    initdisplay(&window, &rend);
    prepare_scene(&rend);
    #endif

    machine_loop(state, p, &window, &rend);
    stop_sdl(&window, &rend);
    
    return 0;

}

void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
	now = clock();
}

void machine_loop(state8080 *state, port *p,
		SDL_Window **window, SDL_Renderer **rend)
{
    SDL_Event event;
    long inst_cycle = 0; 
    short it = 1;
    int msec = 0, trigger = 8; /* 10ms */

    clock_t before = clock();
    clock_t difference;
    
    while(!state->halt)
    {
	while(!state->halt)
	{
	    
	    while(inst_cycle < INSTRUCTION_CYCLE)
	    {
		command_maker(state, p);	
		inst_cycle++;
	    }

#ifndef DEBUG

	    if(state->halt) break;
	    
	    if(msec > trigger)
	    {
		if(it) 
		{
		    it = 0;
		    state->inter_opcode = 0xcf;
		    draw_space(&state->RAM[9216], rend, 0, 128);
		}
		else 
		{
		    it = 1;
		    state->inter_opcode = 0xd7;
		    draw_space(&state->RAM[13312],rend, 128, 224);
		}

		state->interrupt = 1;
		state->inter_ind = 1;

		msec = 0; 
		inst_cycle = 0;
		before = clock();
	    }

	    difference = clock() - before;
	    msec = difference * 1000 / CLOCKS_PER_SEC;

	    if(SDL_PollEvent(&event)) 
	    {
		key_input(event, state, p);
	    }
	    else
	    {
		//p = init_port();
	    }
#endif

	}
    }
}


void load_rom(state8080 *state, char *file_name)
{
    int file_size;
    uint8_t *rom = rom_reader(file_name, &file_size);
    memmove(state->RAM, rom, file_size);
}

uint8_t* rom_reader(char *file_name, int *file_size)
{
    FILE *fileptr;
    uint8_t *buffer;
    long filelen;

    fileptr = fopen(file_name, "rb");  // Open the file in binary mode
    fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
    filelen = ftell(fileptr);             // Get the current byte offset in the file
    rewind(fileptr);                      // Jump back to the beginning of the file

    buffer = (uint8_t *)malloc(filelen * sizeof(uint8_t)); // Enough memory for the file
    fread(buffer, filelen, 1, fileptr); // Read in the entire file
    fclose(fileptr); 

    *file_size = filelen;
    return buffer;
}

