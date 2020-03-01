#include "general.h"

int threaded_main(void*);

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
    REG->SP = 0xf000;
    load_rom(state, argv[1]);

    port *p = init_port();

    SDL_Window *window; 
    SDL_Renderer *rend;

    initdisplay(&window, &rend);
    prepare_scene(&rend);
    machine_loop(state, p, &window, &rend);
    stop_sdl(&window, &rend);
    
    return 0;

}

void machine_loop(state8080 *state, port *p,
		SDL_Window **window, SDL_Renderer **rend)
{
    int inst = 0;
    short log = 1;
    uint16_t max = 0;

    SDL_Event event;
    
    short it = 2;

    int ms = 8;
    int stop = 1; 
    int ms_s = clock() * 1000;
    int end = ms_s + ms;

    while(!state->halt)
    {
	while(!state->halt)
	{

	    if(SDL_PollEvent(&event)) 
	    {
		if (event.type == SDL_QUIT) stop = 0;
		key_input(event, state, p);
	    }
	    else
	    {
		ms_s = clock() * 1000;
		
		if(ms_s >= end)
		{
		    int mem;

		    if(it-1) 
		    {
			restart(state, it);
			it = 1;
			draw_space(&state->RAM[0x2400], rend, 0, 128);
		    }
		    else 
		    {
			restart(state, it);
			it = 2;
			draw_space(&state->RAM[0x2400+0x1000],rend, 128, 244);
		    }
		    
		    

		    ms_s = clock() * 1000;
		    end - ms_s + ms;
		}
		
		command_maker(state, p);
		/*if(log)print_state(state);
		if(inst >= max)
		{
		    print_state(state);
		    printf("inst: %d\n", inst);
		    log = 1;
		    inst = 0;
		}
		if(log)
		{
		    scanf("%hd", &max);
		    log = 0;
		}*/
		print_state(state);
	    }
	}
    }
}


void load_rom(state8080 *state, char *file_name)
{
    int file_size;
    uint8_t *rom = rom_reader(file_name, &file_size);
    memcpy(state->RAM, rom, file_size);
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

