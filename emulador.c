#include "emulador.h"

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
    printf("A    B    C    D    E    H    L    PC     SP\n");
    printf("%#04x",state->registers->A);
    printf(" %#04x",state->registers->B);
    printf(" %#04x",state->registers->C);
    printf(" %#04x",state->registers->D);
    printf(" %#04x",state->registers->E);
    printf(" %#04x",state->registers->H);
    printf(" %#04x",state->registers->L);
    printf(" %#04x",state->registers->PC);
    printf(" %#04x\n",state->registers->SP);

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
    status_flags->jmp = 0;

    return status_flags;
}

state8080* init_machine()
{
    state8080 *state = (state8080*)(malloc(sizeof(state8080)));

    state->status_flags = init_flag();
    state->registers = init_reg();
    state->halt = 0;
    state->interrupt = 0;

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
    uint8_t result = *r + 1;
    *r = set_flags(state, result);
}

//INR M
void inc_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint8_t result = state->RAM[addr] + 1;
    state->RAM[addr] = set_flags(state, result);
}

//DCR r
void dec_register(state8080 *state, uint8_t *r)
{
    uint8_t result = *r - 0x01;
    *r = set_flags(state, result);
}

//DCR M
void dec_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint8_t result = state->RAM[addr] -1;
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

//-------------------- BRANCH GROUP -------------------------

//JMP addr
//addr-1 because for every intruction loop the pc e increased by 1
void jump(state8080 *state)
{
    state->registers->PC++;
    uint8_t byte2 = get_PC_data(state);

    state->registers->PC++;
    uint8_t byte3 = get_PC_data(state);

    uint16_t addr = joint(byte3, byte2);
    state->registers->PC = addr;
    state->status_flags->jmp = 1;
}

//Jcondition addr
void cond_jump(state8080 *state, uint8_t flag)
{
    if(flag) jump(state);
    else REG->PC += 2;
}

//CALL addr
void call(state8080 *state)
{
    uint16_t stack = state->registers->SP;
    uint8_t pch, pcl;
    
    disjoint(state->registers->PC, &pch, &pcl);
    push(state, pch, pcl);
    jump(state);
}

//Condition addr
void cond_call(state8080 *state, uint8_t flag)
{
    if(flag) call(state);
    else REG->PC += 2;
}

//RET
void ret_op(state8080 *state)
{
    uint16_t stack = state->registers->SP;
    uint8_t pch, pcl;
    
    disjoint(state->registers->PC, &pch, &pcl);

    pop(state, &pch, &pcl);

    state->registers->PC = (joint(pch, pcl));
    state->status_flags->jmp = 1;
    
    REG->PC += 3;

    //printf("RET PC %#04x\n", REG->PC);
}

//Rcondition
void cond_ret_op(state8080 *state, uint8_t flag)
{
    if(flag) ret_op(state);
    else REG->PC += 2;
}

//RST n
void restart(state8080 *state, uint8_t opcode)
{
    uint16_t stack = state->registers->SP;
    uint8_t pch, pcl;
    disjoint(state->registers->PC, &pch, &pcl);

    push(state, pch, pcl);

    state->registers->PC = opcode << 3;
    state->status_flags->jmp = 1;
}

//PCHL
void jump_HL_dir(state8080 *state)
{
    uint8_t pch, pcl;
    
    disjoint(state->registers->PC, &pch, &pcl);
    pcl = state->registers->L;
    pch = state->registers->H;
    
    state->registers->PC = joint(pch, pcl);
    state->status_flags->jmp = 1;
}

//--------------------STACK ---------------------------------

//PUSH rp
void push(state8080 *state, uint8_t rh, uint8_t rl)
{
    uint16_t stack = state->registers->SP;
    
    state->RAM[(stack-1)] = rh;
    state->RAM[(stack-2)] = rl;

    state->registers->SP -= 2;
}

//PUSH PSW
void push_psw(state8080 *state)
{
    uint8_t psw = 0;

    if(state->status_flags->CY) set_bit(psw, 0);
    set_bit(psw, 1);
    if(state->status_flags->P) set_bit(psw, 2);
    if(state->status_flags->AC) set_bit(psw, 4);
    if(state->status_flags->Z) set_bit(psw, 6);
    if(state->status_flags->S) set_bit(psw, 7);

    push(state, state->registers->A, psw);
}

//POP rp
void pop(state8080 *state, uint8_t *rh, uint8_t *rl)
{
    uint16_t stack = state->registers->SP;
    
    *rl = state->RAM[(stack)];
    *rh = state->RAM[(stack+1)];
    
    state->registers->SP += 2;
}

//POP PSW
void pop_psw(state8080 *state)
{
    uint8_t reg_a, psw;
    pop(state, &reg_a, &psw);

    state->registers->A = reg_a;

    if(is_bit_set(psw, 0)) state->status_flags->CY = 1;
    else state->status_flags->CY = 0;

    if(is_bit_set(psw, 2)) state->status_flags->P = 1;
    else state->status_flags->P = 0;

    if(is_bit_set(psw, 4)) state->status_flags->AC = 1;
    else state->status_flags->AC = 0;

    if(is_bit_set(psw, 6)) state->status_flags->Z = 1;
    else state->status_flags->Z = 0;

    if(is_bit_set(psw, 7)) state->status_flags->S = 1;
    else state->status_flags->S = 0;
}

//XTHL
void exchange_HL_st(state8080 *state)
{
    uint16_t stack = state->registers->SP;
    swap(&state->registers->H, &state->RAM[stack]);
    swap(&state->registers->L, &state->RAM[(stack+1)]);
}

//SPHL
void move_HL_SP(state8080 *state)
{
    state->registers->SP = joint(state->registers->H, state->registers->L);
}

//--------------------- IO ----------------------
//IN port
void input(state8080 *state, port *p, uint8_t data)
{
    state->registers->A = read_i_port(p, data);
}

//OUT port
void output(state8080 *state, uint8_t *data)
{
    *data = state->registers->A;
}

//EI
void enable_inter(state8080 *state)
{
    state->interrupt = 1;
}

//DI
void disable_inter(state8080 *state)
{
    state->interrupt = 0;
}

//HLT
void halt(state8080 *state)
{
    state->halt = 1;
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































