#ifndef EMULATOR_H_   /* Include guard */
#define EMULATOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define RAM_SIZE 16000
struct ConditionFlags
{
    uint8_t Z:1;
    uint8_t S:1;
    uint8_t P:1;
    uint8_t CY:1;
    uint8_t AC:1;
};

typedef struct ConditionFlags flags;

struct Registers
{
    uint8_t A;    
    uint8_t B;    
    uint8_t C;
    uint8_t D;    
    uint8_t E;    
    uint8_t H;    
    uint8_t L;    

    uint16_t SP;    
    uint16_t PC;    
};

typedef struct Registers reg;

struct Machine
{
    flags *status_flags;
    reg *registers;
    uint8_t *RAM;
};

typedef struct Machine state8080;

//ARITHMETIC
void add_register(state8080 *state, uint8_t *r);
void add_memory(state8080 *state);
void add_immediate(state8080 *state);
void add_register_carry(state8080 *state, uint8_t *r);
void add_memory_carry(state8080 *state);
void add_immediate_carry(state8080 *state);
void sub_register(state8080 *state, uint8_t *r);
void sub_memory(state8080 *state);
void sub_immediate(state8080 *state);                  
void sub_register_borrow(state8080 *state, uint8_t *r);
void sub_memory_borrow(state8080 *state);
void sub_immediate_borrow(state8080 *state);
void inc_register(state8080 *state, uint8_t *r);
void inc_memory(state8080 *state);
void dec_register(state8080 *state, uint8_t *r);
void dec_memory(state8080 *state);
void inc_reg_pair(uint8_t *rh, uint8_t *rl);
void dec_reg_pair(uint8_t *rh, uint8_t *rl);
void add_reg_pair_HL(state8080 *state, uint8_t *rh, uint8_t *rl);

//DATA TRANSFER
void move_register(uint8_t *r1, uint8_t * r2);
void move_from_mem(state8080 *state, uint8_t *r1);
void move_to_mem(state8080 *state, uint8_t *r1);
void move_to_mem_imed(state8080 *state);
void load_reg_pair_imed(state8080 *state, uint8_t *rh, uint8_t *rl);
void load_acc_dir(state8080 *state);
void store_acc_dir(state8080 *state);
void load_HL_dir(state8080 *state);

//INIT
state8080* init_machine();
reg* init_reg();
flags* init_flag();

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

//TEST
void tests();
void print_state(state8080 *state);


#endif 
