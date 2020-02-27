#include "logical.h"

//-------------------- LOGICAL ------------------------------

//general and thing
void and_thing(state8080 *state, uint8_t var)
{
    uint8_t result = state->registers->A & var;
    set_flags(state, result);
    state->status_flags->CY = 0;
    state->registers->A = result;
}

//ANA r
void and_register(state8080 *state, uint8_t r)
{
    and_thing(state, r);
}

//ANA M
void and_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint8_t data = state->RAM[addr];

    and_thing(state, data);
}

//ANI data
void and_immediate(state8080 *state)
{
    state->registers->PC++;
    uint8_t data = get_PC_data(state);

    and_thing(state, data);
}

//general xor thing
void xor_thing(state8080 *state, uint8_t var)
{
    uint8_t result = state->registers->A ^ var;
    set_flags(state, result);
    state->status_flags->CY = 0;
    state->status_flags->AC = 0;
    state->registers->A = result;
}

//XRA r
void xor_register(state8080 *state, uint8_t r)
{
    xor_thing(state, r);
}

//XRA M
void xor_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint8_t data = state->RAM[addr];

    xor_thing(state, data);
}

//XRI data
void xor_immediate(state8080 *state)
{
    state->registers->PC++;
    uint8_t data = get_PC_data(state);

    xor_thing(state, data);
}

//general or thing
void or_thing(state8080 *state, uint8_t var)
{
    uint8_t result = state->registers->A | var;
    set_flags(state, result);
    state->status_flags->CY = 0;
    state->status_flags->AC = 0;
    state->registers->A = result;
}

//ORA r
void or_register(state8080 *state, uint8_t r)
{
    or_thing(state, r);
}

//ORA M
void or_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint8_t data = state->RAM[addr];

    or_thing(state, data);
}

//ORI data
void or_immediate(state8080 *state)
{
    state->registers->PC++;
    uint8_t data = get_PC_data(state);

    or_thing(state, data);
}

//TODO test this
//general or thing
void cmp_thing(state8080 *state, uint8_t var)
{
    uint16_t result = state->registers->A - var;
    set_flags(state, result);
}

//CMP r
void cmp_register(state8080 *state, uint8_t r)
{
    cmp_thing(state, r);
}

//CMP M
void cmp_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint8_t data = state->RAM[addr];

    cmp_thing(state, data);
}

//CPI data
void cmp_immediate(state8080 *state)
{
    state->registers->PC++;
    uint8_t data = get_PC_data(state);

    cmp_thing(state, data);
}


//general rotate byte
void rotate_byte(state8080 *state, uint8_t cy_set, uint8_t lone_bit, uint8_t shifted)
{
    uint8_t result = (lone_bit | shifted);    
    state->status_flags->CY = !(0 == cy_set);    
    state->registers->A = result;
}

//RLC
void rotate_left(state8080 *state)
{
    uint8_t var = state->registers->A;
    uint8_t lone_bit = (var >> 7);
    rotate_byte(state, lone_bit, lone_bit, (var << 1));
}

//RRC 
void rotate_right(state8080 *state)
{
    uint8_t var = state->registers->A;
    uint8_t lone_bit = (var << 7);
    rotate_byte(state, lone_bit, lone_bit, (var >> 1));
}

//RAL
void rotate_left_carry(state8080 *state)
{
    uint8_t var = state->registers->A;
    uint8_t cy = state->status_flags->CY;
    rotate_byte(state, (var >> 7), cy, (var << 1));
}

//RAR
void rotate_right_carry(state8080 *state)
{
    uint8_t var = state->registers->A;
    uint8_t cy = state->status_flags->CY;
    rotate_byte(state, (var << 7), (cy << 7), (var >> 1));
}

//CMA
void complement_acc(state8080 *state)
{
    state->registers->A = ~(state->registers->A);
}

//CMC
void complement_carry(state8080 *state)
{
    state->status_flags->CY = ~(state->status_flags->CY);
}

//STC
void set_carry(state8080 *state)
{
    state->status_flags->CY = 1;
}
