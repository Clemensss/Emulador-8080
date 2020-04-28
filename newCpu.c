#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
 *  This the CPU struct, it is made out of 3 register pairs
 *  1 8-bit Accumulator, a PC register and a SP register
 *  3 arrays for ROM memory, STACK, and RAM
 *
 */

}
:

const int cycle_values[] = {
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x00..0x0f
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,
	4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4,
	4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,

	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5, //0x40..0x4f
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,

	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4, //0x80..8x4f
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,

	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, //0xc0..0xcf
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11,
	11, 10, 10, 18, 17, 11, 7, 11, 11, 5, 10, 5, 17, 17, 7, 11,
	11, 10, 10, 4, 17, 11, 7, 11, 11, 5, 10, 4, 17, 17, 7, 11,
	};

cpu_s
{
    uint8_t a;
    uint8_t b;   
    uint8_t c;   
    uint8_t d; 
    uint8_t e; 
    uint16_t hl;     

    uint16_t sp;
    uint16_t pc;    

    uint8_t *ram;
    uint8_t *rom;
    uint16_t *stack;
};

typedef struct cpu_s cpu;
/*
 *  Flags set by alu operations. 
 *  z = 1 if x == 0 else z = 0
 *  s = 1 if x < 0 else s = 0
 *  p = 1 if parity(x) == 1 else p = 0
 *  c = 1 if carry else c = 0
 *  z = 1 if x == 0 else z = 0
 *  ac not used for space invaders
 */
struct flags_s 
{
    uint8_t z;
    uint8_t s;
    uint8_t p;
    uint8_t c;
    uint8_t ac;
};

typedef struct flags_s flags;

int main()
{

    return 0;
}

cpu* init_cpu()
{
    cpu* cpu = (cpu*)malloc(sizeof(cpu));

    cpu->a = 0;
    cpu->b = 0;
    cpu->c = 0;
    cpu->d = 0;
    cpu->e = 0;
    cpu->hl = 0;
    cpu->pc = 0;
    cpu->sp = 0;
     
    cpu->ram = (uint8_t*)malloc(sizeof(uint8_t)*RAM_SIZE);
    cpu->stack = (uint8_t*)malloc(sizeof(uint8_t)*STACK_SIZE);

    for(int i = 0; i < RAM_SIZE; i++)
    {   
	cpu->ram[i] = 0;
    }

    for(int i = 0; i < STACK_SIZE; i++)
    {   
	cpu->stack[i] = 0;
    }

    return cpu;
}

flags* init_flags()
{
    flags* flags = (flags*)malloc(sizeof(flags));

    flags->z = 0;
    flags->p = 0;
    flags->s = 0;
    flags->cy = 0;
    flags->ac = 0;

    return flags;
}

void addm_general()














