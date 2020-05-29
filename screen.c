#include "screen.h"

struct screen_t* init_screen(int width, int height)
{
    struct screen_t *screen; 

    screen = (struct screen_t*)malloc(sizeof(struct screen_t));

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
	printf("SDL_Init Error: %s\n",  SDL_GetError());
	exit(1);
    }
    
    screen->window = SDL_CreateWindow("SPACE INVADERS",
			SDL_WINDOWPOS_CENTERED, 
			SDL_WINDOWPOS_CENTERED,
			width,
			height,
			SDL_WINDOW_SHOWN);

    if(screen->window == NULL)
    {
	printf("SDL_Window Error: %s\n",  SDL_GetError());
	exit(1);
    }

    screen->rend = SDL_CreateRenderer(screen->window, -1, SDL_RENDERER_SOFTWARE);
    
    if(screen->rend == NULL)
    {
	printf("SDL_Renderer Error: %s\n",  SDL_GetError());
	exit(1);
    }

    return screen;
}

void prepare_scene(struct screen_t *screen)
{
    SDL_SetRenderDrawColor(screen->rend, 0, 0, 0, 0);
    SDL_RenderClear(screen->rend);
    SDL_SetRenderDrawColor(screen->rend, 255, 255, 255, 255);
}

void draw_space(uint8_t *arr, struct screen_t *screen, int start_x, int end_x)
{
    int var = 0, bit = 0;
    
    int color = 0;

    for(int i = start_x; i < end_x; i++)
    {
	for(int j = 256; j > 0; j--)
	{
	    if(bit > 7)
	    {
		bit = 0;
		var++;
	    }
	    
	    color = is_bit_set(arr[var], bit) ? 255 : 0;

	    SDL_SetRenderDrawColor(screen->rend, color, color, color, color);

	    if(SDL_RenderDrawPoint(screen->rend, i, j)) printf("%s\n",  SDL_GetError());

	    bit++;	
	}
    }

    SDL_RenderPresent(screen->rend);
}
