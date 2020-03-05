#ifndef EMULATOR_H_   /* Include guard */
#define EMULATOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define RAM_SIZE 65536 
#define REG state->registers


struct ConditionFlags
{
    uint8_t Z:1;
    uint8_t S:1;
    uint8_t P:1;
    uint8_t CY:1;
    uint8_t AC:1;
    uint8_t jmp:1;
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

/*
    interrrupt will be handled by 3 var
    interrupt tells the cpu there was an 
    interrupt
    
    inter opcode is the opcode
    
*/
struct Machine
{
    flags *status_flags;
    reg *registers;

    uint8_t halt:1;

    uint8_t inter_ind:1;
    uint16_t inter_stack;
    uint8_t interrupt:1;
    uint8_t inter_opcode;

    uint8_t *RAM;
};

typedef struct Machine state8080;

//PORTS
struct Input
{
    uint8_t port0;
    uint8_t port1;
    uint8_t port2;
    uint8_t port3;
};

typedef struct Input i_port;

struct Output
{
    uint8_t port2;
    uint8_t port3;
    uint8_t port4;
    uint8_t port5;
    uint8_t port6;
};
typedef struct Output o_port;

struct Port
{
    i_port *input;
    o_port *output;
};

typedef struct Port port;


//STACK
void push(state8080 *state, uint8_t rh, uint8_t rl);
void push_psw(state8080 *state);
void pop(state8080 *state, uint8_t *rh, uint8_t *rl);
void pop_psw(state8080 *state);
void exchange_HL_st(state8080 *state);
void move_HL_SP(state8080 *state);

//IO
void input(state8080 *state, port *p);
void output(state8080 *state, port *p);
void enable_inter(state8080 *state);
void disable_inter(state8080 *state);
void halt(state8080 *state);

//BRANCH
void jump(state8080 *state);
void cond_jump(state8080 *state, uint8_t flag)  ;
void call(state8080 *state);
void cond_call(state8080 *state, uint8_t flag);
void ret_op(state8080 *state);
void cond_ret_op(state8080 *state, uint8_t flag);
void restart(state8080 *state, uint8_t opcode);
void jump_HL_dir(state8080 *state);

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

//DATA TRANSFER
void move_register(uint8_t *r1, uint8_t * r2);
void move_from_mem(state8080 *state, uint8_t *r1);
void move_to_mem(state8080 *state, uint8_t *r1);
void move_to_mem_imed(state8080 *state);
void move_immediate(state8080 *state, uint8_t *r);
void load_reg_pair_imed(state8080 *state, uint8_t *rh, uint8_t *rl);
void load_acc_dir(state8080 *state);
void store_acc_dir(state8080 *state);
void load_HL_dir(state8080 *state);
void store_HL_dir(state8080 *state);
void load_acc_indir(state8080 *state, uint8_t *rh, uint8_t *rl);
void store_acc_indir(state8080 *state, uint8_t *rh, uint8_t *rl);
void exchange_HL_DE(state8080 *state);

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
uint8_t set_bit(uint8_t byte, uint8_t bit);
int rom_lock(state8080 *state, uint16_t var1, uint16_t var2);

//TEST
void tests();
void print_state(state8080 *state);

uint16_t write_o_port(port *o, uint8_t port, uint8_t data);
uint8_t read_i_port(port *i, uint8_t port);
#endif 
