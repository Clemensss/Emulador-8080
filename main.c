#include <stdio.h> 
#include "newCpu.h"

#define DEB printf("Working\n")
void emulator_loop(cpu *cpu);
int cpu_loop(cpu *cpu, int max_cycles);

int main(int argc, char *argv[])
{
    cpu *cpu = init_cpu(argv[1], 4000);
    emulator_loop(cpu);

    return 0;
}

void emulator_loop(cpu *cpu)
{
    while(!cpu->halt)
    {
	cpu_loop(cpu, 16666);
    }
}

int cpu_loop(cpu *cpu, int max_cycles)
{
    int cycles = 0; 
    while(cycles < max_cycles)
    {
	cycles += inst_process(cpu);
    }
}


