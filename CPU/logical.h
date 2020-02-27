#ifndef LOGIC_H_   /* Include guard */
#define LOGIC_H_

//LOGICAL
void and_thing(state8080 *state, uint8_t var);
void and_register(state8080 *state, uint8_t r);
void and_memory(state8080 *state);
void and_immediate(state8080 *state);
void xor_thing(state8080 *state, uint8_t var);
void xor_register(state8080 *state, uint8_t r);
void xor_memory(state8080 *state);
void xor_immediate(state8080 *state);
void or_thing(state8080 *state, uint8_t var);
void or_register(state8080 *state, uint8_t r);
void or_memory(state8080 *state);
void or_immediate(state8080 *state) ;
void cmp_thing(state8080 *state, uint8_t var);
void cmp_register(state8080 *state, uint8_t r);
void cmp_memory(state8080 *state);
void cmp_immediate(state8080 *state);

void rotate_byte(state8080 *state, uint8_t cy_set, 
		uint8_t lone_bit, uint8_t shifted);

void rotate_left(state8080 *state);        
void rotate_right(state8080 *state);
void rotate_left_carry(state8080 *state);
void rotate_right_carry(state8080 *state);
void complement_acc(state8080 *state);
void complement_carry(state8080 *state);
void set_carry(state8080 *state);


#endif // FOO_H_)
