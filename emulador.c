#include "emulador.h"

int main()
{
    tests();
    return 0;
}

void tests()
{
    state8080* state = init_machine();
    
    state->registers->A = 0x0f;
    state->registers->B = 0x1f;
    
    print_state(state);

    cmp_register(state, state->registers->B);
    
    print_state(state);
}

void print_state(state8080 *state)
{
    printf("Registers A    B    C    D    E    H    L\n");
    printf("          %#04x",state->registers->A);
    printf(" %#04x",state->registers->B);
    printf(" %#04x",state->registers->C);
    printf(" %#04x",state->registers->D);
    printf(" %#04x",state->registers->E);
    printf(" %#04x",state->registers->H);
    printf(" %#04x\n",state->registers->L);

    printf("flags:\n");
    printf("\tZ: %s\n", state->status_flags->Z ? "true":"false");
    printf("\tP: %s\n", state->status_flags->P ? "true":"false");
    printf("\tS: %s\n", state->status_flags->S ? "true":"false");
    printf("\tCY: %s\n", state->status_flags->CY ? "true":"false");   
}

//MISC

//joins the content of two reg pairs
uint16_t joint(uint8_t rh, uint8_t rl)
{    
    uint16_t byte2 = rh;
    uint16_t byte1 = rl;
    return  byte2 << 8 | byte1;
}

//disjoints the contents of a reg pair 
void disjoint(uint16_t joint, uint8_t *rh, uint8_t *rl)
{
    *rl = (uint8_t)  joint;
    *rh = (uint8_t) (joint >> 8);
}

//gets the address inside hl reg pair
uint16_t get_HL_addr(state8080 *state)
{
    return  joint(state->registers->H, state->registers->L);
}

//gets whatever is in the pc
uint8_t get_PC_data(state8080 *state)
{
    return state->RAM[state->registers->PC];
}

//gets the bytes in the imediate 2byte addr
uint16_t get_bytes_addr(state8080 *state)
{
    state->registers->PC++;
    uint8_t byte2 = get_PC_data(state);

    state->registers->PC++;
    uint8_t byte3 = get_PC_data(state);
   
    return joint(byte3, byte2);
}

void swap(uint8_t *one, uint8_t *two)
{
    uint8_t temp;
    temp = *one;
    *one = *two;
    *two = temp;
}

int is_bit_set(uint8_t byte, int bit)
{
    return byte & (1 << bit);
}

uint8_t set_bit(uint8_t byte, uint8_t bit)
{
    return byte | 0x01 << bit; 
}

int parity(uint8_t num)
{
    return !(num%2);
}

uint8_t set_flags(state8080 *state, uint16_t result)
{
    if((result & 0xff00)) state->status_flags->CY = 1;
    else state->status_flags->CY = 0;

    result = (uint8_t) result;

    if(is_bit_set(result, 7)) state->status_flags->S = 1;
    else state->status_flags->S = 0;

    if(parity(result)) state->status_flags->P = 1;
    else state->status_flags->P = 0;

    if(!result) state->status_flags->Z = 1;
    else state->status_flags->Z = 0;
    
    return result;
}

uint8_t twoscomp(uint8_t num)
{
    return (~num)+1;
}

//INIT
reg* init_reg()
{
    reg* registers = (reg*)malloc(sizeof(reg));
    registers->A = 0;
    registers->B = 0;
    registers->C = 0;
    registers->D = 0;
    registers->E = 0;
    registers->H = 0;
    registers->L = 0;
    registers->PC = 0;
    registers->SP = 0;

    return registers;
}

flags* init_flag()
{
    flags* status_flags = (flags*)malloc(sizeof(reg));
    status_flags->Z = 0;
    status_flags->P = 0;
    status_flags->S = 0;
    status_flags->AC = 0;
    status_flags->CY = 0;

    return status_flags;
}

state8080* init_machine()
{
    state8080 *state = (state8080*)(malloc(sizeof(state8080)));

    state->status_flags = init_flag();
    state->registers = init_reg();

    state->RAM = (uint8_t*)malloc(sizeof(uint8_t)*RAM_SIZE);
    return state;
}


//---------------------ARITHMETIC---------------------------

//ADD r
void add_register(state8080 *state, uint8_t r)
{
    uint16_t result = state->registers->A + r;
    state->registers->A = set_flags(state, result);
}

//ADD M
void add_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint8_t data = state->RAM[addr];

    add_register(state, data);
}

//ADI data
void add_immediate(state8080 *state)
{
    state->registers->PC++;
    uint8_t data = get_PC_data(state);

    add_register(state, data);
}

//ADC r
void add_register_carry(state8080 *state, uint8_t r)
{
    add_register(state, r);
    state->registers->A += state->status_flags->CY;
}

//ADC M
void add_memory_carry(state8080 *state)
{
    add_memory(state);
    state->registers->A += state->status_flags->CY;
}

//ACI data
void add_immediate_carry(state8080 *state)
{
    add_immediate(state);
    state->registers->A += state->status_flags->CY;
}

//TODO this is retarded
//SUB r
void sub_register(state8080 *state, uint8_t r)
{
    r = twoscomp(r);
    add_register(state, r);
}

//SUB M
void sub_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint8_t data = state->RAM[addr];

    sub_register(state, data);
}

//SUI data
void sub_immediate(state8080 *state)
{
    state->registers->PC++;
    uint8_t data = get_PC_data(state);

    sub_register(state, data);
}

//SBB r
void sub_register_borrow(state8080 *state, uint8_t r)
{
    sub_register(state, r);
    state->registers->A -= state->status_flags->CY;
}

//SBB M
void sub_memory_borrow(state8080 *state)
{
    sub_memory(state);
    state->registers->A -= state->status_flags->CY;
}

//SBI data
void sub_immediate_borrow(state8080 *state)
{
    sub_immediate(state);
    state->registers->A -= state->status_flags->CY;
}

//INR r
void inc_register(state8080 *state, uint8_t *r)
{
    uint16_t result = *r + 1;
    *r = set_flags(state, result);
}

//INR M
void inc_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint16_t result = state->RAM[addr] +1;
    state->RAM[addr] = set_flags(state, result);
}

//DCR r
void dec_register(state8080 *state, uint8_t *r)
{
    uint16_t result = *r - 1;
    *r = set_flags(state, result);
}

//DCR M
void dec_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint16_t result = state->RAM[addr] -1;
    state->RAM[addr] = set_flags(state, result);
}

//INX rp
void inc_reg_pair(uint8_t *rh, uint8_t *rl)
{
    uint16_t content = joint(*rh, *rl);
    content++;
    disjoint(content, rh, rl);
}

//DCX rp
void dec_reg_pair(uint8_t *rh, uint8_t *rl)
{
    uint16_t content = joint(*rh, *rl);
    content--;
    disjoint(content, rh, rl);
}

//DAD rp
void add_reg_pair_HL(state8080 *state, uint8_t *rh, uint8_t *rl)
{
    uint16_t hl = joint(state->registers->H, state->registers->L);
    uint16_t rp = joint(*rh, *rl);

    uint32_t result = hl + rp;

    if(result & 0xffff0000) state->status_flags->CY = 1;
    else state->status_flags->CY = 0;

    result = (uint16_t) result;

    disjoint(result, &state->registers->H, &state->registers->L);
}

//-------------------- LOGICAL ------------------------------

//general and thing
void and_thing(state8080 *state, uint8_t var)
{
    uint8_t result = state->registers->A & var;
    set_flags(state, result);
    state->status_flags->CY = 0;
    state->registers->A = result;
}

//ANA r
void and_register(state8080 *state, uint8_t r)
{
    and_thing(state, r);
}

//ANA M
void and_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint8_t data = state->RAM[addr];

    and_thing(state, data);
}

//ANI data
void and_immediate(state8080 *state)
{
    state->registers->PC++;
    uint8_t data = get_PC_data(state);

    and_thing(state, data);
}

//general xor thing
void xor_thing(state8080 *state, uint8_t var)
{
    uint8_t result = state->registers->A ^ var;
    set_flags(state, result);
    state->status_flags->CY = 0;
    state->status_flags->AC = 0;
    state->registers->A = result;
}

//XRA r
void xor_register(state8080 *state, uint8_t r)
{
    xor_thing(state, r);
}

//XRA M
void xor_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint8_t data = state->RAM[addr];

    xor_thing(state, data);
}

//XRI data
void xor_immediate(state8080 *state)
{
    state->registers->PC++;
    uint8_t data = get_PC_data(state);

    xor_thing(state, data);
}

//general or thing
void or_thing(state8080 *state, uint8_t var)
{
    uint8_t result = state->registers->A | var;
    set_flags(state, result);
    state->status_flags->CY = 0;
    state->status_flags->AC = 0;
    state->registers->A = result;
}

//ORA r
void or_register(state8080 *state, uint8_t r)
{
    or_thing(state, r);
}

//ORA M
void or_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint8_t data = state->RAM[addr];

    or_thing(state, data);
}

//ORI data
void or_immediate(state8080 *state)
{
    state->registers->PC++;
    uint8_t data = get_PC_data(state);

    or_thing(state, data);
}

//TODO test this
//general or thing
void cmp_thing(state8080 *state, uint8_t var)
{
    uint16_t result = state->registers->A - var;
    set_flags(state, result);
}

//CMP r
void cmp_register(state8080 *state, uint8_t r)
{
    cmp_thing(state, r);
}

//CMP M
void cmp_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint8_t data = state->RAM[addr];

    cmp_thing(state, data);
}

//CPI data
void cmp_immediate(state8080 *state)
{
    state->registers->PC++;
    uint8_t data = get_PC_data(state);

    cmp_thing(state, data);
}


//general rotate byte
void rotate_byte(state8080 *state, uint8_t cy_set, uint8_t lone_bit, uint8_t shifted)
{
    uint8_t result = (lone_bit | shifted);    
    state->status_flags->CY = !(0 == cy_set);    
    state->registers->A = result;
}

//RLC
void rotate_left(state8080 *state)
{
    uint8_t var = state->registers->A;
    uint8_t lone_bit = (var >> 7);
    rotate_byte(state, lone_bit, lone_bit, (var << 1));
}

//RRC 
void rotate_right(state8080 *state)
{
    uint8_t var = state->registers->A;
    uint8_t lone_bit = (var << 7);
    rotate_byte(state, lone_bit, lone_bit, (var >> 1));
}

//RAL
void rotate_left_carry(state8080 *state)
{
    uint8_t var = state->registers->A;
    uint8_t cy = state->status_flags->CY;
    rotate_byte(state, (var >> 7), cy, (var << 1));
}

//RAR
void rotate_right_carry(state8080 *state)
{
    uint8_t var = state->registers->A;
    uint8_t cy = state->status_flags->CY;
    rotate_byte(state, (var << 7), (cy << 7), (var >> 1));
}

//CMA
void complement_acc(state8080 *state)
{
    state->registers->A = ~(state->registers->A);
}

//CMC
void complement_carry(state8080 *state)
{
    state->status_flags->CY = ~(state->status_flags->CY);
}

//STC
void set_carry(state8080 *state)
{
    state->status_flags->CY = 1;
}

//-------------------- DATA TRANSFER -------------------------

//MOV r1, r2
void move_register(uint8_t *r1, uint8_t *r2)
{
    *r1 = *r2;
}

//MOV r, M
void move_from_mem(state8080 *state, uint8_t *r)
{
    uint16_t addr = get_HL_addr(state);
    *r = state->RAM[addr];
}

//MOV M, r
void move_to_mem(state8080 *state, uint8_t *r)
{
    uint16_t addr = get_HL_addr(state);
    state->RAM[addr] = *r;
}

//MVI r, data
void move_immediate(state8080 *state, uint8_t *r)
{
    state->registers->PC++;
    *r = state->RAM[state->registers->PC];
}

//MVI M, data
void move_to_mem_imed(state8080 *state)
{
    state->registers->PC++;
    uint8_t data = get_PC_data(state);

    uint16_t addr = get_HL_addr(state);
    state->RAM[addr] = data;
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
    uint16_t addr = get_bytes_addr(state);
    state->registers->A = state->RAM[addr];
}

//STA addr
void store_acc_dir(state8080 *state)
{
    uint16_t addr = get_bytes_addr(state);
    state->RAM[addr] = state->registers->A;
}

//LHLD addr
void load_HL_dir(state8080 *state)
{
    uint16_t addr = get_bytes_addr(state);
    
    state->registers->L = addr;
    state->registers->H = addr+1;
}

//SHLD addr
void store_HL_dir(state8080 *state)
{
    uint16_t addr = get_bytes_addr(state);

    state->RAM[addr] =  state->registers->L; 
    state->RAM[(addr+1)] =  state->registers->H; 
}

//LDAX rp
void load_acc_indir(state8080 *state, uint8_t *rh, uint8_t *rl)
{
    uint16_t addr = joint(*rh, *rl);
    state->registers->A = state->RAM[addr];
}

//STAX rp
void store_acc_indir(state8080 *state, uint8_t *rh, uint8_t *rl)
{
    uint16_t addr = joint(*rh, *rl);
    state->RAM[addr] = state->registers->A;
}

//XCHG
void exchange_HL_DE(state8080 *state)
{
    swap(&state->registers->H, &state->registers->D);
    swap(&state->registers->L, &state->registers->E);
}

void command_maker(state8080 *state)
{
    uint8_t *opcode = &state->RAM[state->registers->PC];
    switch(*opcode)
    {
	case 0x00: //NOP 1	    
	    break;

	case 0x01: //LXI B,D16   3	    B <- byte 3, C <- byte 2

	    load_reg_pair_imed(state, &state->registers->B, &state->registers->C);
	    break;

	case 0x02: //STAX B	1	(BC) <- A
	    store_acc_indir(state, &state->registers->B, &state->registers->C);
	    break; 

	case 0x03: //INX B	1	BC <- BC+1
	    break;
	case 0x04: //INR B	1   Z, S, P, AC	B <- B+1
	    break;
	case 0x05: //DCR B	1   Z, S, P, AC	B <- B-1
	    break;
	case 0x06: //MVI B, D8   2	    B <- byte 2

	    move_immediate(state, &state->registers->B);
	    break;

	case 0x07: //RLC 1	CY  A = A << 1; bit 0 = prev bit 7; CY = prev bit 7
	    break;
	case 0x08: //-	    
	    break;
	case 0x09: //DAD B	1   CY	HL = HL + BC
	case 0x0a: //LDAX B	1	A <- (BC)

	    load_acc_indir(state, &state->registers->B, &state->registers->C);
	    break;

	case 0x0b: //DCX B	1	BC = BC-1
	    break;
	case 0x0c: //INR C	1   Z, S, P, AC	C <- C+1
	    break;
	case 0x0d: //DCR C	1   Z, S, P, AC	C <-C-1
	    break;
	case 0x0e: //MVI C,D8    2	    C <- byte 2

	    move_immediate(state, &state->registers->C);
	    break;

	case 0x0f: //RRC 1	CY  A = A >> 1; bit 7 = prev bit 0; CY = prev bit 0
	    break;
	case 0x10: //-	    
	    break;
	case 0x11: //LXI D,D16   3	    D <- byte 3, E <- byte 2

	    load_reg_pair_imed(state, &state->registers->D, &state->registers->E);
	    break;

	case 0x12: //STAX D	1	(DE) <- A

	    store_acc_indir(state, &state->registers->D, &state->registers->E);
	    break;

	case 0x13: //INX D	1	DE <- DE + 1
	    break;
	case 0x14: //INR D	1   Z, S, P, AC	D <- D+1
	    break;
	case 0x15: //DCR D	1   Z, S, P, AC	D <- D-1
	    break;
	case 0x16: //MVI D, D8   2	    D <- byte 2

	    move_immediate(state, &state->registers->D);
	    break;

	case 0x17: //RAL 1	CY  A = A << 1; bit 0 = prev CY; CY = prev bit 7
	    
	    break;
	case 0x18: //-	    
	    
	    break;
	case 0x19: //DAD D	1   CY	HL = HL + DE
	    break;
	case 0x1a: //LDAX D	1	A <- (DE)

	    load_acc_indir(state, &state->registers->D,&state->registers->E);
	    break;

	case 0x1b: //DCX D	1	DE = DE-1
	case 0x1c: //INR E	1   Z, S, P, AC	E <-E+1
	case 0x1d: //DCR E	1   Z, S, P, AC	E <- E-1
	case 0x1e: //MVI E,D8    2	    E <- byte 2

	    move_immediate(state, &state->registers->E);
	    break;

	case 0x1f: //RAR 1	CY  A = A >> 1; bit 7 = prev bit 7; CY = prev bit 0
	case 0x20: //-	    
	case 0x21: //LXI H,D16   3	    H <- byte 3, L <- byte 2

	    load_reg_pair_imed(state, &state->registers->H, &state->registers->L);
	    break;

	case 0x22: //SHLD adr    3	    (adr) <-L; (adr+1)<-H
	case 0x23: //INX H	1	HL <- HL + 1
	case 0x24: //INR H	1   Z, S, P, AC	H <- H+1
	case 0x25: //DCR H	1   Z, S, P, AC	H <- H-1
	case 0x26: //MVI H,D8    2	    H <- byte 2

	    move_immediate(state, &state->registers->H);
	    break;

	case 0x27: //DAA 1	    special
	case 0x28: //-	    
	case 0x29: //DAD H	1   CY	HL = HL + HI
	case 0x2a: //LHLD adr    3	    L <- (adr); H<-(adr+1)

	    load_HL_dir(state);
	    break;

	case 0x2b: //DCX H	1	HL = HL-1
	case 0x2c: //INR L	1   Z, S, P, AC	L <- L+1
	case 0x2d: //DCR L	1   Z, S, P, AC	L <- L-1
	case 0x2e: //MVI L, D8   2	    L <- byte 2

	    move_immediate(state, &state->registers->L);
	    break;

	case 0x2f: //CMA 1	    A <- !A
	case 0x30: //-	    
	case 0x31: //LXI SP, D16 3	    SP.hi <- byte 3, SP.lo <- byte 2
	    
	    //TODO make this shit work
	    break;

	case 0x32: //STA adr	3	(adr) <- A

	    store_acc_dir(state);
	    break;

	case 0x33: //INX SP	1	SP = SP + 1
	case 0x34: //INR M	1   Z, S, P, AC	(HL) <- (HL)+1
	case 0x35: //DCR M	1   Z, S, P, AC	(HL) <- (HL)-1
	case 0x36: //MVI M,D8    2	    (HL) <- byte 2

	    move_to_mem_imed(state);
	    break;

	case 0x37: //STC 1	CY  CY = 1
	case 0x38: //-	    
	case 0x39: //DAD SP	1   CY	HL = HL + SP
	case 0x3a: //LDA adr	3	A <- (adr)

	    load_acc_dir(state);
	    break;

	case 0x3b: //DCX SP	1	SP = SP-1
	case 0x3c: //INR A	1   Z, S, P, AC	A <- A+1
	case 0x3d: //DCR A	1   Z, S, P, AC	A <- A-1
	case 0x3e: //MVI A,D8    2	    A <- byte 2

	    move_immediate(state, &state->registers->A);
	    break;

	case 0x3f: //CMC 1	CY  CY=!CY
	    break;
	case 0x40: //MOV B,B	1	B <- B
	    move_register(&state->registers->B, &state->registers->A);
	    break;

	case 0x41: //MOV B,C	1	B <- C
	    move_register(&state->registers->B, &state->registers->C);
	    break;

	case 0x42: //MOV B,D	1	B <- D
	    move_register(&state->registers->B, &state->registers->D);
	    break;

	case 0x43: //MOV B,E	1	B <- E
	    move_register(&state->registers->B, &state->registers->E);
	    break;

	case 0x44: //MOV B,H	1	B <- H
	    move_register(&state->registers->B, &state->registers->H);
	    break;

	case 0x45: //MOV B,L	1	B <- L
	    move_register(&state->registers->B, &state->registers->L);
	    break;

	case 0x46: //MOV B,M	1	B <- (HL)
	    move_from_mem(state, &state->registers->B);
	    break;

	case 0x47: //MOV B,A	1	B <- A
	    move_register(&state->registers->B, &state->registers->A);
	    break;

	case 0x48: //MOV C,B	1	C <- B
	    move_register(&state->registers->C, &state->registers->B);
	    break;

	case 0x49: //MOV C,C	1	C <- C
	    move_register(&state->registers->C, &state->registers->C);
	    break;

	case 0x4a: //MOV C,D	1	C <- D
	    move_register(&state->registers->C, &state->registers->D);
	    break;

	case 0x4b: //MOV C,E	1	C <- E
	    move_register(&state->registers->C, &state->registers->E);
	    break;

	case 0x4c: //MOV C,H	1	C <- H
	    move_register(&state->registers->C, &state->registers->H);
	    break;

	case 0x4d: //MOV C,L	1	C <- L
	    move_register(&state->registers->C, &state->registers->L);
	    break;

	case 0x4e: //MOV C,M	1	C <- (HL)
	    move_from_mem(state, &state->registers->C);
	    break;

	case 0x4f: //MOV C,A	1	C <- A
	    move_register(&state->registers->C, &state->registers->A);
	    break;

	case 0x50: //MOV D,B	1	D <- B
	    move_register(&state->registers->D, &state->registers->B);
	    break;

	case 0x51: //MOV D,C	1	D <- C
	    move_register(&state->registers->B, &state->registers->A);
	    break;

	case 0x52: //MOV D,D	1	D <- D
	    move_register(&state->registers->B, &state->registers->A);
	    break;

	case 0x53: //MOV D,E	1	D <- E
	    move_register(&state->registers->B, &state->registers->A);
	    break;

	case 0x54: //MOV D,H	1	D <- H
	    move_register(&state->registers->B, &state->registers->A);
	    break;

	case 0x55: //MOV D,L	1	D <- L
	    move_register(&state->registers->B, &state->registers->A);
	    break;

	case 0x56: //MOV D,M	1	D <- (HL)
	    move_register(&state->registers->B, &state->registers->A);
	    break;

	case 0x57: //MOV D,A	1	D <- A
	    move_register(&state->registers->B, &state->registers->A);
	    break;

	case 0x58: //MOV E,B	1	E <- B
	    move_register(&state->registers->B, &state->registers->A);
	    break;

	case 0x59: //MOV E,C	1	E <- C
	case 0x5a: //MOV E,D	1	E <- D
	case 0x5b: //MOV E,E	1	E <- E
	case 0x5c: //MOV E,H	1	E <- H
	case 0x5d: //MOV E,L	1	E <- L
	case 0x5e: //MOV E,M	1	E <- (HL)
	case 0x5f: //MOV E,A	1	E <- A
	case 0x60: //MOV H,B	1	H <- B
	case 0x61: //MOV H,C	1	H <- C
	case 0x62: //MOV H,D	1	H <- D
	case 0x63: //MOV H,E	1	H <- E
	case 0x64: //MOV H,H	1	H <- H
	case 0x65: //MOV H,L	1	H <- L
	case 0x66: //MOV H,M	1	H <- (HL)
	case 0x67: //MOV H,A	1	H <- A
	case 0x68: //MOV L,B	1	L <- B
	case 0x69: //MOV L,C	1	L <- C
	case 0x6a: //MOV L,D	1	L <- D
	case 0x6b: //MOV L,E	1	L <- E
	case 0x6c: //MOV L,H	1	L <- H
	case 0x6d: //MOV L,L	1	L <- L
	case 0x6e: //MOV L,M	1	L <- (HL)
	case 0x6f: //MOV L,A	1	L <- A
	case 0x70: //MOV M,B	1	(HL) <- B
	case 0x71: //MOV M,C	1	(HL) <- C
	case 0x72: //MOV M,D	1	(HL) <- D
	case 0x73: //MOV M,E	1	(HL) <- E
	case 0x74: //MOV M,H	1	(HL) <- H
	case 0x75: //MOV M,L	1	(HL) <- L
	case 0x76: //HLT 1	    special
	case 0x77: //MOV M,A	1	(HL) <- A
	case 0x78: //MOV A,B	1	A <- B
	case 0x79: //MOV A,C	1	A <- C
	case 0x7a: //MOV A,D	1	A <- D
	case 0x7b: //MOV A,E	1	A <- E
	case 0x7c: //MOV A,H	1	A <- H
	case 0x7d: //MOV A,L	1	A <- L
	case 0x7e: //MOV A,M	1	A <- (HL)
	case 0x7f: //MOV A,A	1	A <- A
	case 0x80: //ADD B	1   Z, S, P, CY, AC A <- A + B
	case 0x81: //ADD C	1   Z, S, P, CY, AC A <- A + C
	case 0x82: //ADD D	1   Z, S, P, CY, AC A <- A + D
	case 0x83: //ADD E	1   Z, S, P, CY, AC A <- A + E
	case 0x84: //ADD H	1   Z, S, P, CY, AC A <- A + H
	case 0x85: //ADD L	1   Z, S, P, CY, AC A <- A + L
	case 0x86: //ADD M	1   Z, S, P, CY, AC A <- A + (HL)
	case 0x87: //ADD A	1   Z, S, P, CY, AC A <- A + A
	case 0x88: //ADC B	1   Z, S, P, CY, AC A <- A + B + CY
	case 0x89: //ADC C	1   Z, S, P, CY, AC A <- A + C + CY
	case 0x8a: //ADC D	1   Z, S, P, CY, AC A <- A + D + CY
	case 0x8b: //ADC E	1   Z, S, P, CY, AC A <- A + E + CY
	case 0x8c: //ADC H	1   Z, S, P, CY, AC A <- A + H + CY
	case 0x8d: //ADC L	1   Z, S, P, CY, AC A <- A + L + CY
	case 0x8e: //ADC M	1   Z, S, P, CY, AC A <- A + (HL) + CY
	case 0x8f: //ADC A	1   Z, S, P, CY, AC A <- A + A + CY
	case 0x90: //SUB B	1   Z, S, P, CY, AC A <- A - B
	case 0x91: //SUB C	1   Z, S, P, CY, AC A <- A - C
	case 0x92: //SUB D	1   Z, S, P, CY, AC A <- A + D
	case 0x93: //SUB E	1   Z, S, P, CY, AC A <- A - E
	case 0x94: //SUB H	1   Z, S, P, CY, AC A <- A + H
	case 0x95: //SUB L	1   Z, S, P, CY, AC A <- A - L
	case 0x96: //SUB M	1   Z, S, P, CY, AC A <- A + (HL)
	case 0x97: //SUB A	1   Z, S, P, CY, AC A <- A - A
	case 0x98: //SBB B	1   Z, S, P, CY, AC A <- A - B - CY
	case 0x99: //SBB C	1   Z, S, P, CY, AC A <- A - C - CY
	case 0x9a: //SBB D	1   Z, S, P, CY, AC A <- A - D - CY
	case 0x9b: //SBB E	1   Z, S, P, CY, AC A <- A - E - CY
	case 0x9c: //SBB H	1   Z, S, P, CY, AC A <- A - H - CY
	case 0x9d: //SBB L	1   Z, S, P, CY, AC A <- A - L - CY
	case 0x9e: //SBB M	1   Z, S, P, CY, AC A <- A - (HL) - CY
	case 0x9f: //SBB A	1   Z, S, P, CY, AC A <- A - A - CY
	case 0xa0: //ANA B	1   Z, S, P, CY, AC A <- A & B
	case 0xa1: //ANA C	1   Z, S, P, CY, AC A <- A & C
	case 0xa2: //ANA D	1   Z, S, P, CY, AC A <- A & D
	case 0xa3: //ANA E	1   Z, S, P, CY, AC A <- A & E
	case 0xa4: //ANA H	1   Z, S, P, CY, AC A <- A & H
	case 0xa5: //ANA L	1   Z, S, P, CY, AC A <- A & L
	case 0xa6: //ANA M	1   Z, S, P, CY, AC A <- A & (HL)
	case 0xa7: //ANA A	1   Z, S, P, CY, AC A <- A & A
	case 0xa8: //XRA B	1   Z, S, P, CY, AC A <- A ^ B
	case 0xa9: //XRA C	1   Z, S, P, CY, AC A <- A ^ C
	case 0xaa: //XRA D	1   Z, S, P, CY, AC A <- A ^ D
	case 0xab: //XRA E	1   Z, S, P, CY, AC A <- A ^ E
	case 0xac: //XRA H	1   Z, S, P, CY, AC A <- A ^ H
	case 0xad: //XRA L	1   Z, S, P, CY, AC A <- A ^ L
	case 0xae: //XRA M	1   Z, S, P, CY, AC A <- A ^ (HL)
	case 0xaf: //XRA A	1   Z, S, P, CY, AC A <- A ^ A
	case 0xb0: //ORA B	1   Z, S, P, CY, AC A <- A | B
	case 0xb1: //ORA C	1   Z, S, P, CY, AC A <- A | C
	case 0xb2: //ORA D	1   Z, S, P, CY, AC A <- A | D
	case 0xb3: //ORA E	1   Z, S, P, CY, AC A <- A | E
	case 0xb4: //ORA H	1   Z, S, P, CY, AC A <- A | H
	case 0xb5: //ORA L	1   Z, S, P, CY, AC A <- A | L
	case 0xb6: //ORA M	1   Z, S, P, CY, AC A <- A | (HL)
	case 0xb7: //ORA A	1   Z, S, P, CY, AC A <- A | A
	case 0xb8: //CMP B	1   Z, S, P, CY, AC A - B
	case 0xb9: //CMP C	1   Z, S, P, CY, AC A - C
	case 0xba: //CMP D	1   Z, S, P, CY, AC A - D
	case 0xbb: //CMP E	1   Z, S, P, CY, AC A - E
	case 0xbc: //CMP H	1   Z, S, P, CY, AC A - H
	case 0xbd: //CMP L	1   Z, S, P, CY, AC A - L
	case 0xbe: //CMP M	1   Z, S, P, CY, AC A - (HL)
	case 0xbf: //CMP A	1   Z, S, P, CY, AC A - A
	case 0xc0: //RNZ 1	    if NZ, RET
	case 0xc1: //POP B	1	C <- (sp); B <- (sp+1); sp <- sp+2
	case 0xc2: //JNZ adr	3	if NZ, PC <- adr
	case 0xc3: //JMP adr	3	PC <= adr
	case 0xc4: //CNZ adr	3	if NZ, CALL adr
	case 0xc5: //PUSH B	1	(sp-2)<-C; (sp-1)<-B; sp <- sp - 2
	case 0xc6: //ADI D8	2   Z, S, P, CY, AC A <- A + byte
	case 0xc7: //RST 0	1	CALL $0
	case 0xc8: //RZ  1	    if Z, RET
	case 0xc9: //RET 1	    PC.lo <- (sp); PC.hi<-(sp+1); SP <- SP+2
	case 0xca: //JZ adr	3	if Z, PC <- adr
	case 0xcb: //-	    
	case 0xcc: //CZ adr	3	if Z, CALL adr
	case 0xcd: //CALL adr    3	    (SP-1)<-PC.hi;(SP-2)<-PC.lo;SP<-SP-2;PC=adr
	case 0xce: //ACI D8	2   Z, S, P, CY, AC A <- A + data + CY
	case 0xcf: //RST 1	1	CALL $8
	case 0xd0: //RNC 1	    if NCY, RET
	case 0xd1: //POP D	1	E <- (sp); D <- (sp+1); sp <- sp+2
	case 0xd2: //JNC adr	3	if NCY, PC<-adr
	case 0xd3: //OUT D8	2	special
	case 0xd4: //CNC adr	3	if NCY, CALL adr
	case 0xd5: //PUSH D	1	(sp-2)<-E; (sp-1)<-D; sp <- sp - 2
	case 0xd6: //SUI D8	2   Z, S, P, CY, AC A <- A - data
	case 0xd7: //RST 2	1	CALL $10
	case 0xd8: //RC  1	    if CY, RET
	case 0xd9: //-	    
	case 0xda: //JC adr	3	if CY, PC<-adr
	case 0xdb: //IN D8	2	special
	case 0xdc: //CC adr	3	if CY, CALL adr
	case 0xdd: //-	    
	case 0xde: //SBI D8	2   Z, S, P, CY, AC A <- A - data - CY
	case 0xdf: //RST 3	1	CALL $18
	case 0xe0: //RPO 1	    if PO, RET
	case 0xe1: //POP H	1	L <- (sp); H <- (sp+1); sp <- sp+2
	case 0xe2: //JPO adr	3	if PO, PC <- adr
	case 0xe3: //XTHL	1	L <-> (SP); H <-> (SP+1)
	case 0xe4: //CPO adr	3	if PO, CALL adr
	case 0xe5: //PUSH H	1	(sp-2)<-L; (sp-1)<-H; sp <- sp - 2
	case 0xe6: //ANI D8	2   Z, S, P, CY, AC A <- A & data
	case 0xe7: //RST 4	1	CALL $20
	case 0xe8: //RPE 1	    if PE, RET
	case 0xe9: //PCHL	1	PC.hi <- H; PC.lo <- L
	case 0xea: //JPE adr	3	if PE, PC <- adr
	case 0xeb: //XCHG	1	H <-> D; L <-> E
	case 0xec: //CPE adr	3	if PE, CALL adr
	case 0xed: //-	    
	case 0xee: //XRI D8	2   Z, S, P, CY, AC A <- A ^ data
	case 0xef: //RST 5	1	CALL $28
	case 0xf0: //RP  1	    if P, RET
	case 0xf1: //POP PSW	1	flags <- (sp); A <- (sp+1); sp <- sp+2
	case 0xf2: //JP adr	3	if P=1 PC <- adr
	case 0xf3: //DI  1	    special
	case 0xf4: //CP adr	3	if P, PC <- adr
	case 0xf5: //PUSH PSW    1	    (sp-2)<-flags; (sp-1)<-A; sp <- sp - 2
	case 0xf6: //ORI D8	2   Z, S, P, CY, AC A <- A | data
	case 0xf7: //RST 6	1	CALL $30
	case 0xf8: //RM  1	    if M, RET
	case 0xf9: //SPHL	1	SP=HL
	case 0xfa: //JM adr	3	if M, PC <- adr
	case 0xfb: //EI  1	    special
	case 0xfc: //CM adr	3	if M, CALL adr
	case 0xfd: //-	    
	case 0xfe: //CPI D8	2   Z, S, P, CY, AC A - data
	case 0xff: //RST 7	1	CALL $38
	    break;
    }

}






























