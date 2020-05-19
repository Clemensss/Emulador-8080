#ifndef CPU_H_   /* Include guard */
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define CARRY_OFF 0 
#define CARRY_ON  1 
#define PORT_SIZE  100 

#define NO_VALUE 0
#define NO_COND 1

//addressing modes
#define REGISTER  0 
#define DIRECT    1 
#define IMMEDIATE 2 
#define REGISTER_INDIRECT 3

/*
 * Easier to read function pointer for the operations
 * the alu can operate on 
 */
typedef uint16_t (*OP_FUNC_PTR)(uint8_t, uint8_t, uint8_t);


/*
 *  Flags set by alu operations. 
 *  z = 1 if x == 0 else z = 0
 *  s = 1 if x < 0 else s = 0
 *  p = 1 if parity(x) == 1 else p = 0
 *  c = 1 if carry else c = 0
 *  z = 1 if x == 0 else z = 0
 *  ac not used for space invaders
 */
struct flags_s 
{
    uint8_t z:1;
    uint8_t p:1;
    uint8_t s:1;
    uint8_t c:1;
};

typedef struct flags_s flags;


/*
 *  This the CPU struct, it is made out of 3 register pairs
 *  1 8-bit Accumulator, a PC register and a SP register
 *  3 arrays for ROM memory, STACK, and RAM
 *  and the flags struct inside of it
 */
struct cpu_s
{
    uint8_t a;
    uint8_t b;   
    uint8_t c;   
    uint8_t d; 
    uint8_t e; 
    uint8_t h;     
    uint8_t l;     

    uint16_t sp;
    uint16_t pc;    
    
    struct flags_s *flags;
    
    uint32_t ROM_SIZE;
    uint32_t RAM_SIZE;
    uint32_t STACK_SIZE;

    uint8_t *ram;
    uint8_t *rom;
    uint8_t *stack;
    uint8_t *ports;

    uint8_t halt:1;
    uint8_t intr:1;
    uint8_t intr_opcode;
    uint8_t intr_enable:1;
};

typedef struct cpu_s cpu;

//util
flags*   init_flags       ();
void     load_rom         (char *file_name, uint8_t *buffer, uint32_t *file_size)  ;
void     memset_zero      (uint8_t *arr, uint32_t arr_size)                     ;
cpu*     init_cpu         (char *file_name, uint32_t stack_size, uint32_t ram_size);
uint16_t join             (uint8_t rh, uint8_t rl)              ;
uint16_t join_hl          (cpu *cpu)                         ;
uint8_t  get_rh           (uint16_t bytes)                     ;
uint8_t  get_rl           (uint16_t bytes)                     ;
uint16_t mem_out          (cpu *cpu, uint16_t addr)          ;
void     mem_in           (cpu *cpu, uint16_t addr, uint8_t val)  ;
uint16_t mem_check        (uint32_t bound, uint16_t addr,  char *memname, uint16_t pc);
void     stack_in         (cpu *cpu, uint16_t addr, uint8_t val);
uint8_t  stack_out        (cpu *cpu, uint16_t addr)         ;
uint8_t  get_psw          (cpu *cpu)                          ;
void     set_psw          (cpu *cpu, uint8_t psw)                ;
void     swap             (uint8_t *r1, uint8_t *r2)                ;
int      is_bit_set       (uint8_t byte, uint8_t bit)          ;
uint8_t  set_bit          (uint8_t byte, uint8_t bit)         ;
int      parity           (uint8_t byte)                           ;
void     set_flag_c       (cpu *cpu, uint16_t result)         ;
void     set_flag_s       (cpu *cpu, uint8_t result)          ;
void     set_flag_p       (cpu *cpu, uint8_t result)          ;
void     set_flag_z       (cpu *cpu, uint8_t result)          ;
void     set_flags_all    (cpu *cpu, uint16_t result)      ;
void     set_reset_flags  (cpu *cpu, uint16_t result, const uint8_t *arr_flag);
void     get_next_pc_bytes(cpu *cpu, uint8_t *byte_low, uint8_t *byte_high)  ;
uint8_t  read_port        (cpu *cpu, uint8_t port)            ;
void     write_port       (cpu *cpu, uint8_t port, uint8_t val) ;

//data transfer
void     load_word    (cpu *cpu, uint8_t *r)                 ;
void     load_word_hl (cpu *cpu, uint8_t *r)              ;
void     store_word_hl(cpu *cpu, uint8_t r)              ;
void     store_byte_hl(cpu *cpu)                         ;
void     load_rp_data (cpu *cpu, uint8_t *rh, uint8_t *rl);
void     load_sp_rp   (cpu *cpu)                            ;
void     load_a_addr  (cpu *cpu)                           ;
void     store_a_addr (cpu *cpu)                          ;
void     load_hl_addr (cpu *cpu)                          ;
void     store_hl_addr(cpu *cpu)                         ;
void     load_a_rp    (cpu *cpu, uint8_t rh, uint8_t rl)     ;
void     store_a_rp   (cpu *cpu, uint8_t rh, uint8_t rl)    ;
void     swap_hl_de   (cpu *cpu)                            ;

//arithmetic and logical
uint8_t  immediate_value  (cpu *cpu)                    ;
uint8_t  direct_value     (cpu *cpu)                       ;
uint8_t  register_indirect(cpu *cpu);

uint8_t  alu_inst         (cpu *cpu, uint8_t addr_mode, OP_FUNC_PTR operation, 
	                   uint8_t val, uint8_t add_flag, const uint8_t *set_flag_arr);

uint16_t add (uint8_t r1, uint8_t r2, uint8_t flag);
uint16_t sub (uint8_t r1, uint8_t r2, uint8_t flag);
uint16_t and (uint8_t r1, uint8_t r2, uint8_t flag);
uint16_t or  (uint8_t r1, uint8_t r2, uint8_t flag);
uint16_t xor (uint8_t r1, uint8_t r2, uint8_t flag);
uint16_t cmp (uint8_t r1, uint8_t r2, uint8_t flag);
uint16_t incr(uint8_t r1, uint8_t r2, uint8_t flag);
uint16_t decr(uint8_t r1, uint8_t r2, uint8_t flag);

void     incr_m      (cpu *cpu)                              ;
void     decr_m      (cpu *cpu)                              ;
void     incr_rp     (uint8_t *rh, uint8_t *rl)             ;
void     decr_rp     (uint8_t *rh, uint8_t *rl)             ;
void     add_rp_hl   (cpu *cpu, uint8_t rh, uint8_t rl)   ;
void     add_sp_hl   (cpu *cpu)                           ;
void     incr_sp     (cpu *cpu)                             ;
void     decr_sp     (cpu *cpu)                             ;

void     rotate_byte (cpu *cpu, uint8_t c_set, 
	             uint8_t lone_bit, uint8_t shifted);

void     rotate_left (cpu *cpu, uint8_t carry)                 ;
void     rotate_right(cpu *cpu, uint8_t carry)                ;

//branch
void     push         (cpu *cpu, uint8_t rh, uint8_t rl)               ;
void     pop         (cpu *cpu, uint8_t *rh, uint8_t *rl)              ;
void     pop_psw     (cpu *cpu)                                    ;
void     ex_hl_sp    (cpu *cpu)                                   ;
void     store_hl_sp (cpu *cpu)                                ;
void     jump        (cpu *cpu, uint8_t cond)                         ;
void     call        (cpu *cpu, uint8_t cond)                         ;
void     ret         (cpu *cpu, uint8_t cond)                          ;
void     rst_n       (cpu *cpu, uint8_t opcode)                      ;
void     jump_hl     (cpu *cpu)                                    ;
int      inst_process(cpu *cpu)                    ;

//io
void     port_input(cpu *cpu)                                 ;
void     port_output(cpu *cpu)                                ;


void debug_emu(cpu *cpu, uint8_t opcode);
void     assert(char *fun, uint32_t result, uint32_t equal_to);
void     tests(void)                                          ;

#endif
