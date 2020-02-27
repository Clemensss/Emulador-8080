#ifndef DATA_H_   /* Include guard */
#define DATA_H_

void move_register(uint8_t *r1, uint8_t * r2);
void move_from_mem(state8080 *state, uint8_t *r1);
void move_to_mem(state8080 *state, uint8_t *r1);
void move_to_mem_imed(state8080 *state);
void load_reg_pair_imed(state8080 *state, uint8_t *rh, uint8_t *rl);
void load_acc_dir(state8080 *state);
void store_acc_dir(state8080 *state);
void load_HL_dir(state8080 *state);

#endif // FOO_H_)
