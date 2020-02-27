#include "branch_group.h" 

//-------------------- BRANCH GROUP -------------------------

//JMP addr
//addr-1 because for every intruction loop the pc e increased by 1
void jump(state8080 *state)
{
    state->registers->PC++;
    uint8_t byte2 = get_PC_data(state);

    state->registers->PC++;
    uint8_t byte3 = get_PC_data(state);

    uint16_t addr = joint(byte3, byte2);
    state->registers->PC = addr;
    state->status_flags->jmp = 1;
}

//Jcondition addr
void cond_jump(state8080 *state, uint8_t flag)
{
    if(flag) jump(state);
}

//CALL addr
void call(state8080 *state)
{
    uint16_t stack = state->registers->SP;
    uint8_t pch, pcl;
    
    disjoint(state->registers->PC, &pch, &pcl);
    push(state, pch, pcl);
    jump(state);
}

//Condition addr
void cond_call(state8080 *state, uint8_t flag)
{
    if(flag) call(state);
}

//RET
void ret_op(state8080 *state)
{
    uint16_t stack = state->registers->SP;
    uint8_t pch, pcl;
    
    disjoint(state->registers->PC, &pch, &pcl);

    pop(state, &pch, &pcl);

    state->registers->PC = (joint(pch, pcl));
    state->status_flags->jmp = 1;
}

//Rcondition
void cond_ret_op(state8080 *state, uint8_t flag)
{
    if(flag) ret_op(state);
}

//RST n
void restart(state8080 *state, uint8_t opcode)
{
    uint16_t stack = state->registers->SP;
    uint8_t pch, pcl;
    disjoint(state->registers->PC, &pch, &pcl);

    push(state, pch, pcl);

    state->registers->PC = opcode << 3;
    state->status_flags->jmp = 1;
}

//PCHL
void jump_HL_dir(state8080 *state)
{
    uint8_t pch, pcl;
    
    disjoint(state->registers->PC, &pch, &pcl);
    pcl = state->registers->L;
    pch = state->registers->H;
    
    state->registers->PC = joint(pch, pcl);
    state->status_flags->jmp = 1;
}




