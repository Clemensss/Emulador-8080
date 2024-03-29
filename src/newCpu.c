#include "newCpu.h"

//set all flags
const uint8_t ALL_FLAGS[]         =  {1,1,1,1,1};
const uint8_t ALL_CY_CLEARED[]    =  {1,1,1,-1, 1};
const uint8_t ALL_CY_AC_CLEARED[] =  {1,1,1,-1,-1};
const uint8_t ALL_BUT_CY[]        =  {1,1,1,0,1};

const uint8_t instruction_cycle[] = {
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


flags* init_flags()
{
    struct flags_t* flags = (struct flags_t*)malloc(sizeof(struct flags_t));

    flags->z  = 0;
    flags->p  = 0;
    flags->s  = 0;
    flags->c  = 0;
    flags->ac = 0;

    return flags;
}

/*
 * Reads the rom into a uint8_t arr buffer
 */
void load_rom(char *file_name, uint8_t **memory, uint32_t *file_size)
{
    FILE *fp;

    fp = fopen(file_name, "rb");     // Open the file in binary mode
    fseek(fp, 0, SEEK_END);          // Jump to the end of the file
    *file_size = ftell(fp);             // Get the current byte offset in the file
    rewind(fp);                      // Jump back to the beginning of the file

    *memory = (uint8_t *)malloc((*file_size)*sizeof(uint8_t)); 
    fread(*memory, *file_size, 1, fp); // Read in the entire file
    fclose(fp); 
}

void memset_zero(uint8_t *arr, uint32_t arr_size)
{
    for(int i = 0; i < arr_size; i++) arr[i] = 0;
}

/*
 * Initializes a cpu_s struct and loads a rom into memory with a defined
 * stack and ram size 
 */
cpu* init_cpu(char *file_name, uint32_t ram_size)
{
    struct cpu_t* cpu = (struct cpu_t*)malloc(sizeof(struct cpu_t));
    
    cpu->a  = 0;
    cpu->b  = 0;
    cpu->c  = 0;
    cpu->d  = 0;
    cpu->e  = 0;
    cpu->h  = 0;
    cpu->l  = 0;
    cpu->pc = 0;
    cpu->sp = 0;
    
    cpu->halt        = 0;
    cpu->intr        = 0;
    cpu->intr_opcode = 0;
    cpu->intr_enable = 0;

    cpu->RAM_SIZE = ram_size;

    cpu->flags = init_flags();

    load_rom(file_name, &cpu->rom, &cpu->ROM_SIZE);

    cpu->ram   = (uint8_t*)malloc(cpu->RAM_SIZE * sizeof(uint8_t));
    cpu->ports = (uint16_t*)malloc(PORT_SIZE *     sizeof(uint16_t));
    
    memset_zero(cpu->ram, cpu->RAM_SIZE);
    for(int i = 0; i < PORT_SIZE; i++)
		cpu->ports[i] = 0;

    return cpu;
}

//-------------------- DATA TRANSFER -------------------------

/*
 * joins two bytes
 */
uint16_t join(uint8_t rh, uint8_t rl)
{    
    return (uint16_t) rh << 8 | rl;
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

//Controls virtualization of memory
/*
 * If the addr is smaller than ROM_SIZE then return that 
 * Address in the rom arr, else, do the calculations
 * nescessary to map the address to an index of the ram arr 
 */

//void mapped_addr()
uint8_t fetch_inst(cpu *cpu, uint16_t addr)
{
    if(addr < cpu->ROM_SIZE && addr >= 0)
	return cpu->rom[addr];

    else if(addr >= cpu->ROM_SIZE)
	printf("ERROR Address %#04x out of bound not in ROM \npc = %#04x\n", addr, cpu->pc);
    else if(addr < 0) 
	printf("ERROR Address invalid not in ROM\npc = %#04x\n", cpu->pc);


    exit(1);
}

uint8_t* mem_ptr_out(cpu *cpu, uint16_t addr)
{
    uint16_t virtual_addr = addr - cpu->ROM_SIZE;
    
    if(addr < cpu->ROM_SIZE && addr >= 0)
	return &cpu->rom[addr];

    if(addr < (cpu->ROM_SIZE + cpu->RAM_SIZE) && addr >= cpu->ROM_SIZE)
	return &cpu->ram[virtual_addr];

    else if(addr > (cpu->ROM_SIZE + cpu->RAM_SIZE) || addr < 0)
	printf("ERROR Address %#04x out of bound not in RAM or ROM\npc = %#04x\n", addr, cpu->pc);

    exit(1);
}

uint8_t mem_out(cpu *cpu, uint16_t addr)
{
    return *(mem_ptr_out(cpu, addr));
}

void mem_in(cpu *cpu, uint16_t addr, uint8_t val)
{
    uint16_t virtual_addr = addr - cpu->ROM_SIZE;

    if(addr < cpu->ROM_SIZE) 
    {
	cpu->rom[addr] = val;
	//printf("ERROR Address %#04x; Trying to write to rom; PC = %#04x\n", addr, cpu->pc);
	//exit(1);
    }

    else cpu->ram[virtual_addr] = val;

}

uint8_t get_psw(cpu *cpu)
{
    uint8_t psw = 0;

					   psw = set_bit(psw, 1);
    if(cpu->flags->c)  psw = set_bit(psw, 0);
    if(cpu->flags->p)  psw = set_bit(psw, 2);
    if(cpu->flags->ac) psw = set_bit(psw, 4);
    if(cpu->flags->z)  psw = set_bit(psw, 6);
    if(cpu->flags->s)  psw = set_bit(psw, 7);

    return psw;
}

void set_psw(cpu *cpu, uint8_t psw)
{
    cpu->flags->c  = !!is_bit_set(psw, 0);
    cpu->flags->p  = !!is_bit_set(psw, 2);
    cpu->flags->ac = !!is_bit_set(psw, 4);
    cpu->flags->z  = !!is_bit_set(psw, 6);
    cpu->flags->s  = !!is_bit_set(psw, 7);
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

// ======= flags ========

void set_flag_c (cpu *cpu, uint16_t result)
{  
    cpu->flags->c = !!(result & 0xff00);    
}

void set_flag_ac(cpu *cpu, uint8_t r1, uint8_t r2)
{
    uint8_t nibble_r1 = r1 & 0x0f;
    uint8_t nibble_r2 = r2 & 0x0f;
    
    uint8_t result = nibble_r1 + nibble_r2;

    cpu->flags->ac = !!(result & 0xf0);
}

void set_flag_s (cpu *cpu, uint8_t result)
{  
    cpu->flags->s = !!is_bit_set(result, 7);
}

void set_flag_p (cpu *cpu, uint8_t result) 
{  
    cpu->flags->p = parity(result);       
}

void set_flag_z (cpu *cpu, uint8_t result) 
{  
    cpu->flags->z = !!!result;               
}

/*
 * Receives and array of 1, -1 or 0, each one defines what
 * happens to the flag, either set, reset, or ignored,
 * respectively
 *
 * r1 and r2 are added solely for the ac flag
 */
void set_reset_flags(cpu *cpu, uint16_t result, uint8_t r1, uint8_t r2, 
		     const uint8_t *arr_flag)
{
    if(arr_flag[0])          set_flag_z(cpu, result);
    else if(arr_flag[0] == -1) cpu->flags->z  = 0;

    if(arr_flag[1])          set_flag_s(cpu, result);
    else if(arr_flag[1] == -1) cpu->flags->s  = 0;

    if(arr_flag[2])          set_flag_p(cpu, result);
    else if(arr_flag[2] == -1) cpu->flags->p  = 0;

    if(arr_flag[3])          set_flag_c(cpu, result);
    else if(arr_flag[3] == -1) cpu->flags->c  = 0;

    if(arr_flag[4])          set_flag_ac(cpu, r1, r2);
    else if(arr_flag[4] == -1) cpu->flags->ac = 0;
}

void get_next_pc_bytes(cpu *cpu, uint8_t *byte_low, uint8_t *byte_high)
{
    *byte_low  = mem_out(cpu, ++cpu->pc);
    *byte_high = mem_out(cpu, ++cpu->pc);
}



// ========== data transfer ==============
//mov r <- data
void load_word(cpu *cpu, uint8_t *r)
{
    *r = mem_out(cpu, ++cpu->pc);
}

//mov r <- hl
void load_word_hl(cpu *cpu, uint8_t *r)
{
    *r = mem_out(cpu, join(cpu->h, cpu->l));
}

//mov (hl) <- r
void store_word_hl(cpu *cpu, uint8_t r)
{
    //HL is the addrs
    mem_in(cpu, join(cpu->h, cpu->l), r);
}

//MVI M, data
void store_byte_hl(cpu *cpu)
{
    mem_in(cpu, 
	   join(cpu->h, cpu->l), 
	   mem_out(cpu, ++cpu->pc));
}

//LXI rp, data 16
void load_rp_data(cpu *cpu, uint8_t *rh, uint8_t *rl)
{
    get_next_pc_bytes(cpu, rl, rh);
}

void load_sp_rp(cpu *cpu)
{
    uint8_t rh, rl;
    get_next_pc_bytes(cpu, &rl, &rh);
    cpu->sp = join(rh, rl);
}

//LDA addr
void load_a_addr(cpu *cpu)
{
    uint8_t byte_h, byte_l;
    get_next_pc_bytes(cpu, &byte_l, &byte_h);

    cpu->a = mem_out(cpu, join(byte_h, byte_l));
}

//STA addr
void store_a_addr(cpu *cpu)
{
    uint8_t byte_h, byte_l;
    get_next_pc_bytes(cpu, &byte_l, &byte_h);

    mem_in(cpu, join(byte_h, byte_l), cpu->a);
}

//LHLD addr
void load_hl_addr(cpu *cpu)
{
    uint8_t byte_h, byte_l;
    get_next_pc_bytes(cpu, &byte_l, &byte_h);

    cpu->l = mem_out(cpu, join(byte_h, byte_l));
    cpu->h = mem_out(cpu, join(byte_h, byte_l) + 1);
}

//SHLD addr
void store_hl_addr(cpu *cpu)
{
    uint8_t byte_h, byte_l;
    get_next_pc_bytes(cpu, &byte_l, &byte_h);

    mem_in(cpu, 
	    join(byte_h, byte_l),
            cpu->l);
    mem_in(cpu, 
	    join(byte_h, byte_l) + 1,
            cpu->h);
}

//LDAX rp
void load_a_rp(cpu *cpu, uint8_t rh, uint8_t rl)
{
    cpu->a = mem_out(cpu, join(rh, rl));
}


//STAX rp
void store_a_rp(cpu *cpu, uint8_t rh, uint8_t rl)
{
    mem_in(cpu, join(rh, rl), cpu->a);
}

//XCHG
void swap_hl_de(cpu *cpu)
{
    swap(&cpu->h, &cpu->d);
    swap(&cpu->l, &cpu->e);
}

// ======= arithmetic && logical ========

// ========== general ==========

uint8_t immediate_value(cpu *cpu)
{
    return mem_out(cpu, ++cpu->pc);
}

uint8_t direct_value(cpu *cpu)
{
    uint8_t byte2 = mem_out(cpu, ++cpu->pc);
    uint8_t byte3 = mem_out(cpu, ++cpu->pc);

    return mem_out(cpu, join(byte3, byte2));
}

uint8_t register_indirect(cpu *cpu)
{
    return mem_out(cpu, join(cpu->h, cpu->l));
}

/* Generic function that represents the ALU
 * Receives the addressing mode, a function pointer to an operation
 * the val r2 that this operation will operate on,
 * if the operation will have an added flag to it, 
 * and an array containing the flags that should be set 
 */
uint8_t alu_inst(cpu *cpu, uint8_t addr_mode, OP_FUNC_PTR operation, 
	         uint8_t val, uint8_t c_flag, const uint8_t *arr_flag)
{
    uint16_t result;
    
    if(addr_mode == IMMEDIATE)              val = immediate_value(cpu);
    else if(addr_mode == REGISTER_INDIRECT) val = register_indirect(cpu);
    else if(addr_mode == DIRECT)            val = direct_value(cpu);

    result = operation(cpu->a, val, c_flag);
    
    //2's complement for the ac check
    if(operation == sub) val = (~val + 1);

    set_reset_flags(cpu, result, cpu->a, val, arr_flag);

    return result;
}

// ======= operations =========

uint16_t add (uint8_t a_reg, uint8_t val, uint8_t c_flag){ return a_reg + val + c_flag;}
uint16_t sub (uint8_t a_reg, uint8_t val, uint8_t c_flag){ return a_reg - val - c_flag;}
uint16_t and (uint8_t a_reg, uint8_t val, uint8_t c_flag){ return a_reg & val;         }
uint16_t or  (uint8_t a_reg, uint8_t val, uint8_t c_flag){ return a_reg | val;         }
uint16_t xor (uint8_t a_reg, uint8_t val, uint8_t c_flag){ return a_reg ^ val;         }
uint16_t cmp (uint8_t a_reg, uint8_t val, uint8_t c_flag){ return a_reg - val;         }

//uint16_t incr(uint8_t a_reg, uint8_t val, uint8_t c_flag){ return val   +   1;         }
//uint16_t decr(uint8_t a_reg, uint8_t val, uint8_t c_flag){ return val   -   1;         }

// ====== special cases ====

void incr(cpu *cpu, uint8_t *r)
{
    set_reset_flags(cpu, (*r)+1, *r, 1, ALL_BUT_CY);
    (*r)++;
}

void incr_m(cpu *cpu)
{
    incr(cpu, mem_ptr_out(cpu, join(cpu->h, cpu->l)));
}

void decr(cpu *cpu, uint8_t *r)
{
    set_reset_flags(cpu, (*r)-1, *r, -1, ALL_BUT_CY);
    (*r)--;
}

//DCR M
void decr_m(cpu *cpu)
{
    decr(cpu, mem_ptr_out(cpu, join(cpu->h, cpu->l)));
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
    cpu->flags->c = !!(result & 0xffff0000);

    cpu->h = get_rh((uint16_t)result);
    cpu->l = get_rl((uint16_t)result);
}

void add_sp_hl(cpu *cpu)
{
    add_rp_hl(cpu, get_rh(cpu->sp), get_rl(cpu->sp));
}

void incr_sp(cpu *cpu)
{
    uint8_t rh = get_rh(cpu->sp);
    uint8_t rl = get_rl(cpu->sp);

    incr_rp(&rh, &rl);
    cpu->sp = join(rh, rl);
}

void decr_sp(cpu *cpu)
{
    uint8_t rh = get_rh(cpu->sp);
    uint8_t rl = get_rl(cpu->sp);

    decr_rp(&rh, &rl);
    cpu->sp = join(rh, rl);
}

//general rotate byte
void rotate_byte(cpu *cpu, uint8_t c_set, uint8_t lone_bit, uint8_t shifted)
{
    uint8_t result = (lone_bit | shifted);    
    printf("%d\n", result);
    cpu->flags->c = !!c_set;    
    cpu->a = result;
}

//RLC
void rotate_left(cpu *cpu, uint8_t carry)
{
    uint8_t lone_bit = (cpu->a >> 7);
    if(carry){lone_bit = cpu->flags->c;}

    rotate_byte(cpu, (cpu->a >> 7), lone_bit, (cpu->a << 1));
}

//RRC 
void rotate_right(cpu *cpu, uint8_t carry)
{
    uint8_t lone_bit = (cpu->a << 7);
    if(carry){lone_bit = (cpu->flags->c << 7);}

    rotate_byte(cpu, (cpu->a << 7), lone_bit, (cpu->a >> 1));
}

//DAA 
void decimal_adj_acc(cpu *cpu)
{
    if((cpu->a & 0x0f) > 0x9 || cpu->flags->ac) cpu->a += 6;

    if((cpu->a >> 4) > 0x9 || cpu->flags->c)
    {
        uint8_t tmp = (cpu->a >> 4) + 6;
	cpu->flags->c = !!(tmp & 0xf0);
        cpu->a = (tmp << 4) | (cpu->a & 0x0f);
    }
}


//=========== stack ===================

//PUSH rp
void push(cpu *cpu, uint8_t rh, uint8_t rl)
{
    mem_in(cpu, --cpu->sp, rh);
    mem_in(cpu, --cpu->sp, rl);
}

//POP rp
void pop(cpu *cpu, uint8_t *rh, uint8_t *rl)
{
    *rl = mem_out(cpu, cpu->sp++);
    *rh = mem_out(cpu, cpu->sp++);
}
//POP PSW
void pop_psw(cpu *cpu)
{
    uint8_t psw;
    pop(cpu, &cpu->a, &psw);

    set_psw(cpu, psw);
}

//XTHL
/*
 * I know it's weird but, bear with me, this is a literal
 * one time case, so yeah, I am doing the weird swap
 */
void ex_hl_sp(cpu *cpu)
{
    swap(&cpu->l, mem_ptr_out(cpu, cpu->sp));
    swap(&cpu->h, mem_ptr_out(cpu, cpu->sp+1));
}

//SPHL
void store_hl_sp(cpu *cpu)
{
    cpu->sp = join(cpu->h, cpu->l);
}

//========== IO ===============
//IN port
void in_port(cpu *cpu)
{
    uint8_t port = mem_out(cpu, ++cpu->pc);
    cpu->h = get_rh(cpu->ports[port]);
    cpu->l = get_rl(cpu->ports[port]);

}

//OUT port
void out_port(cpu *cpu)
{
    uint8_t port = mem_out(cpu, ++cpu->pc);
    cpu->ports[port] = join_hl(cpu);
}


//========== flow control ==========

//JMP addr
/*
 * If you are ever confused, the jump ALWAYS adds up 2 to pc,
 * if or if not the condition is true
 */
void jump(cpu *cpu, uint8_t cond)
{
    uint8_t byte2 = mem_out(cpu, ++cpu->pc);
    uint8_t byte3 = mem_out(cpu, ++cpu->pc);

    if(cond)
    {
	cpu->pc = join(byte3, byte2);
	//weird but it's only so that I can inc the pc every inst_process
	cpu->pc--;
    }
}

//CALL addr
void call(cpu *cpu, uint8_t cond)
{
    if(cond)
    {
	push(cpu, get_rh(cpu->pc+2), get_rl(cpu->pc+2));
	jump(cpu, cond);
    }

    else cpu->pc += 2;
}

//RET
void ret(cpu *cpu, uint8_t cond)
{
    if(cond)
    {
	uint8_t rh, rl;
	pop(cpu, &rh, &rl);
	cpu->pc = join(rh, rl);
    }
}

//RST n
void rst_n(cpu *cpu, uint8_t n)
{
    push(cpu, get_rh(cpu->pc), get_rl(cpu->pc));
    cpu->pc = n << 3;
    cpu->pc--;
}

//PCHL
void jump_hl(cpu *cpu)
{
    cpu->pc = join(cpu->h, cpu->l);
    cpu->pc--;
}

void generate_intr(cpu *cpu, uint8_t opcode)
{
    cpu->intr = 1;
    cpu->intr_opcode = opcode;
}


// ================ emulation ==================

/*
 * Process an instruction and returns its cycle as defined in 
 * the manual
 */
int inst_process(cpu *cpu)
{
    uint8_t opcode = fetch_inst(cpu, cpu->pc);

    if(cpu->intr && cpu->intr_enable) 
    {
	opcode           = cpu->intr_opcode;
	cpu->intr_opcode = 0;
	cpu->intr        = 0;
    }

#ifdef DEBUG
    printf("\x1b[33m");
    printf("%#04x ", cpu->pc);
    printf("\x1b[0m"); 

    printf("%#04x ", opcode);
    debug_emu(opcode);
#endif

    switch(opcode)
    {
	//========== misc ============
	case 0x76: cpu->halt = 1;        break; //    HLT
	case 0xf3: cpu->intr_enable = 0; break; //    DI	
	case 0xfb: cpu->intr_enable = 1; break; //    EI	
	
	case 0x2f: cpu->a = ~cpu->a;               break; //    CMA
	case 0x37: cpu->flags->c = 1;              break; //    STC
	case 0x3f: cpu->flags->c = ~cpu->flags->c; break; //    CMC

	// ======== data transfer =======
	
	case 0x22: store_hl_addr(cpu); break;     //    SHLD adr
	case 0x2a: load_hl_addr(cpu); break;      //    LHLD adr

	case 0xe3: ex_hl_sp(cpu); break; //    XTHL	
	case 0xf9: store_hl_sp(cpu); break; //    SPHL	

	case 0xeb: swap_hl_de(cpu); break;        //    XCHG	    

	case 0x3a: load_a_addr(cpu); break;      //    LDA adr
	case 0x32: store_a_addr(cpu); break;    //    STA adr

	case 0x01: load_rp_data(cpu, &cpu->b, &cpu->c); break; //    LXI B,D16	
	case 0x11: load_rp_data(cpu, &cpu->d, &cpu->e); break; //    LXI D,D16	
	case 0x21: load_rp_data(cpu, &cpu->h, &cpu->l); break; //    LXI H,D16	
	case 0x31: load_sp_rp(cpu);                     break; //    LXI SP,D16	

	case 0x02: store_a_rp(cpu, cpu->b, cpu->c); break; //    STAX B	
	case 0x12: store_a_rp(cpu, cpu->d, cpu->e); break; //    STAX D	
	case 0x0a: load_a_rp(cpu, cpu->b, cpu->c); break; //    LDAX B	
	case 0x1a: load_a_rp(cpu, cpu->d, cpu->e); break; //    LDAX D	

	case 0x06: load_word(cpu, &cpu->b); break; //    MVI B, D8	
	case 0x0e: load_word(cpu, &cpu->c); break; //    MVI C, D8	
	case 0x16: load_word(cpu, &cpu->d); break; //    MVI D, D8	
	case 0x1e: load_word(cpu, &cpu->e); break; //    MVI E, D8	
	case 0x26: load_word(cpu, &cpu->h); break; //    MVI H, D8	
	case 0x2e: load_word(cpu, &cpu->l); break; //    MVI L, D8	
	case 0x3e: load_word(cpu, &cpu->a); break; //    MVI A, D8	

	case 0x36: store_byte_hl(cpu);      break; //    MVI M, D8

	case 0x46: load_word_hl(cpu, &cpu->b); break; //    MOV B,M	
	case 0x4e: load_word_hl(cpu, &cpu->c); break; //    MOV C,M	
	case 0x56: load_word_hl(cpu, &cpu->d); break; //    MOV D,M	
	case 0x5e: load_word_hl(cpu, &cpu->e); break; //    MOV E,M	
	case 0x66: load_word_hl(cpu, &cpu->h); break; //    MOV H,M	
	case 0x6e: load_word_hl(cpu, &cpu->l); break; //    MOV L,M	
	case 0x7e: load_word_hl(cpu, &cpu->a); break; //    MOV A,M	

	case 0x40: cpu->b = cpu->b; break;  //    MOV B,B	
	case 0x41: cpu->b = cpu->c; break;  //    MOV B,C	
	case 0x42: cpu->b = cpu->d; break;  //    MOV B,D	
	case 0x43: cpu->b = cpu->e; break;  //    MOV B,E	
	case 0x44: cpu->b = cpu->h; break;  //    MOV B,H	
	case 0x45: cpu->b = cpu->l; break;  //    MOV B,L	
	case 0x47: cpu->b = cpu->a; break;  //    MOV B,A	

	case 0x48: cpu->c = cpu->b; break;  //    MOV C,B	
	case 0x49: cpu->c = cpu->c; break;  //    MOV C,C	
	case 0x4a: cpu->c = cpu->d; break;  //    MOV C,D	
	case 0x4b: cpu->c = cpu->e; break;  //    MOV C,E	
	case 0x4c: cpu->c = cpu->h; break;  //    MOV C,H	
	case 0x4d: cpu->c = cpu->l; break;  //    MOV C,L	
	case 0x4f: cpu->c = cpu->a; break;  //    MOV C,A	
		   
	case 0x50: cpu->d = cpu->b; break;  //    MOV D,B	
	case 0x51: cpu->d = cpu->c; break;  //    MOV D,C	
	case 0x52: cpu->d = cpu->d; break;  //    MOV D,D	
	case 0x53: cpu->d = cpu->e; break;  //    MOV D,E	
	case 0x54: cpu->d = cpu->h; break;  //    MOV D,H	
	case 0x55: cpu->d = cpu->l; break;  //    MOV D,L	
	case 0x57: cpu->d = cpu->a; break;  //    MOV D,A	

	case 0x58: cpu->e = cpu->b; break;  //    MOV E,B	
	case 0x59: cpu->e = cpu->c; break;  //    MOV E,C	
	case 0x5a: cpu->e = cpu->d; break;  //    MOV E,D	
	case 0x5b: cpu->e = cpu->e; break;  //    MOV E,E	
	case 0x5c: cpu->e = cpu->h; break;  //    MOV E,H	
	case 0x5d: cpu->e = cpu->l; break;  //    MOV E,L	
	case 0x5f: cpu->e = cpu->a; break;  //    MOV E,A	

	case 0x60: cpu->h = cpu->b; break;  //    MOV H,B	
	case 0x61: cpu->h = cpu->c; break;  //    MOV H,C	
	case 0x62: cpu->h = cpu->d; break;  //    MOV H,D	
	case 0x63: cpu->h = cpu->e; break;  //    MOV H,E	
	case 0x64: cpu->h = cpu->h; break;  //    MOV H,H	
	case 0x65: cpu->h = cpu->l; break;  //    MOV H,L	
	case 0x67: cpu->h = cpu->a; break;  //    MOV H,A	

	case 0x68: cpu->l = cpu->b; break;  //    MOV L,B	
	case 0x69: cpu->l = cpu->c; break;  //    MOV L,C	
	case 0x6a: cpu->l = cpu->d; break;  //    MOV L,D	
	case 0x6b: cpu->l = cpu->e; break;  //    MOV L,E	
	case 0x6c: cpu->l = cpu->h; break;  //    MOV L,H	
	case 0x6d: cpu->l = cpu->l; break;  //    MOV L,L	
	case 0x6f: cpu->l = cpu->a; break;  //    MOV L,A	

	case 0x70: store_word_hl(cpu, cpu->b); break; //    MOV M,B	
	case 0x71: store_word_hl(cpu, cpu->c); break; //    MOV M,C	
	case 0x72: store_word_hl(cpu, cpu->d); break; //    MOV M,D	
	case 0x73: store_word_hl(cpu, cpu->e); break; //    MOV M,E	
	case 0x74: store_word_hl(cpu, cpu->h); break; //    MOV M,H	
	case 0x75: store_word_hl(cpu, cpu->l); break; //    MOV M,L	
	case 0x77: store_word_hl(cpu, cpu->a); break; //    MOV M,A	

	case 0x78: cpu->a = cpu->b; break;  //    MOV A,B
	case 0x79: cpu->a = cpu->c; break;  //    MOV A,C
	case 0x7a: cpu->a = cpu->d; break;  //    MOV A,D
	case 0x7b: cpu->a = cpu->e; break;  //    MOV A,E
	case 0x7c: cpu->a = cpu->h; break;  //    MOV A,H
	case 0x7d: cpu->a = cpu->l; break;  //    MOV A,L
	case 0x7f: cpu->a = cpu->a; break;  //    MOV A,A

	// ======== arithmetic =======
	
	case 0x09: add_rp_hl(cpu, cpu->b, cpu->c); break; //    DAD B	
	case 0x19: add_rp_hl(cpu, cpu->d, cpu->e); break; //    DAD D	
	case 0x29: add_rp_hl(cpu, cpu->h, cpu->l); break; //    DAD H	
	case 0x39: add_sp_hl(cpu) ; break; //    DAD SP	

	case 0x17: rotate_left (cpu, 1); break; //    RAL
	case 0x1f: rotate_right(cpu, 1); break; //   RAR
	case 0x07: rotate_left (cpu, CARRY_OFF);  break; //    RLC
	case 0x0f: rotate_right(cpu, CARRY_OFF);  break; //   RRC

	case 0x03: incr_rp(&cpu->b, &cpu->c); break; //    INX B	
	case 0x13: incr_rp(&cpu->d, &cpu->e); break; //    INX D	
	case 0x23: incr_rp(&cpu->h, &cpu->l); break; //    INX H

	case 0x0b: decr_rp(&cpu->b, &cpu->c); break; //    DCX B	
	case 0x1b: decr_rp(&cpu->d, &cpu->e); break;//     DCX D
	case 0x2b: decr_rp(&cpu->h, &cpu->l); break; //    DCX H
	
	case 0x04: incr(cpu, &cpu->b); break; //    INR B	
	case 0x0c: incr(cpu, &cpu->c); break; //    INR C	
	case 0x14: incr(cpu, &cpu->d); break; //    INR D	
	case 0x1c: incr(cpu, &cpu->e); break; //    INR E	
	case 0x24: incr(cpu, &cpu->h); break; //    INR H	
	case 0x2c: incr(cpu, &cpu->l); break; //    INR L	
	case 0x3c: incr(cpu, &cpu->a); break; //    INR A	

	case 0x05: decr(cpu, &cpu->b); break; //    DCR B	
	case 0x0d: decr(cpu, &cpu->c); break; //    DCR C	
	case 0x15: decr(cpu, &cpu->d); break; //    DCR D	
	case 0x1d: decr(cpu, &cpu->e); break; //    DCR E	
	case 0x25: decr(cpu, &cpu->h); break; //    DCR H	
	case 0x2d: decr(cpu, &cpu->l); break; //    DCR L	
	case 0x3d: decr(cpu, &cpu->a); break; //    DCR A	

	case 0x3b: decr_sp(cpu); break; //    DCX SP	
	case 0x33: incr_sp(cpu); break; //    INX SP	

	case 0x34: incr_m(cpu); break; //    INR M	
	case 0x35: decr_m(cpu); break; //    DCR M	

	case 0x27: decimal_adj_acc(cpu); break; //    DAA	

	case 0x80: cpu->a = alu_inst(cpu, REGISTER, add, cpu->b, CARRY_OFF, ALL_FLAGS); break; //    ADD B	
	case 0x81: cpu->a = alu_inst(cpu, REGISTER, add, cpu->c, CARRY_OFF, ALL_FLAGS); break; //    ADD C	
	case 0x82: cpu->a = alu_inst(cpu, REGISTER, add, cpu->d, CARRY_OFF, ALL_FLAGS); break; //    ADD D	
	case 0x83: cpu->a = alu_inst(cpu, REGISTER, add, cpu->e, CARRY_OFF, ALL_FLAGS); break; //    ADD E	
	case 0x84: cpu->a = alu_inst(cpu, REGISTER, add, cpu->h, CARRY_OFF, ALL_FLAGS); break; //    ADD H	
	case 0x85: cpu->a = alu_inst(cpu, REGISTER, add, cpu->l, CARRY_OFF, ALL_FLAGS); break; //    ADD L	
	case 0x87: cpu->a = alu_inst(cpu, REGISTER, add, cpu->a, CARRY_OFF, ALL_FLAGS); break; //    ADD A	

	case 0x88: cpu->a = alu_inst(cpu, REGISTER, add, cpu->b, CARRY_ON, ALL_FLAGS); break; //    ADC B	
	case 0x89: cpu->a = alu_inst(cpu, REGISTER, add, cpu->c, CARRY_ON, ALL_FLAGS); break; //    ADC C	
	case 0x8a: cpu->a = alu_inst(cpu, REGISTER, add, cpu->d, CARRY_ON, ALL_FLAGS); break; //    ADC D	
	case 0x8b: cpu->a = alu_inst(cpu, REGISTER, add, cpu->e, CARRY_ON, ALL_FLAGS); break; //    ADC E	
	case 0x8c: cpu->a = alu_inst(cpu, REGISTER, add, cpu->h, CARRY_ON, ALL_FLAGS); break; //    ADC H	
	case 0x8d: cpu->a = alu_inst(cpu, REGISTER, add, cpu->l, CARRY_ON, ALL_FLAGS); break; //    ADC L	
	case 0x8f: cpu->a = alu_inst(cpu, REGISTER, add, cpu->a, CARRY_ON, ALL_FLAGS); break; //    ADC A	

	case 0x90: cpu->a = alu_inst(cpu, REGISTER, sub, cpu->b, CARRY_OFF, ALL_FLAGS); break; //    SUB B	
	case 0x91: cpu->a = alu_inst(cpu, REGISTER, sub, cpu->c, CARRY_OFF, ALL_FLAGS); break; //    SUB C	
	case 0x92: cpu->a = alu_inst(cpu, REGISTER, sub, cpu->d, CARRY_OFF, ALL_FLAGS); break; //    SUB D	
	case 0x93: cpu->a = alu_inst(cpu, REGISTER, sub, cpu->e, CARRY_OFF, ALL_FLAGS); break; //    SUB E	
	case 0x94: cpu->a = alu_inst(cpu, REGISTER, sub, cpu->h, CARRY_OFF, ALL_FLAGS); break; //    SUB H	
	case 0x95: cpu->a = alu_inst(cpu, REGISTER, sub, cpu->l, CARRY_OFF, ALL_FLAGS); break; //    SUB L	
	case 0x97: cpu->a = alu_inst(cpu, REGISTER, sub, cpu->a, CARRY_OFF, ALL_FLAGS); break; //    SUB A	

	case 0x98: cpu->a = alu_inst(cpu, REGISTER, sub, cpu->b, CARRY_ON, ALL_FLAGS); break; //    SBB B	
	case 0x99: cpu->a = alu_inst(cpu, REGISTER, sub, cpu->c, CARRY_ON, ALL_FLAGS); break; //    SBB C	
	case 0x9a: cpu->a = alu_inst(cpu, REGISTER, sub, cpu->d, CARRY_ON, ALL_FLAGS); break; //    SBB D	
	case 0x9b: cpu->a = alu_inst(cpu, REGISTER, sub, cpu->e, CARRY_ON, ALL_FLAGS); break; //    SBB E	
	case 0x9c: cpu->a = alu_inst(cpu, REGISTER, sub, cpu->h, CARRY_ON, ALL_FLAGS); break; //    SBB H	
	case 0x9d: cpu->a = alu_inst(cpu, REGISTER, sub, cpu->l, CARRY_ON, ALL_FLAGS); break; //    SBB L	
	case 0x9f: cpu->a = alu_inst(cpu, REGISTER, sub, cpu->a, CARRY_ON, ALL_FLAGS); break; //    SBB A	

	case 0xc6: cpu->a = alu_inst(cpu, IMMEDIATE, add, NO_VALUE, CARRY_OFF, ALL_FLAGS); break; //    ADI D8	
	case 0xd6: cpu->a = alu_inst(cpu, IMMEDIATE, sub, NO_VALUE, CARRY_OFF, ALL_FLAGS); break; //    SUI D8	

	case 0xce: cpu->a = alu_inst(cpu, IMMEDIATE, add, NO_VALUE, CARRY_ON,  ALL_FLAGS); break; //    ACI D8	
	case 0xde: cpu->a = alu_inst(cpu, IMMEDIATE, sub, NO_VALUE, CARRY_ON,  ALL_FLAGS); break; //    SBI D8	

	case 0xe6: cpu->a = alu_inst(cpu, IMMEDIATE, and, NO_VALUE, CARRY_OFF, ALL_CY_AC_CLEARED); break; //    ANI D8	
	case 0xee: cpu->a = alu_inst(cpu, IMMEDIATE, xor, NO_VALUE, CARRY_OFF, ALL_CY_AC_CLEARED); break; //    XRI D8	    
	case 0xf6: cpu->a = alu_inst(cpu, IMMEDIATE, or,  NO_VALUE, CARRY_OFF, ALL_CY_AC_CLEARED); break; //    ORI D8	    
	case 0xfe:          alu_inst(cpu, IMMEDIATE, cmp, NO_VALUE, CARRY_OFF, ALL_FLAGS); break; //    CPI D8	    

	case 0x86: cpu->a = alu_inst(cpu, REGISTER_INDIRECT, add, NO_VALUE, CARRY_OFF, ALL_FLAGS); break; //   ADD M	
	case 0x8e: cpu->a = alu_inst(cpu, REGISTER_INDIRECT, add, NO_VALUE, CARRY_ON,  ALL_FLAGS); break; //   ADC M	
	case 0x96: cpu->a = alu_inst(cpu, REGISTER_INDIRECT, sub, NO_VALUE, CARRY_OFF, ALL_FLAGS); break; //   SUB M	
	case 0x9e: cpu->a = alu_inst(cpu, REGISTER_INDIRECT, sub, NO_VALUE, CARRY_ON,  ALL_FLAGS); break; //   SBB M	

	case 0xa6: cpu->a = alu_inst(cpu, REGISTER_INDIRECT, and, NO_VALUE, CARRY_OFF, ALL_FLAGS); break; //   ANA M	
	case 0xae: cpu->a = alu_inst(cpu, REGISTER_INDIRECT, xor, NO_VALUE, CARRY_OFF, ALL_FLAGS); break; //   XRA M	
	case 0xb6: cpu->a = alu_inst(cpu, REGISTER_INDIRECT, or,  NO_VALUE, CARRY_OFF, ALL_FLAGS); break; //   ORA M	
	case 0xbe:          alu_inst(cpu, REGISTER_INDIRECT, cmp, NO_VALUE, CARRY_OFF, ALL_FLAGS); break; //   CMP M	
	
	case 0xa0: cpu->a = alu_inst(cpu, REGISTER, and, cpu->b, CARRY_OFF, ALL_CY_CLEARED); break; //    ANA B
	case 0xa1: cpu->a = alu_inst(cpu, REGISTER, and, cpu->c, CARRY_OFF, ALL_CY_CLEARED); break; //    ANA C
	case 0xa2: cpu->a = alu_inst(cpu, REGISTER, and, cpu->d, CARRY_OFF, ALL_CY_CLEARED); break; //    ANA D
	case 0xa3: cpu->a = alu_inst(cpu, REGISTER, and, cpu->e, CARRY_OFF, ALL_CY_CLEARED); break; //    ANA E
	case 0xa4: cpu->a = alu_inst(cpu, REGISTER, and, cpu->h, CARRY_OFF, ALL_CY_CLEARED); break; //    ANA H
	case 0xa5: cpu->a = alu_inst(cpu, REGISTER, and, cpu->l, CARRY_OFF, ALL_CY_CLEARED); break; //    ANA L
	case 0xa7: cpu->a = alu_inst(cpu, REGISTER, and, cpu->a, CARRY_OFF, ALL_CY_CLEARED); break; //    ANA A

	case 0xa8: cpu->a = alu_inst(cpu, REGISTER, xor, cpu->b, CARRY_OFF, ALL_CY_AC_CLEARED); break; //    XRA B
	case 0xa9: cpu->a = alu_inst(cpu, REGISTER, xor, cpu->c, CARRY_OFF, ALL_CY_AC_CLEARED); break; //    XRA C
	case 0xaa: cpu->a = alu_inst(cpu, REGISTER, xor, cpu->d, CARRY_OFF, ALL_CY_AC_CLEARED); break; //    XRA D
	case 0xab: cpu->a = alu_inst(cpu, REGISTER, xor, cpu->e, CARRY_OFF, ALL_CY_AC_CLEARED); break; //    XRA E
	case 0xac: cpu->a = alu_inst(cpu, REGISTER, xor, cpu->h, CARRY_OFF, ALL_CY_AC_CLEARED); break; //    XRA H
	case 0xad: cpu->a = alu_inst(cpu, REGISTER, xor, cpu->l, CARRY_OFF, ALL_CY_AC_CLEARED); break; //    XRA L
	case 0xaf: cpu->a = alu_inst(cpu, REGISTER, xor, cpu->a, CARRY_OFF, ALL_CY_AC_CLEARED); break; //    XRA A

	case 0xb0: cpu->a = alu_inst(cpu, REGISTER, or, cpu->b, CARRY_OFF, ALL_CY_AC_CLEARED); break; //     ORA B
	case 0xb1: cpu->a = alu_inst(cpu, REGISTER, or, cpu->c, CARRY_OFF, ALL_CY_AC_CLEARED); break; //     ORA C
	case 0xb2: cpu->a = alu_inst(cpu, REGISTER, or, cpu->d, CARRY_OFF, ALL_CY_AC_CLEARED); break; //     ORA D
	case 0xb3: cpu->a = alu_inst(cpu, REGISTER, or, cpu->e, CARRY_OFF, ALL_CY_AC_CLEARED); break; //     ORA E
	case 0xb4: cpu->a = alu_inst(cpu, REGISTER, or, cpu->h, CARRY_OFF, ALL_CY_AC_CLEARED); break; //     ORA H
	case 0xb5: cpu->a = alu_inst(cpu, REGISTER, or, cpu->l, CARRY_OFF, ALL_CY_AC_CLEARED); break; //     ORA L
	case 0xb7: cpu->a = alu_inst(cpu, REGISTER, or, cpu->a, CARRY_OFF, ALL_CY_AC_CLEARED); break; //     ORA A

	case 0xb8: alu_inst(cpu, REGISTER, cmp, cpu->b, CARRY_OFF, ALL_FLAGS); break; //    CMP B	
	case 0xb9: alu_inst(cpu, REGISTER, cmp, cpu->c, CARRY_OFF, ALL_FLAGS); break; //    CMP C	
	case 0xba: alu_inst(cpu, REGISTER, cmp, cpu->d, CARRY_OFF, ALL_FLAGS); break; //    CMP D	
	case 0xbb: alu_inst(cpu, REGISTER, cmp, cpu->e, CARRY_OFF, ALL_FLAGS); break; //    CMP E	
	case 0xbc: alu_inst(cpu, REGISTER, cmp, cpu->h, CARRY_OFF, ALL_FLAGS); break; //    CMP H	
	case 0xbd: alu_inst(cpu, REGISTER, cmp, cpu->l, CARRY_OFF, ALL_FLAGS); break; //    CMP L	
	case 0xbf: alu_inst(cpu, REGISTER, cmp, cpu->a, CARRY_OFF, ALL_FLAGS); break; //    CMP A	

	// ========= stack =========
	case 0xc1: pop(cpu, &cpu->b, &cpu->c); break; //    POP B	
	case 0xd1: pop(cpu, &cpu->d, &cpu->e); break; //    POP D	
	case 0xe1: pop(cpu, &cpu->h, &cpu->l); break; //    POP H	
	case 0xf1: pop_psw(cpu); break; //    POP PSW	

	case 0xc5: push(cpu, cpu->b, cpu->c); break; //    PUSH B	
	case 0xd5: push(cpu, cpu->d, cpu->e); break; //    PUSH D	
	case 0xe5: push(cpu, cpu->h, cpu->l); break; //    PUSH H	
	case 0xf5: push(cpu, cpu->a, get_psw(cpu)); break; //    PUSH PSW	

	// ========= flow control ===========

	case 0xe9: jump_hl(cpu); break; //    PCHL	

	case 0xc3: jump(cpu, NO_COND); break; //    JMP adr	

//cpu diagnostic call procedure
#ifdef CPUDIAG
	case 0xcd: cpu_diag_call(cpu); break; 
#else
	case 0xcd: call(cpu, NO_COND); break; //    CALL adr
#endif
	case 0xc9: ret(cpu,  NO_COND); break; //    RET	

	case 0xc2: jump(cpu, !cpu->flags->z); break; //    JNZ adr	
	case 0xd2: jump(cpu, !cpu->flags->c); break; //    JNC adr	
	case 0xe2: jump(cpu, !cpu->flags->p); break; //    JPO adr	
	case 0xf2: jump(cpu, !cpu->flags->s); break; //    JP adr	    

	case 0xc4: call(cpu, !cpu->flags->z); break; //    CNZ  adr	
	case 0xd4: call(cpu, !cpu->flags->c); break; //    CNC  adr	
	case 0xe4: call(cpu, !cpu->flags->p); break; //    CPO  adr	
	case 0xf4: call(cpu, !cpu->flags->s); break; //    CP   adr	    

	case 0xc0: ret(cpu, !cpu->flags->z); break; //    RNZ	
	case 0xd0: ret(cpu, !cpu->flags->c); break; //    RNC	
	case 0xe0: ret(cpu, !cpu->flags->p); break; //    RPO	
	case 0xf0: ret(cpu, !cpu->flags->s); break; //    RP	

	case 0xca: jump(cpu, cpu->flags->z); break; //    JZ adr
	case 0xda: jump(cpu, cpu->flags->c); break; //    JC adr	
	case 0xea: jump(cpu, cpu->flags->p); break; //    JPE adr	    
	case 0xfa: jump(cpu, cpu->flags->s); break; //    JM adr	    

	case 0xcc: call(cpu, cpu->flags->z); break; //    CZ   adr	
	case 0xdc: call(cpu, cpu->flags->c); break; //    CC   adr	
	case 0xec: call(cpu, cpu->flags->p); break; //    CPE  adr	    
	case 0xfc: call(cpu, cpu->flags->s); break; //    CM   adr	    
                                         
	case 0xc8: ret(cpu, cpu->flags->z); break; //    RZ	
	case 0xd8: ret(cpu, cpu->flags->c); break; //    RC	
	case 0xe8: ret(cpu, cpu->flags->p); break; //    RPE	
	case 0xf8: ret(cpu, cpu->flags->s); break; //    RM

	case 0xc7: rst_n(cpu, 0); break; //    RST 0	
	case 0xcf: rst_n(cpu, 1); break; //    RST 1	
	case 0xd7: rst_n(cpu, 2); break; //    RST 2	
	case 0xdf: rst_n(cpu, 3); break; //    RST 3	
	case 0xe7: rst_n(cpu, 4); break; //    RST 4	
	case 0xef: rst_n(cpu, 5); break; //    RST 5	    
	case 0xf7: rst_n(cpu, 6); break; //    RST 6	    
	case 0xff: rst_n(cpu, 7); break; //    RST 7	


	//======== IO =========
	case 0xd3: out_port(cpu); break; //    OUT D8	
	case 0xdb: in_port(cpu);  break; //    IN D8	

	//========= NOP =======
	case 0x00: break; //    -
	case 0x08: break; //    -
	case 0x10: break; //    -	
	case 0x18: break; //    -	
	case 0x20: break; //    -	
	case 0x28: break; //    -	
	case 0x30: break; //    -	
	case 0x38: break; //    -	
	case 0xcb: break; //    -		
	case 0xd9: break; //    -	
	case 0xdd: break; //    -	
	case 0xed: break; //    -	
	case 0xfd: break; //    -	

	default: printf("%#04x not found\n", opcode);
    }

#ifdef STATE
    printf("\n");
    print_state(cpu);
#endif
   
    //increase pc
    cpu->pc++;

    //return instruction cycle 
    return instruction_cycle[opcode];
}

//debungging function 
void debug_emu(uint8_t opcode)
{
    printf("\x1b[032m"); //Set the text to the color red.
    switch(opcode)
    {
	case 0x00: printf("NOP	\n"); break;
	case 0x01: printf("LXI B,D16\n"); break;
	case 0x02: printf("STAX B	\n"); break;
	case 0x03: printf("INX B	\n"); break; 
	case 0x04: printf("INR B	\n"); break; 
	case 0x05: printf("DCR B	\n"); break; 
	case 0x06: printf("MVI B, D8	\n"); break; 
	case 0x07: printf("RLC	1	\n"); break; 
	case 0x08: printf("-		\n"); break; 
	case 0x09: printf("DAD B	\n"); break; 
	case 0x0a: printf("LDAX B	\n"); break; 
	case 0x0b: printf("DCX B	\n"); break; 
	case 0x0c: printf("INR C	\n"); break; 
	case 0x0d: printf("DCR C	\n"); break; 
	case 0x0e: printf("MVI C,D8	\n"); break; 
	case 0x0f: printf("RRC	1	\n"); break; 
	case 0x10: printf("-		\n"); break; 
	case 0x11: printf("LXI D,D16	\n"); break; 
	case 0x12: printf("STAX D	\n"); break; 
	case 0x13: printf("INX D	\n"); break; 
	case 0x14: printf("INR D	\n"); break; 
	case 0x15: printf("DCR D	\n"); break; 
	case 0x16: printf("MVI D, D8	\n"); break; 
	case 0x17: printf("RAL	1	\n"); break; 
	case 0x18: printf("-		\n"); break; 
	case 0x19: printf("DAD D	\n"); break; 
	case 0x1a: printf("LDAX D	\n"); break; 
	case 0x1b: printf("DCX D	\n"); break; 
	case 0x1c: printf("INR E	\n"); break; 
	case 0x1d: printf("DCR E	\n"); break; 
	case 0x1e: printf("MVI E,D8	\n"); break; 
	case 0x1f: printf("RAR	1	\n"); break; 
	case 0x20: printf("-		\n"); break; 
	case 0x21: printf("LXI H,D16	\n"); break; 
	case 0x22: printf("SHLD adr	\n"); break; 
	case 0x23: printf("INX H	\n"); break; 
	case 0x24: printf("INR H	\n"); break; 
	case 0x25: printf("DCR H	\n"); break; 
	case 0x26: printf("MVI H,D8	\n"); break; 
	case 0x27: printf("DAA	1	\n"); break; 
	case 0x28: printf("-		\n"); break; 
	case 0x29: printf("DAD H	\n"); break; 
	case 0x2a: printf("LHLD adr	\n"); break; 
	case 0x2b: printf("DCX H	\n"); break; 
	case 0x2c: printf("INR L	\n"); break; 
	case 0x2d: printf("DCR L	\n"); break; 
	case 0x2e: printf("MVI L, D8	\n"); break; 
	case 0x2f: printf("CMA	1	\n"); break; 
	case 0x30: printf("-		\n"); break; 
	case 0x31: printf("LXI SP, D16	\n"); break; 
	case 0x32: printf("STA adr	\n"); break; 
	case 0x33: printf("INX SP	\n"); break; 
	case 0x34: printf("INR M	\n"); break; 
	case 0x35: printf("DCR M	\n"); break; 
	case 0x36: printf("MVI M,D8	\n"); break; 
	case 0x37: printf("STC	1	\n"); break; 
	case 0x38: printf("-		\n"); break; 
	case 0x39: printf("DAD SP	\n"); break; 
	case 0x3a: printf("LDA adr	\n"); break; 
	case 0x3b: printf("DCX SP	\n"); break; 
	case 0x3c: printf("INR A	\n"); break; 
	case 0x3d: printf("DCR A	\n"); break; 
	case 0x3e: printf("MVI A,D8	\n"); break; 
	case 0x3f: printf("CMC	1	\n"); break; 
	case 0x40: printf("MOV B,B	\n"); break; 
	case 0x41: printf("MOV B,C	\n"); break; 
	case 0x42: printf("MOV B,D	\n"); break; 
	case 0x43: printf("MOV B,E	\n"); break; 
	case 0x44: printf("MOV B,H	\n"); break; 
	case 0x45: printf("MOV B,L	\n"); break; 
	case 0x46: printf("MOV B,M	\n"); break; 
	case 0x47: printf("MOV B,A	\n"); break; 
	case 0x48: printf("MOV C,B	\n"); break; 
	case 0x49: printf("MOV C,C	\n"); break; 
	case 0x4a: printf("MOV C,D	\n"); break; 
	case 0x4b: printf("MOV C,E	\n"); break; 
	case 0x4c: printf("MOV C,H	\n"); break; 
	case 0x4d: printf("MOV C,L	\n"); break; 
	case 0x4e: printf("MOV C,M	\n"); break; 
	case 0x4f: printf("MOV C,A	\n"); break; 
	case 0x50: printf("MOV D,B	\n"); break; 
	case 0x51: printf("MOV D,C	\n"); break; 
	case 0x52: printf("MOV D,D	\n"); break; 
	case 0x53: printf("MOV D,E	\n"); break; 
	case 0x54: printf("MOV D,H	\n"); break; 
	case 0x55: printf("MOV D,L	\n"); break; 
	case 0x56: printf("MOV D,M	\n"); break; 
	case 0x57: printf("MOV D,A	\n"); break; 
	case 0x58: printf("MOV E,B	\n"); break; 
	case 0x59: printf("MOV E,C	\n"); break; 
	case 0x5a: printf("MOV E,D	\n"); break; 
	case 0x5b: printf("MOV E,E	\n"); break; 
	case 0x5c: printf("MOV E,H	\n"); break; 
	case 0x5d: printf("MOV E,L	\n"); break; 
	case 0x5e: printf("MOV E,M	\n"); break; 
	case 0x5f: printf("MOV E,A	\n"); break; 
	case 0x60: printf("MOV H,B	\n"); break; 
	case 0x61: printf("MOV H,C	\n"); break; 
	case 0x62: printf("MOV H,D	\n"); break; 
	case 0x63: printf("MOV H,E	\n"); break; 
	case 0x64: printf("MOV H,H	\n"); break; 
	case 0x65: printf("MOV H,L	\n"); break; 
	case 0x66: printf("MOV H,M	\n"); break; 
	case 0x67: printf("MOV H,A	\n"); break; 
	case 0x68: printf("MOV L,B	\n"); break; 
	case 0x69: printf("MOV L,C	\n"); break; 
	case 0x6a: printf("MOV L,D	\n"); break; 
	case 0x6b: printf("MOV L,E	\n"); break; 
	case 0x6c: printf("MOV L,H	\n"); break; 
	case 0x6d: printf("MOV L,L	\n"); break; 
	case 0x6e: printf("MOV L,M	\n"); break; 
	case 0x6f: printf("MOV L,A	\n"); break; 
	case 0x70: printf("MOV M,B	\n"); break; 
	case 0x71: printf("MOV M,C	\n"); break; 
	case 0x72: printf("MOV M,D	\n"); break; 
	case 0x73: printf("MOV M,E	\n"); break; 
	case 0x74: printf("MOV M,H	\n"); break; 
	case 0x75: printf("MOV M,L	\n"); break; 
	case 0x76: printf("HLT	1	\n"); break; 
	case 0x77: printf("MOV M,A	\n"); break; 
	case 0x78: printf("MOV A,B	\n"); break; 
	case 0x79: printf("MOV A,C	\n"); break; 
	case 0x7a: printf("MOV A,D	\n"); break; 
	case 0x7b: printf("MOV A,E	\n"); break; 
	case 0x7c: printf("MOV A,H	\n"); break; 
	case 0x7d: printf("MOV A,L	\n"); break; 
	case 0x7e: printf("MOV A,M	\n"); break; 
	case 0x7f: printf("MOV A,A	\n"); break; 
	case 0x80: printf("ADD B	\n"); break; 
	case 0x81: printf("ADD C	\n"); break; 
	case 0x82: printf("ADD D	\n"); break; 
	case 0x83: printf("ADD E	\n"); break; 
	case 0x84: printf("ADD H	\n"); break; 
	case 0x85: printf("ADD L	\n"); break; 
	case 0x86: printf("ADD M	\n"); break; 
	case 0x87: printf("ADD A	\n"); break; 
	case 0x88: printf("ADC B	\n"); break; 
	case 0x89: printf("ADC C	\n"); break; 
	case 0x8a: printf("ADC D	\n"); break; 
	case 0x8b: printf("ADC E	\n"); break; 
	case 0x8c: printf("ADC H	\n"); break; 
	case 0x8d: printf("ADC L	\n"); break; 
	case 0x8e: printf("ADC M	\n"); break; 
	case 0x8f: printf("ADC A	\n"); break; 
	case 0x90: printf("SUB B	\n"); break; 
	case 0x91: printf("SUB C	\n"); break; 
	case 0x92: printf("SUB D	\n"); break; 
	case 0x93: printf("SUB E	\n"); break; 
	case 0x94: printf("SUB H	\n"); break; 
	case 0x95: printf("SUB L	\n"); break; 
	case 0x96: printf("SUB M	\n"); break; 
	case 0x97: printf("SUB A	\n"); break; 
	case 0x98: printf("SBB B	\n"); break; 
	case 0x99: printf("SBB C	\n"); break; 
	case 0x9a: printf("SBB D	\n"); break; 
	case 0x9b: printf("SBB E	\n"); break; 
	case 0x9c: printf("SBB H	\n"); break; 
	case 0x9d: printf("SBB L	\n"); break; 
	case 0x9e: printf("SBB M	\n"); break; 
	case 0x9f: printf("SBB A	\n"); break; 
	case 0xa0: printf("ANA B	\n"); break; 
	case 0xa1: printf("ANA C	\n"); break; 
	case 0xa2: printf("ANA D	\n"); break; 
	case 0xa3: printf("ANA E	\n"); break; 
	case 0xa4: printf("ANA H	\n"); break; 
	case 0xa5: printf("ANA L	\n"); break; 
	case 0xa6: printf("ANA M	\n"); break; 
	case 0xa7: printf("ANA A	\n"); break; 
	case 0xa8: printf("XRA B	\n"); break; 
	case 0xa9: printf("XRA C	\n"); break; 
	case 0xaa: printf("XRA D	\n"); break; 
	case 0xab: printf("XRA E	\n"); break; 
	case 0xac: printf("XRA H	\n"); break; 
	case 0xad: printf("XRA L	\n"); break; 
	case 0xae: printf("XRA M	\n"); break; 
	case 0xaf: printf("XRA A	\n"); break; 
	case 0xb0: printf("ORA B	\n"); break; 
	case 0xb1: printf("ORA C	\n"); break; 
	case 0xb2: printf("ORA D	\n"); break; 
	case 0xb3: printf("ORA E	\n"); break; 
	case 0xb4: printf("ORA H	\n"); break; 
	case 0xb5: printf("ORA L	\n"); break; 
	case 0xb6: printf("ORA M	\n"); break; 
	case 0xb7: printf("ORA A	\n"); break; 
	case 0xb8: printf("CMP B	\n"); break; 
	case 0xb9: printf("CMP C	\n"); break; 
	case 0xba: printf("CMP D	\n"); break; 
	case 0xbb: printf("CMP E	\n"); break; 
	case 0xbc: printf("CMP H	\n"); break; 
	case 0xbd: printf("CMP L	\n"); break; 
	case 0xbe: printf("CMP M	\n"); break; 
	case 0xbf: printf("CMP A	\n"); break; 
	case 0xc0: printf("RNZ	1	\n"); break; 
	case 0xc1: printf("POP B	\n"); break; 
	case 0xc2: printf("JNZ adr	\n"); break; 
	case 0xc3: printf("JMP adr	\n"); break; 
	case 0xc4: printf("CNZ adr	\n"); break; 
	case 0xc5: printf("PUSH B	\n"); break; 
	case 0xc6: printf("ADI D8	\n"); break; 
	case 0xc7: printf("RST 0	\n"); break; 
	case 0xc8: printf("RZ	1	\n"); break; 
	case 0xc9: printf("RET	1	\n"); break; 
	case 0xca: printf("JZ adr	\n"); break; 
	case 0xcb: printf("-		\n"); break; 
	case 0xcc: printf("CZ adr	\n"); break; 
	case 0xcd: printf("CALL adr	\n"); break; 
	case 0xce: printf("ACI D8	\n"); break; 
	case 0xcf: printf("RST 1	\n"); break; 
	case 0xd0: printf("RNC	1	\n"); break; 
	case 0xd1: printf("POP D	\n"); break; 
	case 0xd2: printf("JNC adr	\n"); break; 
	case 0xd3: printf("OUT D8	\n"); break; 
	case 0xd4: printf("CNC adr	\n"); break; 
	case 0xd5: printf("PUSH D	\n"); break; 
	case 0xd6: printf("SUI D8	\n"); break; 
	case 0xd7: printf("RST 2	\n"); break; 
	case 0xd8: printf("RC	1	\n"); break; 
	case 0xd9: printf("-		\n"); break; 
	case 0xda: printf("JC adr	\n"); break; 
	case 0xdb: printf("IN D8	\n"); break; 
	case 0xdc: printf("CC adr	\n"); break; 
	case 0xdd: printf("-		\n"); break; 
	case 0xde: printf("SBI D8	\n"); break; 
	case 0xdf: printf("RST 3	\n"); break; 
	case 0xe0: printf("RPO	1	\n"); break; 
	case 0xe1: printf("POP H	\n"); break; 
	case 0xe2: printf("JPO adr	\n"); break; 
	case 0xe3: printf("XTHL	\n");         break;
	case 0xe4: printf("CPO adr	\n"); break; 
	case 0xe5: printf("PUSH H	\n"); break; 
	case 0xe6: printf("ANI D8	\n"); break; 
	case 0xe7: printf("RST 4	\n"); break; 
	case 0xe8: printf("RPE	1	\n"); break; 
	case 0xe9: printf("PCHL	\n");         break;
	case 0xea: printf("JPE adr	\n"); break; 
	case 0xeb: printf("XCHG	\n");         break;
	case 0xec: printf("CPE adr	\n"); break; 
	case 0xed: printf("-		\n"); break; 
	case 0xee: printf("XRI D8	\n"); break; 
	case 0xef: printf("RST 5	\n"); break; 
	case 0xf0: printf("RP	1	\n"); break; 
	case 0xf1: printf("POP PSW	\n"); break; 
	case 0xf2: printf("JP adr	\n"); break; 
	case 0xf3: printf("DI	1	\n"); break; 
	case 0xf4: printf("CP adr	\n"); break; 
	case 0xf5: printf("PUSH PSW	\n"); break; 
	case 0xf6: printf("ORI D8       \n"); break;
	case 0xf7: printf("RST 6        \n"); break;
	case 0xf8: printf("RM	1       \n"); break;
	case 0xf9: printf("SPHL         \n"); break;
	case 0xfa: printf("JM adr       \n"); break;
	case 0xfb: printf("EI	1       \n"); break;
	case 0xfc: printf("CM adr       \n"); break;
	case 0xfd: printf("-	        \n"); break;
	case 0xfe: printf("CPI D8       \n"); break;
	case 0xff: printf("RST 7        \n"); break;  
    }
    printf("\x1b[0m"); 
}

void print_state(cpu *cpu)
{
    /*printf("\na %#04x ",cpu->a);
    printf("b %#04x ",cpu->b);
    printf("c %#04x ",cpu->c);
    printf("d %#04x ",cpu->d);
    printf("e %#04x ",cpu->e);
    printf("h %#04x ",cpu->h);
    printf("l %#04x ",cpu->l);
    printf("sp %#04x\n",cpu->sp);

    printf("z: %d ", cpu->flags->z); 
    printf("p: %d ", cpu->flags->p);
    printf("s: %d ", cpu->flags->s);
    printf("c: %d\n", cpu->flags->c);*/
    
    printf("A    B    C    D    E    H    L     SP\n");
    printf("%#04x"   ,cpu->a);
    printf(" %#04x"  ,cpu->b);
    printf(" %#04x"  ,cpu->c);
    printf(" %#04x"  ,cpu->d);
    printf(" %#04x"  ,cpu->e);
    printf(" %#04x"  ,cpu->h);
    printf(" %#04x"  ,cpu->l);
    printf(" %#04x\n",cpu->sp);

    printf("flags:\n");
    printf("\tZ: %s\n", cpu->flags->z ? "true":"false");
    printf("\tP: %s\n", cpu->flags->p ? "true":"false");
    printf("\tS: %s\n", cpu->flags->s ? "true":"false");
    printf("\tCY: %s\n",cpu->flags->c ? "true":"false");   

}

#define CPUER 0x589
void cpu_diag_call(cpu *cpu)
{
    uint8_t byte_low, byte_high;
    get_next_pc_bytes(cpu, &byte_low, &byte_high);
    
    if(CPUER == join(byte_high, byte_low)) printf("-- BREAKPOINT --\n\n");
    if(5 ==  join(byte_high, byte_low))    
    {    
	if (cpu->c == 9)    
	{    
	    uint16_t i = join(cpu->d, cpu->e);

	    while(cpu->rom[i] != '$')    
	    {
		printf("%c", cpu->rom[i]);    
		i++;
	    }
            printf("\n");     

	    exit(0);
	}    
	else if (cpu->c == 2) printf("print char routine called\n");    
    }    
    else if (0 ==  (join(byte_high, byte_low))) exit(0);    
    else {--cpu->pc; --cpu->pc; call(cpu, NO_COND);}
}







