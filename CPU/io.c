#include "io.h"

//IN port
void input(state8080 *state, uint8_t data)
{
    state->registers->A = data;
}

//OUT port
void output(state8080 *state, uint8_t *data)
{
    *data = state->registers->A;
}

//EI
void enable_inter(state8080 *state)
{
    state->interrupt = 1;
}

//DI
void disable_inter(state8080 *state)
{
    state->interrupt = 0;
}

//HLT
void halt(state8080 *state)
{
    state->halt = 1;
}


