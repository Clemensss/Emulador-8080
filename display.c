#include <stdio.h>
#include <SDL.h>
#include "emulador.h"

void draw_space(uint8_t *arr);
void display();

state8080 *state;

int main()
{
    state = init_machine();
    display();
    return 0;
}

void display()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
	printf("SDL_Init Error: %s\n",  SDL_GetError());
	return 1;
    }
    
    SDL_Window *win = SDL_CreateWindow("SPACE INVADERS",
			SDL_WINDOWPOS_CENTERED, 
			SDL_WINDOWPOS_CENTERED,
			244,
			256,
			SDL_WINDOW_SHOWN)

    if(window == NULL)
    {

    }

    draw_space(state->RAM);
}

void draw_space(uint8_t *arr)
{
    int var = 0; 
    int bit = 0;
    for(int i = 0; i < 256; i++)
    {
	for(int j = 0; j < 244; j++)
	{
	    if(bit > 7)
	    {
		bit = 0;
		var++;
	    }
	    
	    char *c;
	    *c = '#';
	    if(is_bit_set(arr[var], bit)) *c = '#';
	    printf("%#04x\n", arr[var]);
	    c[1] = '\0';
	    //mvprintw(j, i, c);

	    bit++;	
	}
    }

	    //refresh();
}

