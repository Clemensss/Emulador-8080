#ifndef BRANCH_H_   /* Include guard */
#define BRANCH_H_

//BRANCH
void jump(state8080 *state);
void cond_jump(state8080 *state, uint8_t flag)  ;
void call(state8080 *state);
void cond_call(state8080 *state, uint8_t flag);
void ret_op(state8080 *state);
void cond_ret_op(state8080 *state, uint8_t flag);
void restart(state8080 *state, uint8_t opcode);
void jump_HL_dir(state8080 *state);


#endif // FOO_H_)
