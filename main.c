#include <stdio.h> 
#include "newCpu.h"

int main(int argc, char *argv[])
{
    cpu *cpu = init_cpu("invaders", 1000, 1000);

    for(int i = 0x01; i <= 0xff; i++)
    {
        printf("adshaiod\n");
	inst_process(cpu, i);
    }
}


