//--------------------STACK ---------------------------------

//PUSH rp
void push(state8080 *state, uint8_t rh, uint8_t rl)
{
    uint16_t stack = state->registers->SP;
    
    state->RAM[(stack-1)] = rh;
    state->RAM[(stack-2)] = rl;

    state->registers->SP -= 2;
}

//PUSH PSW
void push_psw(state8080 *state)
{
    uint8_t psw = 0;

    set_bit(psw, 1);
    if(state->status_flags->CY) set_bit(psw, 0);
    if(state->status_flags->P) set_bit(psw, 2);
    if(state->status_flags->AC) set_bit(psw, 4);
    if(state->status_flags->Z) set_bit(psw, 6);
    if(state->status_flags->S) set_bit(psw, 7);

    push(state, state->registers->A, psw);
}

//POP rp
void pop(state8080 *state, uint8_t *rh, uint8_t *rl)
{
    uint16_t stack = state->registers->SP;
    
    *rl = state->RAM[(stack)];
    *rh = state->RAM[(stack+1)];
    
    state->registers->SP += 2;
}

//POP PSW
void pop_psw(state8080 *state)
{
    uint8_t reg_a, psw;
    pop(state, &reg_a, &psw);

    state->registers->A = reg_a;

    if(is_bit_set(psw, 0)) state->status_flags->CY = 1;
    else state->status_flags->CY = 0;

    if(is_bit_set(psw, 2)) state->status_flags->P = 1;
    else state->status_flags->P = 0;

    if(is_bit_set(psw, 4)) state->status_flags->AC = 1;
    else state->status_flags->AC = 0;

    if(is_bit_set(psw, 6)) state->status_flags->Z = 1;
    else state->status_flags->Z = 0;

    if(is_bit_set(psw, 7)) state->status_flags->S = 1;
    else state->status_flags->S = 0;
}

//XTHL
void exchange_HL_st(state8080 *state)
{
    uint16_t stack = state->registers->SP;
    swap(&state->registers->H, &state->RAM[stack]);
    swap(&state->registers->L, &state->RAM[(stack+1)]);
}

//SPHL
void move_HL_SP(state8080 *state)
{
    state->registers->SP = joint(state->registers->H, state->registers->L);
}
