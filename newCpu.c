#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


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

const int cycle_values[] = {
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
 *  This the CPU struct, it is made out of 3 register pairs
 *  1 8-bit Accumulator, a PC register and a SP register
 *  3 arrays for ROM memory, STACK, and RAM
 *
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

    uint8_t *ram;
    uint8_t *rom;
    uint16_t *stack;
};

typedef struct cpu_s cpu;
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

int main()
{

    return 0;
}

cpu* init_cpu()
{
    cpu* cpu = (cpu*)malloc(sizeof(cpu));

    cpu->a = 0;
    cpu->b = 0;
    cpu->c = 0;
    cpu->d = 0;
    cpu->e = 0;
    cpu->h = 0;
    cpu->l = 0;
    cpu->pc = 0;
    cpu->sp = 0;
     
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
    flags* flags = (flags*)malloc(sizeof(flags));

    flags->z:1 = 0;
    flags->p:1 = 0;
    flags->s:1 = 0;
    flags->c:1 = 0;

    return flags;
}

//-------------------- DATA TRANSFER -------------------------

/*
 * joins two bytes
 */
uint16_t join_hl(cpu *cpu)
{
    return join(cpu->h, cpu->l);
}

uint16_t join(uint8_t rh, uint8_t rl)
{    
    return  (uint16_t)rh << 8 | rl;
}

/* 
 * function pair to get either the most or least 
 * significant byte
 */
uint8_t get_rh(uint16_t bytes)
{
    return joint >> 8;
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
    return cpu->ram[addr];
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

void set_flag_c(flags *flags, uint16_t result)
{
    flags->c = result & 0xff00;
}

void set_flag_s(flags *flags, uint8_t result)
{
    flags->s = !!is_bit_set(result, 7);
}

void set_flag_p(flags *flags, uint8_t result)
{
    flags->p = parity(result);
}

void set_flag_z(flags *flags, uint8_t result)
{
    flags->z = !!result;
}

void set_flags_all(flags *flags, uint16_t result)
{
    set_flag_c(flags, result); 
    set_flag_s(flags, result);
    set_flag_p(flags, result);
    set_flag_z(flags, result);
}

// ========== data transfer ==============

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

//mov (addr) <- r
void lw(cpu *cpu, uint8_t *r)
{
    *r = mem_out(cpu, join(cpu->h, cpu->l));
}

void lw(cpu *cpu)
{
    uint8_t byte1 = cpu->rom[++pc];
    uint8_t byte2 = cpu->rom[++pc];

    //HL is the addrs
    cpu->ram[join(cpu->h, cpu->l)] = datamem_out(cpu, join());

    uint16_t addr = get_HL_addr(state);

    //if(rom_lock(state, addr, 0x1fff)) return;
}

//LXP rp, data 16
void load_reg_pair_imed(state8080 *state, uint8_t *rh, uint8_t *rl)
{
    state->registers->PC++;
    uint8_t byte2 = get_PC_data(state);

    state->registers->PC++;
    uint8_t byte3 = get_PC_data(state);

    *rh = byte3;
    *rl = byte2;
}

//LDA addr
void load_acc_dir(state8080 *state)
{
    state->registers->PC++;
    uint8_t byte2 = get_PC_data(state);

    state->registers->PC++;
    uint8_t byte3 = get_PC_data(state);

    uint16_t addr = joint(byte3, byte2);

    //if(rom_lock(state, addr,0x1fff)) return;
    state->registers->A = state->RAM[addr];
}

//STA addr
void store_acc_dir(state8080 *state)
{
    state->registers->PC++;
    uint8_t byte2 = get_PC_data(state);

    state->registers->PC++;
    uint8_t byte3 = get_PC_data(state);

    uint16_t addr = joint(byte3, byte2);

    //if(rom_lock(state, addr, 0x1fff)) return;
    state->RAM[addr] = state->registers->A;
}

//LHLD addr
void load_HL_dir(state8080 *state)
{
    state->registers->PC++;
    uint8_t byte2 = get_PC_data(state);

    state->registers->PC++;
    uint8_t byte3 = get_PC_data(state);

    uint16_t addr = joint(byte3, byte2);

    state->registers->L = state->RAM[addr];
    state->registers->H = state->RAM[addr+1];
}

//SHLD addr
void store_HL_dir(state8080 *state)
{
    state->registers->PC++;
    uint8_t byte2 = get_PC_data(state);

    state->registers->PC++;
    uint8_t byte3 = get_PC_data(state);

    uint16_t addr = joint(byte3, byte2);

    if(rom_lock(state, addr)) return;

    state->RAM[addr] =  state->registers->L; 
    state->RAM[(addr+1)] =  state->registers->H; 
}

//LDAX rp
void load_acc_indir(state8080 *state, uint8_t *rh, uint8_t *rl)
{
    uint16_t addr = joint(*rh, *rl);

    //if(rom_lock(state, addr)) return;
    state->registers->A = state->RAM[addr];
}

//STAX rp
void store_acc_indir(state8080 *state, uint8_t *rh, uint8_t *rl)
{
    uint16_t addr = joint(*rh, *rl);

    if(rom_lock(state, addr)) return;
    state->RAM[addr] = state->registers->A;
}

//XCHG
void exchange_HL_DE(state8080 *state)
{
    swap(&state->registers->H, &state->registers->D);
    swap(&state->registers->L, &state->registers->E);
}















