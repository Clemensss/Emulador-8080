#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ALL_FLAGS [1,1,1,1]
#define CARRY_OFF 0 
#define CARRY_ON 1 


/*
 * Register numbers: 
 *  111       A
 *  000       B
 *  001	      C
 *  010       D
 *  011       E
 *  100       H
 *  101       L 
 */
#define A 0x06
#define B 0x00 
#define C 0x01 
#define D 0x02 
#define E 0x03 
#define H 0x04 
#define L 0x05 

#define RAM_SIZE 1000
#define STACK_SIZE 1000
const uint8_t cycle_values[] = {
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x00..0x0f
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,
	4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4,
	4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,

	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5, //0x40..0x4f
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,

	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4, //0x80..8x4f
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,

	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, //0xc0..0xcf
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11,
	11, 10, 10, 18, 17, 11, 7, 11, 11, 5, 10, 5, 17, 17, 7, 11,
	11, 10, 10, 4, 17, 11, 7, 11, 11, 5, 10, 4, 17, 17, 7, 11,
    };
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
    
    struct flags_s flags;

    uint8_t *ram;
    uint8_t *rom;
    uint8_t *stack;

    uint8_t halt:1;
    uint8_t intr:1;
};

typedef struct cpu_s cpu;
typedef uint16_t (*OP_FUNC_PTR)(uint8_t, uint8_t, uint8_t);

cpu* init_cpu()
{
    struct cpu_s* 
	cpu = (struct cpu_s*)malloc(sizeof(struct cpu_s));

    cpu->a = 0;
    cpu->b = 0;
    cpu->c = 0;
    cpu->d = 0;
    cpu->e = 0;
    cpu->h = 0;
    cpu->l = 0;
    cpu->pc = 0;
    cpu->sp = 0;

    cpu->flags = init_flags();

    cpu->ram = (uint8_t*)malloc(sizeof(uint8_t)*RAM_SIZE);
    cpu->stack = (uint8_t*)malloc(sizeof(uint8_t)*STACK_SIZE);

    for(int i = 0; i < RAM_SIZE; i++)
    {   
	cpu->ram[i] = 0;
    }

    for(int i = 0; i < STACK_SIZE; i++)
    {   
	cpu->stack[i] = 0;
    }

    return cpu;
}

flags* init_flags()
{
    struct flags_s* 
	flags = (struct flags_s*)malloc(sizeof(struct flags_s));

    flags->z = 0;
    flags->p = 0;
    flags->s = 0;
    flags->c = 0;

    return flags;
}

//-------------------- DATA TRANSFER -------------------------

/*
 * joins two bytes
 */
uint16_t join(uint8_t rh, uint8_t rl)
{    
    return  (uint16_t)rh << 8 | rl;
}

uint16_t join_hl(cpu *cpu)
{
    return join(cpu->h, cpu->l);
}

/* 
 * function pair to get either the most or least 
 * significant byte
 */
uint8_t get_rh(uint16_t bytes)
{
    return bytes >> 8;
}

uint8_t get_rl(uint16_t bytes)
{
    return bytes;
}

uint16_t mem_out(cpu *cpu, uint16_t addr)
{
    return cpu->rom[addr];
}

void mem_in(cpu *cpu, uint16_t addr, uint8_t val)
{
    cpu->ram[addr] = val;
}

void swap(uint8_t *r1, uint8_t *r2)
{
    uint8_t tmp;
    tmp = *r2;
    *r2 = *r1;
    *r1 = tmp;
}

int is_bit_set(uint8_t byte, uint8_t bit)
{
    return byte & (1 << bit);
}

uint8_t set_bit(uint8_t byte, uint8_t bit)
{
    return byte | 0x01 << bit; 
}

int parity(uint8_t byte)
{
    byte ^= byte >> 4;
    byte ^= byte >> 2;
    byte ^= byte >> 1;
    return (~byte) & 1;
}

void set_flag_c(cpu *cpu, uint16_t result)
{
    cpu->flags->c = !!(result & 0xff00);
}

void set_flag_s(cpu *cpu uint8_t result)
{
    cpu->flags->s = !!is_bit_set(result, 7);
}

void set_flag_p(cpu *cpu, uint8_t result)
{
    cpu->flags->p = parity(result);
}

void set_flag_z(cpu *cpu, uint8_t result)
{
    cpu->flags->z = !!result;
}

void set_flags_all(cpu *cpu, uint16_t result)
{
    set_flag_c(cpu, result); 
    set_flag_s(cpu, result);
    set_flag_p(cpu, result);
    set_flag_z(cpu, result);
}

// ========== data transfer ==============
//mov r <- data
void lw(cpu *cpu, uint8_t *r)
{
    *r = mem_out(cpu, ++cpu->pc);
}

//mov r <- hl
void lw_hl(cpu *cpu, uint8_t *r)
{
    *r = mem_out(cpu, join(cpu->h, cpu->l));
}

//mov (hl) <- r
void sw_hl(cpu *cpu, uint8_t r)
{
    //HL is the addrs
    mem_in(cpu, join(cpu->h, cpu->l), r);
}

//MVI M, data
void swi_hl(cpu *cpu)
{
    mem_in(cpu, 
	   join(cpu->h, cpu->l) 
	   mem_out(cpu, ++cpu->pc));
}

//LXI rp, data 16
void lw_rp(cpu *cpu, uint8_t *rh, uint8_t *rl)
{
    *rl = mem_out(cpu, ++cpu->pc);
    *rh = mem_out(cpu, ++cpu->pc);
}

void lw_rp_sp(cpu *cpu)
{
    uint8_t rl = mem_out(cpu, ++cpu->pc);
    uint8_t rh = mem_out(cpu, ++cpu->pc);
    cpu->sp =  join(rh, rl);
}

//LDA addr
void lwd_a(cpu *cpu)
{
    uint8_t byte2 = mem_out(cpu, ++cpu->pc);
    uint8_t byte3 = mem_out(cpu, ++cpu->pc);

    cpu->a = mem_out(cpu, join(byte3, byte2));
}

//STA addr
void swd_a(cpu *cpu)
{
    uint8_t byte2 = mem_out(cpu, ++cpu->pc);
    uint8_t byte3 = mem_out(cpu, ++cpu->pc);

    mem_in(cpu, join(byte3, byte2), cpu->a);
}

//LHLD addr
void lwd_hl(cpu *cpu)
{
    uint8_t byte2 = mem_out(cpu, ++cpu->pc);
    uint8_t byte3 = mem_out(cpu, ++cpu->pc);

    cpu->l = mem_out(cpu, join(byte3, byte2));
    cpu->h = mem_out(cpu, join(byte3, byte2) + 1);
}

//SHLD addr
void swd_hl(cpu *cpu)
{
    uint8_t byte2 = mem_out(cpu, ++cpu->pc);
    uint8_t byte3 = mem_out(cpu, ++cpu->pc);

    mem_in(cpu, 
	    join(byte3, byte2),
            join(cpu->h, cpu->l));
    mem_in(cpu, 
	    join(byte3, byte2) + 1,
            join(cpu->h, cpu->l));
}

//LDAX rp
void lwi_a(cpu *cpu, uint8_t rh, uint8_t rl)
{
    cpu->a = mem_out(cpu, join(rh, rl));
}


//STAX rp
void swi_a(cpu *cpu, uint8_t rh, uint8_t rl)
{
    mem_in(cpu, join(rh, rl), cpu->a);
}

//XCHG
void swap_hl_de(cpu *cpu)
{
    swap(&cpu->h, &cpu->d);
    swap(&cpu->l, &cpu->e);
}

void set_reset_flags(cpu *cpu, uint8_t *arr_flag, uint16_t result) 
{
    if(flags[0]) set_flag_z(cpu, result);
    else cpu->flags->z = 0;

    if(flags[1]) set_flag_p(cpu, result);
    else cpu->flags->p = 0;

    if(flags[2]) set_flag_s(cpu, result);
    else cpu->flags->s = 0;

    if(flags[3]) set_flag_c(cpu, result);
    else cpu->flags->c = 0;
}

// ======= arithmetic && logical ========

// ========== general ==========

/* Register function 
 *
 * Takes as argument a function that operates directly
 * general function
 * the op function take the register, 
 */ 


uint8_t op_reg(cpu *cpu, OP_FUNC_PTR operation, uint8_t value, 
		uint8_t add_flag, uint8_t *arr_flag)
{
    uint16_t result;

    result = operation(cpu->a, value, flag);
    set_flags(cpu, arr_flag);

    return result
}

/*
 * immediate function executer
 */
uint8_t op_imed(cpu, OP_FUNC_PTR operation, 
	     uint8_t add_flag, uint8_t *arr_flag)
{
    uint8_t value = mem_out(cpu, ++cpu->pc);
    return op_reg(cpu, operation, store, value, add_flag, arr_flag);
}

/*
 * direct function 
 */
uint8_t op_dir(cpu, OP_FUNC_PTR operation, 
	    uint8_t add_flag, uint8_t *arr_flag)
{
    uint8_t byte2 = mem_out(cpu, ++cpu->pc);
    uint8_t byte3 = mem_out(cpu, ++cpu->pc);
    uint8_t value = mem_out(cpu, join(byte3, byte2));

    return op_reg(cpu, operation, value, add_flag, arr_flag);

}


// ======= operations =========

uint16_t add(uint8_t r1, uint8_t r2, uint8_t flag)
{
    return r1 + r2 + flag; 
}

uint16_t sub(uint8_t r1, uint8_t r2, uint8_t flag)
{
    return r1 - r2 - flag;
}

uint16_t and(uint8_t r1, uint8_t r2, uint8_t flag)
{
    return r1 & r2;
}

uint16_t or(uint8_t r1, uint8_t r2, uint8_t flag)
{
    return r1 | r2; 
}

uint16_t xor(uint8_t r1, uint8_t r2, uint8_t flag)
{
    return r1 ^ r2;
}

uint16_t cmp(uint8_t r1, uint8_t r2, uint8_t flag)
{
    return r1 - r2;
}

uint16_t incr(uint8_t r1, uint8_t r2, uint8_t flag)
{
    return r1 + 1;
}

uint16_t decr(uint8_t r1, uint8_t r2, uint8_t flag)
{
    return r1 - 1;
}

// ====== special cases ====
void incr_m(cpu *cpu)
{
    uint8_t next_byte = mem_out(cpu, ++cpu->pc);
    uint16_t result = mem_out(cpu, next_byte);

    set_flags_all(cpu, ++result);

    mem_in(cpu, next_byte, result);
}

//DCR M
void decr_m(cpu *cpu)
{
    uint8_t addr = mem_out(cpu, ++cpu->pc);
    uint16_t result = mem_out(cpu, next_byte);

    set_flags_all(cpu, --result);

    mem_in(cpu, next_byte, result);
}

//INX rp
void incr_rp(uint8_t *rh, uint8_t *rl)
{
    uint16_t result = join(*rh, *rl) + 1;
    *rh = get_rh(result);
    *rl = get_rl(result);
}

//DCX rp
void decr_rp(uint8_t *rh, uint8_t *rl)
{
    uint16_t result = join(*rh, *rl) - 1;
    *rh = get_rh(result);
    *rl = get_rl(result);
}

//DAD rp
void add_rp_hl(cpu *cpu, uint8_t rh, uint8_t rl)
{
    uint32_t result = join(cpu->h, cpu->l) + join(rh, rl);
    cpu->flags->c = result & 0xffff0000;

    cpu->h = get_rh(result);
    cpu->j = get_rl(result);
}

//general rotate byte
void rotate_byte(cpu *cpu, uint8_t cy_set, uint8_t lone_bit, uint8_t shifted)
{
    uint8_t result = (lone_bit | shifted);    
    cpu->flags->c = !(0 == cy_set);    
    state->registers->A = result;
}

//RLC
void rotate_left(cpu *cpu, uint8_t carry)
{
    uint8_t lone_bit = (cpu->a >> 7);
    if(carry){lone_bit = cpu->flags->c;}

    rotate_byte(state, (cpu->a >> 7), lone_bit, (cpu->a << 1));
}

//RRC 
void rotate_right(cpu *cpu)
{
    uint8_t lone_bit = (cpu->a << 7);
    if(carry){lone_bit = (cpu->flags->c << 7);}

    rotate_byte(state, (cpu->a << 7), lone_bit, (cpu->a >> 1));
}

int inst_process(cpu *cpu, int opcode)
{
    //uint8_t opcode = mem_out(cpu, cpu-pc);

    /*if(state->interrupt) 
    {
	opcode = state->inter_opcode;
	state->interrupt = 0;
    }*/
    switch(opcode)
    {
	case 0x76: cpu->halt = 1; break; //    "HLT	"); break;

	case 0x03: incr_rp(&cpu->b, &cpu->c); break; //    "INX B	"); break;
	case 0x0b: decr_rp(&cpu->b, &cpu->c); break; //    "DCX B	"); break;

	case 0x04: cpu->b = op_dir(cpu, incr, NO_VALUE, CARRY_OFF, ALL_FLAGS); break; //    "INR B	"); break;
	case 0x05: cpu->b = op_dir(cpu, incr, NO_VALUE, CARRY_OFF, ALL_FLAGS); break; //    "DCR B	"); break;

	case 0x09: add_rp_hl(cpu, cpu->b, cpu->c); break; //    "DAD B	"); break;

	case 0x07: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RLC	"); break;
	case 0x0f: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RRC	"); break;
	case 0x1f: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RAR	"); break;
	case 0x17: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RAL	"); break;

	case 0x08: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "-		"); break;

	case 0x0c: cpu->c = op_dir(cpu, incr, NO_VALUE, CARRY_OFF, ALL_FLAGS); break; //    "INR C	"); break;
	case 0x0d: cpu->c = op_dir(cpu, incr, NO_VALUE, CARRY_OFF, ALL_FLAGS); break; //    "DCR C	"); break;

	case 0x10: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "-		"); break;
	case 0x13: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "INX D	"); break;
	case 0x14: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "INR D	"); break;
	case 0x15: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "DCR D	"); break;
	case 0x18: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "-		"); break;
	case 0x19: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "DAD D	"); break;
	case 0x1b: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "DCX D	"); break;
	case 0x1c: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "INR E	"); break;
	case 0x1d: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "DCR E	"); break;
	case 0x20: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "-		"); break;

	case 0x23: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "INX H	"); break;
	case 0x24: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "INR H	"); break;
	case 0x25: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "DCR H	"); break;


	case 0x27: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "DAA	"); break;
	case 0x28: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "-		"); break;
	case 0x29: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "DAD H	"); break;

	case 0x2b: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "DCX H	"); break;
	case 0x2c: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "INR L	"); break;
	case 0x2d: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "DCR L	"); break;


	case 0x30: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "-		"); break;


	case 0x33: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "INX SP	"); break;
	case 0x34: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "INR M	"); break;
	case 0x35: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "DCR M	"); break;

	case 0x38: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "-		"); break;

	case 0x39: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "DAD SP	"); break;
	case 0x3b: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "DCX SP	"); break;
	case 0x3c: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "INR A	"); break;
	case 0x3d: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "DCR A	"); break;

	case 0x2f: cpu->a = ~cpu->a; break; //    "CMA	"); break;
	case 0x37: set_flag_c(cpu); break; //    "STC	"); break;
	case 0x3f: cpu->flags->c = ~cpu->flags->c; break; //    "CMC	"); break;

	// ======== data transfer =======
	case 0x22: swd_hl(cpu); break; //    "SHLD adr	"); break;
	case 0x2a: lwd_hl(cpu); break; //    "LHLD adr	"); break;
	case 0x3a: lwi_a(cpu) ; break; //    "LDA adr	"); break;
	case 0x32: sw_a(cpu)  ; break; //    "STA adr	"); break;

	case 0x01: lw_rp(cpu, &cpu->b, &cpo->c); break; //    "LXI B,D16	"); break;
	case 0x11: lw_rp(cpu, &cpu->d, &cpo->e); break; //    "LXI C,D16	"); break;
	case 0x21: lw_rp(cpu, &cpu->h, &cpo->l); break; //    "LXI H,D16	"); break;
	case 0x31: lw_rp_sp(cpu);                break; //    "LXI SP,D16	"); break;

	case 0x02: swi_a(cpu, cpu->b, cpu->c); break; //    "STAX B	"); break;
	case 0x12: swi_a(cpu, cpu->d, cpu->e); break; //    "STAX D	"); break;

	case 0x36: swi_hl(cpu);     break; //    "MVI M,D8	"); break;
	case 0x06: lw(cpu, &cpu->b); break; //    "MVI B, D8	"); break;
	case 0x0e: lw(cpu, &cpu->c); break; //    "MVI C, D8	"); break;
	case 0x16: lw(cpu, &cpu->d); break; //    "MVI D, D8	"); break;
	case 0x1e: lw(cpu, &cpu->e); break; //    "MVI E, D8	"); break;
	case 0x3e: lw(cpu, &cpu->a); break; //    "MVI A,D8	"); break;
	case 0x16: lw(cpu, &cpu->h); break; //    "MVI H, D8	"); break;
	case 0x2e: lw(cpu, &cpu->l); break; //    "MVI L, D8	"); break;

	case 0xeb: swap_hl_de(cpu); break; //    "XCHG	"); break;    

	case 0x0a: lwi_a(cpu, cpu->b, cpu->c); break; //    "LDAX B	"); break;
	case 0x1a: lwi_a(cpu, cpu->d, cpu->e); break; //    "LDAX D	"); break;

	case 0x46: lw_hl(cpu, &cpu->b);break; //    "MOV B,M	"); break;
	case 0x4e: lw_hl(cpu, &cpu->c);break; //    "MOV C,M	"); break;
	case 0x56: lw_hl(cpu, &cpu->d);break; //    "MOV D,M	"); break;
	case 0x5e: lw_hl(cpu, &cpu->e);break; //    "MOV E,M	"); break;
	case 0x66: lw_hl(cpu, &cpu->h);break; //    "MOV H,M	"); break;
	case 0x6e: lw_hl(cpu, &cpu->l); break; //    "MOV L,M	"); break;
	case 0x7e: lw_hl(cpu, &cpu->a); break; //    "MOV A,M	"); break;

	case 0x40: cpu->b = cpu->b; break; //    "MOV B,B	"); break;
	case 0x41: cpu->b = cpu->c; break; //    "MOV B,C	"); break;
	case 0x42: cpu->b = cpu->d; break; //    "MOV B,D	"); break;
	case 0x43: cpu->b = cpu->e; break; //    "MOV B,E	"); break;
	case 0x44: cpu->b = cpu->h; break; //    "MOV B,H	"); break;
	case 0x45: cpu->b = cpu->l; break; //    "MOV B,L	"); break;
	case 0x47: cpu->b = cpu->a; break; //    "MOV B,A	"); break;

	case 0x48: cpu->c = cpu->b; break;  //    "MOV C,B	"); break;
	case 0x49: cpu->c = cpu->c; break;  //    "MOV C,C	"); break;
	case 0x4a: cpu->c = cpu->d; break;  //    "MOV C,D	"); break;
	case 0x4b: cpu->c = cpu->e; break;  //    "MOV C,E	"); break;
	case 0x4c: cpu->c = cpu->h; break;  //    "MOV C,H	"); break;
	case 0x4d: cpu->c = cpu->l; break;  //    "MOV C,L	"); break;
	case 0x4f: cpu->c = cpu->a; break;  //    "MOV C,A	"); break;
		   
	case 0x50: cpu->d = cpu->b; break;  //    "MOV D,B	"); break;
	case 0x51: cpu->d = cpu->c; break;  //    "MOV D,C	"); break;
	case 0x52: cpu->d = cpu->d; break;  //    "MOV D,D	"); break;
	case 0x53: cpu->d = cpu->e; break;  //    "MOV D,E	"); break;
	case 0x54: cpu->d = cpu->h; break;  //    "MOV D,H	"); break;
	case 0x55: cpu->d = cpu->l; break;  //    "MOV D,L	"); break;
	case 0x57: cpu->d = cpu->a; break;  //    "MOV D,A	"); break;

	case 0x58: cpu->e = cpu->b; break;  //    "MOV E,B	"); break;
	case 0x59: cpu->e = cpu->c; break;  //    "MOV E,C	"); break;
	case 0x5a: cpu->e = cpu->d; break;  //    "MOV E,D	"); break;
	case 0x5b: cpu->e = cpu->e; break;  //    "MOV E,E	"); break;
	case 0x5c: cpu->e = cpu->h; break;  //    "MOV E,H	"); break;
	case 0x5d: cpu->e = cpu->l; break;  //    "MOV E,L	"); break;
	case 0x5f: cpu->e = cpu->a; break;  //    "MOV E,A	"); break;

	case 0x60: cpu->h = cpu->b; break;  //    "MOV H,B	"); break;
	case 0x61: cpu->h = cpu->c; break;  //    "MOV H,C	"); break;
	case 0x62: cpu->h = cpu->d; break;  //    "MOV H,D	"); break;
	case 0x63: cpu->h = cpu->e; break;  //    "MOV H,E	"); break;
	case 0x64: cpu->h = cpu->h; break;  //    "MOV H,H	"); break;
	case 0x65: cpu->h = cpu->l; break;  //    "MOV H,L	"); break;
	case 0x67: cpu->h = cpu->a; break;  //    "MOV H,A	"); break;

	case 0x68: cpu->l = cpu->b; break;  //    "MOV L,B	"); break;
	case 0x69: cpu->l = cpu->c; break;  //    "MOV L,C	"); break;
	case 0x6a: cpu->l = cpu->d; break;  //    "MOV L,D	"); break;
	case 0x6b: cpu->l = cpu->e; break;  //    "MOV L,E	"); break;
	case 0x6c: cpu->l = cpu->h; break;  //    "MOV L,H	"); break;
	case 0x6d: cpu->l = cpu->l; break;  //    "MOV L,L	"); break;
	case 0x6f: cpu->l = cpu->a; break;  //    "MOV L,A	"); break;

	case 0x70: sw_hl(cpu, cpu->b); break; //    "MOV M,B	"); break;
	case 0x71: sw_hl(cpu, cpu->c); break; //    "MOV M,C	"); break;
	case 0x72: sw_hl(cpu, cpu->d); break; //    "MOV M,D	"); break;
	case 0x73: sw_hl(cpu, cpu->e); break; //    "MOV M,E	"); break;
	case 0x74: sw_hl(cpu, cpu->h); break; //    "MOV M,H	"); break;
	case 0x75: sw_hl(cpu, cpu->l); break; //    "MOV M,L	"); break;
	case 0x77: sw_hl(cpu, cpu->a); break; //    "MOV M,A	"); break;

	case 0x78: cpu->a = cpu->b; break;  //    "MOV A,B	"); break;
	case 0x79: cpu->a = cpu->c; break;  //    "MOV A,C	"); break;
	case 0x7a: cpu->a = cpu->d; break;  //    "MOV A,D	"); break;
	case 0x7b: cpu->a = cpu->e; break;  //    "MOV A,E	"); break;
	case 0x7c: cpu->a = cpu->h; break;  //    "MOV A,H	"); break;
	case 0x7d: cpu->a = cpu->l; break;  //    "MOV A,L	"); break;
	case 0x7f: cpu->a = cpu->a; break;  //    "MOV A,A	"); break;

	// ======== arithmetic =======
	case 0x8e: cpu->a op_dir(cpu, add, CARRY_ON, ALL_FLAGS); break; //    "ADC M	
	case 0x86: cpu->a op_dir(cpu, add, CARRY_OFF, ALL_FLAGS); break; //    "ADD M	

	case 0x80: cpu->a = op_reg(cpu, add, cpu->b, CARRY_OFF, ALL_FLAGS); break; //    "ADD B	"
	case 0x81: cpu->a = op_reg(cpu, add, cpu->c, CARRY_OFF, ALL_FLAGS); break; //    "ADD C	"
	case 0x82: cpu->a = op_reg(cpu, add, cpu->d, CARRY_OFF, ALL_FLAGS); break; //    "ADD D	"
	case 0x83: cpu->a = op_reg(cpu, add, cpu->e, CARRY_OFF, ALL_FLAGS); break; //    "ADD E	"
	case 0x84: cpu->a = op_reg(cpu, add, cpu->h, CARRY_OFF, ALL_FLAGS); break; //    "ADD H	"
	case 0x85: cpu->a = op_reg(cpu, add, cpu->l, CARRY_OFF, ALL_FLAGS); break; //    "ADD L	"
	case 0x87: cpu->a = op_reg(cpu, add, cpu->a, CARRY_OFF, ALL_FLAGS); break; //    "ADD A	"

	case 0x88: cpu->a = op_reg(cpu, add, cpu->b, CARRY_ON, ALL_FLAGS); break; //    "ADC B	"
	case 0x89: cpu->a = op_reg(cpu, add, cpu->c, CARRY_ON, ALL_FLAGS); break; //    "ADC C	"
	case 0x8a: cpu->a = op_reg(cpu, add, cpu->d, CARRY_ON, ALL_FLAGS); break; //    "ADC D	"
	case 0x8b: cpu->a = op_reg(cpu, add, cpu->e, CARRY_ON, ALL_FLAGS); break; //    "ADC E	"
	case 0x8c: cpu->a = op_reg(cpu, add, cpu->h, CARRY_ON, ALL_FLAGS); break; //    "ADC H	"
	case 0x8d: cpu->a = op_reg(cpu, add, cpu->l, CARRY_ON, ALL_FLAGS); break; //    "ADC L	"
	case 0x8f: cpu->a = op_reg(cpu, add, cpu->a, CARRY_ON, ALL_FLAGS); break; //    "ADC A	"

	case 0x96: cpu->a = op_dir(cpu, sub, CARRY_OFF, ALL_FLAGS); break; //    "SUB M	
	case 0x9e: cpu->a = op_dir(cpu, sub, CARRY_ON, ALL_FLAGS); break; //    "SBB M	

	case 0x90: cpu->a = op_reg(cpu, sub, cpu->b, CARRY_OFF, ALL_FLAGS); break; //    "SUB B	
	case 0x91: cpu->a = op_reg(cpu, sub, cpu->c, CARRY_OFF, ALL_FLAGS); break; //    "SUB C	
	case 0x92: cpu->a = op_reg(cpu, sub, cpu->d, CARRY_OFF, ALL_FLAGS); break; //    "SUB D	
	case 0x93: cpu->a = op_reg(cpu, sub, cpu->e, CARRY_OFF, ALL_FLAGS); break; //    "SUB E	
	case 0x94: cpu->a = op_reg(cpu, sub, cpu->h, CARRY_OFF, ALL_FLAGS); break; //    "SUB H	
	case 0x95: cpu->a = op_reg(cpu, sub, cpu->l, CARRY_OFF, ALL_FLAGS); break; //    "SUB L	
	case 0x97: cpu->a = op_reg(cpu, sub, cpu->a, CARRY_OFF, ALL_FLAGS); break; //    "SUB A	

	case 0x98: cpu->a = op_reg(cpu, sub, cpu->b, CARRY_ON, ALL_FLAGS); break; //    "SBB B	
	case 0x99: cpu->a = op_reg(cpu, sub, cpu->c, CARRY_ON, ALL_FLAGS); break; //    "SBB C	
	case 0x9a: cpu->a = op_reg(cpu, sub, cpu->d, CARRY_ON, ALL_FLAGS); break; //    "SBB D	
	case 0x9b: cpu->a = op_reg(cpu, sub, cpu->e, CARRY_ON, ALL_FLAGS); break; //    "SBB E	
	case 0x9c: cpu->a = op_reg(cpu, sub, cpu->h, CARRY_ON, ALL_FLAGS); break; //    "SBB H	
	case 0x9d: cpu->a = op_reg(cpu, sub, cpu->l, CARRY_ON, ALL_FLAGS); break; //    "SBB L	
	case 0x9f: cpu->a = op_reg(cpu, sub, cpu->a, CARRY_ON, ALL_FLAGS); break; //    "SBB A	

	case 0xa6: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ANA M	"); break;

	case 0xa0: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ANA B	"); break;
	case 0xa1: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ANA C	"); break;
	case 0xa2: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ANA D	"); break;
	case 0xa3: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ANA E	"); break;
	case 0xa4: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ANA H	"); break;
	case 0xa5: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ANA L	"); break;
	case 0xa7: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ANA A	"); break;

	case 0xae: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "XRA M	"); break;

	case 0xa8: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "XRA B	"); break;
	case 0xa9: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "XRA C	"); break;
	case 0xaa: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "XRA D	"); break;
	case 0xab: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "XRA E	"); break;
	case 0xac: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "XRA H	"); break;
	case 0xad: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "XRA L	"); break;
	case 0xaf: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "XRA A	"); break;

	case 0xb6: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ORA M	"); break;

	case 0xb0: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ORA B	"); break;
	case 0xb1: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ORA C	"); break;
	case 0xb2: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ORA D	"); break;
	case 0xb3: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ORA E	"); break;
	case 0xb4: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ORA H	"); break;
	case 0xb5: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ORA L	"); break;
	case 0xb7: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ORA A	"); break;

	case 0xbe: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "CMP M	"); break;

	case 0xb8: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "CMP B	"); break;
	case 0xb9: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "CMP C	"); break;
	case 0xba: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "CMP D	"); break;
	case 0xbb: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "CMP E	"); break;
	case 0xbc: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "CMP H	"); break;
	case 0xbd: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "CMP L	"); break;
	case 0xbf: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "CMP A	"); break;


		   //
	case 0xc0: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RNZ	"); break;

	case 0xc1: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "POP B	"); break;
	case 0xc5: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "PUSH B	"); break;

	case 0xc2: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "JNZ adr	"); break;
	case 0xc3: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "JMP adr	"); break;
	case 0xc4: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "CNZ adr	"); break;

	case 0xc6: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ADI D8	"); break;
	case 0xc7: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RST 0	"); break;
	case 0xc8: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RZ	1	"); break;
	case 0xc9: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RET	"); break;
	case 0xca: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "JZ adr	"); break;
	case 0xcb: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "-		"); break;
	case 0xcc: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "CZ adr	"); break;
	case 0xcd: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "CALL adr	"); break;
	case 0xce: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ACI D8	"); break;
	case 0xcf: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RST 1	"); break;
	case 0xd0: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RNC	"); break;
	case 0xd1: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "POP D	"); break;
	case 0xd2: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "JNC adr	"); break;
	case 0xd3: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "OUT D8	"); break;
	case 0xd4: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "CNC adr	"); break;
	case 0xd5: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "PUSH D	"); break;
	case 0xd6: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "SUI D8	"); break;
	case 0xd7: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RST 2	"); break;
	case 0xd8: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RC	1	"); break;
	case 0xd9: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "-		"); break;
	case 0xda: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "JC adr	"); break;
	case 0xdb: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "IN D8	"); break;
	case 0xdc: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "CC adr	"); break;
	case 0xdd: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "-		"); break;
	case 0xde: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "SBI D8	"); break;
	case 0xdf: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RST 3	"); break;
	case 0xe0: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RPO	"); break;
	case 0xe1: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "POP H	"); break;
	case 0xe2: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "JPO adr	"); break;
	case 0xe3: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "XTHL	"); break;
	case 0xe4: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "CPO adr	"); break;
	case 0xe5: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "PUSH H	"); break;
	case 0xe6: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ANI D8	"); break;
	case 0xe7: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RST 4	"); break;
	case 0xe8: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RPE	"); break;
	case 0xe9: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "PCHL	"); break;    
	case 0xea: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "JPE adr	"); break;    
	case 0xec: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "CPE adr	"); break;    
	case 0xed: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "-		"); break;
	case 0xee: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "XRI D8	"); break;    
	case 0xef: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RST 5	"); break;    
	case 0xf0: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RP	1	"); break;
	case 0xf1: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "POP PSW	"); break;    
	case 0xf2: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "JP adr	"); break;    
	case 0xf3: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "DI	1	"); break;
	case 0xf4: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "CP adr	"); break;    
	case 0xf5: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "PUSH PSW	"); break;
	case 0xf6: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "ORI D8	"); break;    
	case 0xf7: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RST 6	"); break;    
	case 0xf8: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RM	1	"); break;
	case 0xf9: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "SPHL	"); break;    
	case 0xfa: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "JM adr	"); break;    
	case 0xfb: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "EI	1	"); break;
	case 0xfc: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "CM adr	"); break;    
	case 0xfd: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "-		"); break;
	case 0xfe: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "CPI D8	"); break;    
	case 0xff: printf("Missing instruction opcode: %#04x\n", opcode); break; //    "RST 7	"); break;
	default: printf("%#04x not found\n", opcode);
    }
}


int main()
{
    cpu *cpu = init_cpu();
    for(int i = 0x00; i <= 0xff; i++)
    {
	inst_process(cpu, i);
    }
}









