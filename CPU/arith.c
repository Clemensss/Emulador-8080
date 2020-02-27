#include "arith.h" 

//---------------------ARITHMETIC---------------------------

//ADD r
void add_register(state8080 *state, uint8_t r)
{
    uint16_t result = state->registers->A + r;
    state->registers->A = set_flags(state, result);
}

//ADD M
void add_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint8_t data = state->RAM[addr];

    add_register(state, data);
}

//ADI data
void add_immediate(state8080 *state)
{
    state->registers->PC++;
    uint8_t data = get_PC_data(state);

    add_register(state, data);
}

//ADC r
void add_register_carry(state8080 *state, uint8_t r)
{
    add_register(state, r);
    state->registers->A += state->status_flags->CY;
}

//ADC M
void add_memory_carry(state8080 *state)
{
    add_memory(state);
    state->registers->A += state->status_flags->CY;
}

//ACI data
void add_immediate_carry(state8080 *state)
{
    add_immediate(state);
    state->registers->A += state->status_flags->CY;
}

//TODO this is retarded
//SUB r
void sub_register(state8080 *state, uint8_t r)
{
    r = twoscomp(r);
    add_register(state, r);
}

//SUB M
void sub_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint8_t data = state->RAM[addr];

    sub_register(state, data);
}

//SUI data
void sub_immediate(state8080 *state)
{
    state->registers->PC++;
    uint8_t data = get_PC_data(state);

    sub_register(state, data);
}

//SBB r
void sub_register_borrow(state8080 *state, uint8_t r)
{
    sub_register(state, r);
    state->registers->A -= state->status_flags->CY;
}

//SBB M
void sub_memory_borrow(state8080 *state)
{
    sub_memory(state);
    state->registers->A -= state->status_flags->CY;
}

//SBI data
void sub_immediate_borrow(state8080 *state)
{
    sub_immediate(state);
    state->registers->A -= state->status_flags->CY;
}

//INR r
void inc_register(state8080 *state, uint8_t *r)
{
    uint16_t result = *r + 1;
    *r = set_flags(state, result);
}

//INR M
void inc_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint16_t result = state->RAM[addr] +1;
    state->RAM[addr] = set_flags(state, result);
}

//DCR r
void dec_register(state8080 *state, uint8_t *r)
{
    uint16_t result = *r - 1;
    *r = set_flags(state, result);
}

//DCR M
void dec_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint16_t result = state->RAM[addr] -1;
    state->RAM[addr] = set_flags(state, result);
}

//INX rp
void inc_reg_pair(uint8_t *rh, uint8_t *rl)
{
    uint16_t content = joint(*rh, *rl);
    content++;
    disjoint(content, rh, rl);
}

//DCX rp
void dec_reg_pair(uint8_t *rh, uint8_t *rl)
{
    uint16_t content = joint(*rh, *rl);
    content--;
    disjoint(content, rh, rl);
}

//DAD rp
void add_reg_pair_HL(state8080 *state, uint8_t *rh, uint8_t *rl)
{
    uint16_t hl = joint(state->registers->H, state->registers->L);
    uint16_t rp = joint(*rh, *rl);

    uint32_t result = hl + rp;

    if(result & 0xffff0000) state->status_flags->CY = 1;
    else state->status_flags->CY = 0;

    result = (uint16_t) result;

    disjoint(result, &state->registers->H, &state->registers->L);
}
