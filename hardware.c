#include "hardware.h" 


//hardware
//TODO
    //display and shit
port* init_port()
{
    port *p = (port*)malloc(sizeof(port));

    p->input->port0;
    p->input->port1;
    p->input->port2;
    p->input->port3;

    p->output->port2;
    p->output->port3;
    p->output->port4;
    p->output->port5;
    p->output->port6;

    return p;
}

//TODO figure how the game comunicates with the port
void key_input(SDL_Event event, state8080 *state, port *p)
{
    switch(event.type)
    {
	case SDL_KEYDOWN:
	    switch(event.key.keysym.sym)
	    {
		case SDLK_q:

		    state->halt = 1;
		    break;

		case SDLK_p:

		    p->input->port1 = set_bit(p->input->port1, 2);
		    break;

		case SDLK_c :
		    
		    p->input->port1 = set_bit(p->input->port1, 0);
		    break;

		case SDLK_SPACE:

		    p->input->port1 = set_bit(p->input->port1, 4);
		    break;

		case SDLK_LEFT:

		    p->input->port1 = set_bit(p->input->port1, 5);
		    break;

		case SDLK_RIGHT:

		    p->input->port1 = set_bit(p->input->port1, 6);
		    printf("asudhasudh\n");
		    break;
	    }

    }
}

uint8_t read_i_port(port *i, uint8_t port)
{
    if(port == 0) return i->input->port0;
    else if(port == 1) return i->input->port1;
    else if(port == 2) return i->input->port2;
    else if(port == 3) return i->input->port3;
}

uint16_t write_o_port(port *o, uint8_t port, uint8_t data)
{
    if(port == 2)
    {
	uint8_t x = 8 - data;
	uint8_t result = o->output->port4 >> x;
	o->input->port3 = result;
    }
    
    else if(port == 5)
    {
	//something around fleet data;
    }
    
    else if(port == 4) 
    {
	data = (uint16_t) data;
	o->output->port4 = data << 8 | o->output->port4 >> 8;	
    }
}

uint16_t read_o_port(port *o, uint8_t port, uint8_t data)
{
    
}


uint8_t write_i_port(port *i, uint8_t port)
{
    
}
    
//put that into the command maker
void user_input(uint8_t *port) 
{
    
}

//display
void stop_sdl(SDL_Window **wind, SDL_Renderer **rend)
{
    SDL_DestroyRenderer(*rend);
    SDL_DestroyWindow(*wind);
    SDL_Quit();
}

int initdisplay(SDL_Window **window, SDL_Renderer **rend)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
	printf("SDL_Init Error: %s\n",  SDL_GetError());
	return 1;
    }
    
    *window = SDL_CreateWindow("SPACE INVADERS",
			SDL_WINDOWPOS_CENTERED, 
			SDL_WINDOWPOS_CENTERED,
			244,
			256,
			SDL_WINDOW_SHOWN);

    if(*window == NULL)
    {
	printf("SDL_Window Error: %s\n",  SDL_GetError());
	return 1;
    }

    *rend = SDL_CreateRenderer(*window, -1, SDL_RENDERER_SOFTWARE);
    if(*rend == NULL)
    {
	printf("SDL_Renderer Error: %s\n",  SDL_GetError());
	return 1;
    }
}

void prepare_scene(SDL_Renderer **rend)
{
    SDL_SetRenderDrawColor(*rend, 0, 0, 0, 0);
    SDL_RenderClear(*rend);
    SDL_SetRenderDrawColor(*rend, 255, 255, 255, 255);
}

void draw_space(uint8_t *arr, SDL_Renderer **rend, int startX, int endX)
{
    int var = 0; 
    int bit = 0;
    for(int i = startX; i < endX; i++)
    {
	for(int j = 0; j < 256; j--)
	{
	    if(bit > 7)
	    {
		bit = 0;
		var++;
	    }
	    
	    if(is_bit_set(arr[var], bit))
		SDL_RenderDrawPoint(*rend, i, j);

	    bit++;	
	}
    }

    SDL_RenderPresent(*rend);
}

