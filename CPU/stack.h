#ifndef STACK_H_   /* Include guard */
#define STACK_H__

//STACK
void push(state8080 *state, uint8_t rh, uint8_t rl);
void push_psw(state8080 *state);
void pop(state8080 *state, uint8_t *rh, uint8_t *rl);
void pop_psw(state8080 *state);
void exchange_HL_st(state8080 *state);
void move_HL_SP(state8080 *state);


#endif 
