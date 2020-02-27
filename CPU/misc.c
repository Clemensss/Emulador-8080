#include "misc.h"

//MISC

//joins the content of two reg pairs
uint16_t joint(uint8_t rh, uint8_t rl)
{    
    uint16_t byte2 = rh;
    uint16_t byte1 = rl;
    return  byte2 << 8 | byte1;
}

//disjoints the contents of a reg pair 
void disjoint(uint16_t joint, uint8_t *rh, uint8_t *rl)
{
    *rl = (uint8_t)  joint;
    *rh = (uint8_t) (joint >> 8);
}

//gets the address inside hl reg pair
uint16_t get_HL_addr(state8080 *state)
{
    return  joint(state->registers->H, state->registers->L);
}

//gets whatever is in the pc
uint8_t get_PC_data(state8080 *state)
{
    return state->RAM[state->registers->PC];
}

//gets the bytes in the imediate 2byte addr
uint16_t get_bytes_addr(state8080 *state)
{
    state->registers->PC++;
    uint8_t byte2 = get_PC_data(state);

    state->registers->PC++;
    uint8_t byte3 = get_PC_data(state);
   
    return joint(byte3, byte2);
}

void swap(uint8_t *one, uint8_t *two)
{
    uint8_t temp;
    temp = *one;
    *one = *two;
    *two = temp;
}

int is_bit_set(uint8_t byte, int bit)
{
    return byte & (1 << bit);
}

uint8_t set_bit(uint8_t byte, uint8_t bit)
{
    return byte | 0x01 << bit; 
}

int parity(uint8_t num)
{
    return !(num%2);
}

uint8_t set_flags(state8080 *state, uint16_t result)
{
    if((result & 0xff00)) state->status_flags->CY = 1;
    else state->status_flags->CY = 0;

    result = (uint8_t) result;

    if(is_bit_set(result, 7)) state->status_flags->S = 1;
    else state->status_flags->S = 0;

    if(parity(result)) state->status_flags->P = 1;
    else state->status_flags->P = 0;

    if(!result) state->status_flags->Z = 1;
    else state->status_flags->Z = 0;
    
    return result;
}

uint8_t twoscomp(uint8_t num)
{
    return (~num)+1;
}


