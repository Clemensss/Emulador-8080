#ifndef ARITH_H   /* Include guard */
#define ARITH_H

//ARITHMETIC
void add_register(state8080 *state, uint8_t r);
void add_memory(state8080 *state);
void add_immediate(state8080 *state);
void add_register_carry(state8080 *state, uint8_t r);
void add_memory_carry(state8080 *state);
void add_immediate_carry(state8080 *state);
void sub_register(state8080 *state, uint8_t r);
void sub_memory(state8080 *state);
void sub_immediate(state8080 *state);                  
void sub_register_borrow(state8080 *state, uint8_t r);
void sub_memory_borrow(state8080 *state);
void sub_immediate_borrow(state8080 *state);
void inc_register(state8080 *state, uint8_t *r);
void inc_memory(state8080 *state);
void dec_register(state8080 *state, uint8_t *r);
void dec_memory(state8080 *state);
void inc_reg_pair(uint8_t *rh, uint8_t *rl);
void dec_reg_pair(uint8_t *rh, uint8_t *rl);
void add_reg_pair_HL(state8080 *state, uint8_t *rh, uint8_t *rl);


#endif // FOO_H_)
