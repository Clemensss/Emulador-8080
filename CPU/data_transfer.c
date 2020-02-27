#include "data_transfer.h"

//-------------------- DATA TRANSFER -------------------------

//MOV r1, r2
void move_register(uint8_t *r1, uint8_t *r2)
{
    *r1 = *r2;
}

//MOV r, M
void move_from_mem(state8080 *state, uint8_t *r)
{
    uint16_t addr = get_HL_addr(state);
    *r = state->RAM[addr];
}

//MOV M, r
void move_to_mem(state8080 *state, uint8_t *r)
{
    uint16_t addr = get_HL_addr(state);
    state->RAM[addr] = *r;
}

//MVI r, data
void move_immediate(state8080 *state, uint8_t *r)
{
    state->registers->PC++;
    *r = state->RAM[state->registers->PC];
}

//MVI M, data
void move_to_mem_imed(state8080 *state)
{
    state->registers->PC++;
    uint8_t data = get_PC_data(state);

    uint16_t addr = get_HL_addr(state);
    state->RAM[addr] = data;
}

//LXP rp, data 16
void load_reg_pair_imed(state8080 *state, uint8_t *rh, uint8_t *rl)
{
    state->registers->PC++;
    uint8_t byte2 = get_PC_data(state);

    state->registers->PC++;
    uint8_t byte3 = get_PC_data(state);

    *rh = byte3;
    *rl = byte2;
}

//LDA addr
void load_acc_dir(state8080 *state)
{
    uint16_t addr = get_bytes_addr(state);
    state->registers->A = state->RAM[addr];
}

//STA addr
void store_acc_dir(state8080 *state)
{
    uint16_t addr = get_bytes_addr(state);
    state->RAM[addr] = state->registers->A;
}

//LHLD addr
void load_HL_dir(state8080 *state)
{
    uint16_t addr = get_bytes_addr(state);
    
    state->registers->L = addr;
    state->registers->H = addr+1;
}

//SHLD addr
void store_HL_dir(state8080 *state)
{
    uint16_t addr = get_bytes_addr(state);

    state->RAM[addr] =  state->registers->L; 
    state->RAM[(addr+1)] =  state->registers->H; 
}

//LDAX rp
void load_acc_indir(state8080 *state, uint8_t *rh, uint8_t *rl)
{
    uint16_t addr = joint(*rh, *rl);
    state->registers->A = state->RAM[addr];
}

//STAX rp
void store_acc_indir(state8080 *state, uint8_t *rh, uint8_t *rl)
{
    uint16_t addr = joint(*rh, *rl);
    state->RAM[addr] = state->registers->A;
}

//XCHG
void exchange_HL_DE(state8080 *state)
{
    swap(&state->registers->H, &state->registers->D);
    swap(&state->registers->L, &state->registers->E);
}


