#ifndef EMULATOR_H_   /* Include guard */
#define EMULATOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define RAM_SIZE 16000
#define REG state->registers

struct ConditionFlags
{
    uint8_t Z:1;
    uint8_t S:1;
    uint8_t P:1;
    uint8_t CY:1;
    uint8_t AC:1;
    uint8_t jmp:1;
};

typedef struct ConditionFlags flags;

struct Registers
{
    uint8_t A;    
    uint8_t B;    
    uint8_t C;
    uint8_t D;    
    uint8_t E;    
    uint8_t H;    
    uint8_t L;    

    uint16_t SP;    
    uint16_t PC;    
};

typedef struct Registers reg;

struct Machine
{
    flags *status_flags;
    reg *registers;

    uint8_t halt:1;
    uint8_t interrupt:1;

    uint8_t *RAM;
};

typedef struct Machine state8080;

//INIT
state8080* init_machine();
reg* init_reg();
flags* init_flag();

//TEST
void tests();
void print_state(state8080 *state);

#endif 

