#include <unistd.h>
#include <string.h>

#include "machine.h"
#include "emulador.h"

void machine_loop(state8080 *state);
void load_rom(state8080 *state, char *file_name);
uint8_t* rom_reader(char *file_name, int *file_size);

int main(int argc, char **argv)
{
    state8080 *state = init_machine();
    load_rom(state, argv[1]);
    REG->SP = 0xf000;
    machine_loop(state);
    return 0;
}

void machine_loop(state8080 *state)
{
    uint16_t inst = 0;
    short log = 1;
    uint16_t max = 0;
    while(!state->halt)
    {
	command_maker(state);
	
	inst++;
	
	
	//if(log)print_state(state);
	
	if(inst >= max)
	{
	    print_state(state);
	    log = 1;
	    inst = 0;
	}

	if(log)
	{
	    scanf("%hd", &max);
	    log = 0;
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

