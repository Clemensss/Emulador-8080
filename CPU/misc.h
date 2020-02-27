#ifndef MISC_H_   /* Include guard */
#define MISC_H_

//MISC
uint16_t joint(uint8_t rh, uint8_t rl);
void disjoint(uint16_t joint, uint8_t *rh, uint8_t *rl);
uint16_t get_HL_addr(state8080 *state);
uint8_t get_PC_data(state8080 *state);
uint16_t get_bytes_addr(state8080 *state);
void swap(uint8_t *one, uint8_t *two);
int is_bit_set(uint8_t byte, int bit);
uint8_t set_flags(state8080 *state, uint16_t result);
int parity(uint8_t num);
uint8_t twoscomp(uint8_t num);
uint8_t set_bit(uint8_t byte, uint8_t bit);


#endif // FOO_H_)
