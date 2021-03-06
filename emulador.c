#include "emulador.h"

//Temporary, should be changed to a var

#define ROM_MAX 0x1fff

void tests()
{
    state8080* state = init_machine();
    
    state->registers->A = 0x0f;
    state->registers->B = 0x1f;
    
    print_state(state);

    cmp_register(state, state->registers->B);
    
    print_state(state);
}

//checks if memval is smaller than the rom
int rom_lock(state8080 *state, uint16_t memval)
{
    if(memval <= ROM_MAX)
    {
	state->halt = 1;
	printf("SHITS GONE IN THE FAN\n");
	print_state(state);
	printf("trying to write where it shouldnt %#04x\n", memval);
	return 1;
    }
    return 0;
}


void print_state(state8080 *state)
{
    
    printf("opcode %#04x\n", state->RAM[REG->PC]);
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
    flags* status_flags = (flags*)malloc(sizeof(flags));
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

    for(int i = 0; i < RAM_SIZE; i++)
    {
	state->RAM[i] = 0;
    }

    state->SPRAM = (uint8_t*)malloc(sizeof(uint8_t)*SPRAM_SIZE);

    for(int i = 0; i < SPRAM_SIZE; i++)
    {
	state->SPRAM[i] = 0;
    }
    
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
    if(rom_lock(state, addr)) return;
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
    state->registers->A += state->status_flags->CY;
    add_register(state, r);
}

//ADC M
void add_memory_carry(state8080 *state)
{
    state->registers->A += state->status_flags->CY;
    add_memory(state);
}

//ACI data
void add_immediate_carry(state8080 *state)
{
    state->registers->A += state->status_flags->CY;
    add_immediate(state);
}

//TODO this is retarded
//SUB r
void sub_register(state8080 *state, uint8_t r)
{
    uint16_t result = REG->A - r;
    REG->A = set_flags(state, result);
}

//SUB M
void sub_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    if(rom_lock(state, addr)) return;
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
    uint16_t result = REG->A - r - state->status_flags->CY;
    REG->A = set_flags(state, result);
}

//SBB M
void sub_memory_borrow(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);
    uint8_t data = state->RAM[addr];

    sub_register_borrow(state, data);
}

//SBI data
void sub_immediate_borrow(state8080 *state)
{
    REG->PC++;
    uint8_t data = get_PC_data(state);

    sub_register_borrow(state, data);
}

//INR r
void inc_register(state8080 *state, uint8_t *r)
{
    uint8_t result = *r + 0x01;
    *r = set_flags(state, result);
}

//INR M
void inc_memory(state8080 *state)
{
    uint16_t addr = get_HL_addr(state);

    if(rom_lock(state, addr)) return;

    uint8_t result = state->RAM[addr] + 0x01;
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
    
    if(rom_lock(state, addr)) return;

    uint8_t result = state->RAM[addr] -0x01;
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

    if(rom_lock(state, addr)) return;

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
    
    if(rom_lock(state, addr)) return;
    
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

    if(rom_lock(state, addr)) return;

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

    if(rom_lock(state, addr)) return;

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

    //if(rom_lock(state, 0x1fff, addr)) return;
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
//TODO check stack
/*
    shouldn you jump just one on the pc
    when you return why are you uppint 2?
*/
void call(state8080 *state)
{
    uint8_t pch, pcl;
    
    disjoint(state->registers->PC+3, &pch, &pcl);
    push(state, pch, pcl);
    jump(state);
}


//Condition addr
//it doesnt make sense to add two more 
void cond_call(state8080 *state, uint8_t flag)
{
    if(flag) call(state);
    else REG->PC += 2;
}

//RET
//TODO check pc
//this pc is 3 more because we have to skip the 3 bytes of
//the call instruction

void ret_op(state8080 *state)
{
    state->registers->PC = joint(state->RAM[REG->SP+1], state->RAM[REG->SP]);
    REG->SP +=2 ;

    //if(rom_lock(state, 0x1fff, REG->PC)) return;

    state->status_flags->jmp = 1;
    
    //printf("RET PC %#04x\n", REG->PC);
}

//Rcondition
void cond_ret_op(state8080 *state, uint8_t flag)
{
    if(flag) ret_op(state);
}

//RST n
void restart(state8080 *state, uint8_t opcode)
{
    state->inter_stack = REG->SP; 

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
    
    pcl = state->registers->L;
    pch = state->registers->H;
    
    state->registers->PC = joint(pch, pcl);
    state->status_flags->jmp = 1;
}

//--------------------STACK ---------------------------------

//PUSH rp
void push(state8080 *state, uint8_t rh, uint8_t rl)
{
    state->registers->SP--;
    state->RAM[REG->SP] = rh;

    state->registers->SP--;
    state->RAM[REG->SP] = rl;
}

//PUSH PSW
void push_psw(state8080 *state)
{
    uint8_t psw = 0;

    if(state->status_flags->CY) psw = set_bit(psw, 0);
    psw = set_bit(psw, 1);
    if(state->status_flags->P)psw = set_bit(psw, 2);
    if(state->status_flags->AC)psw = set_bit(psw, 4);
    if(state->status_flags->Z)psw = set_bit(psw, 6);
    if(state->status_flags->S)psw = set_bit(psw, 7);

    push(state, state->registers->A, psw);
}

//POP rp
void pop(state8080 *state, uint8_t *rh, uint8_t *rl)
{
    *rl = state->RAM[(REG->SP)];
    state->registers->SP++;
    *rh = state->RAM[(REG->SP)];
    state->registers->SP++;
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
    swap(&state->registers->L, &state->RAM[stack]);
    swap(&state->registers->H, &state->RAM[(stack+1)]);
}

//SPHL
void move_HL_SP(state8080 *state)
{
    state->registers->SP = joint(state->registers->H, state->registers->L);
}

//--------------------- IO ----------------------
//IN port
void input(state8080 *state, port *p)
{
    REG->PC++;  
    uint8_t data = get_PC_data(state);

    state->registers->A = read_i_port(p, data);

    if(data == 0x01)
    {
	    p->input->port1 = 0x0;
    }

    else if(data == 0x02) p->input->port2 = 0x0;
    else if(data == 0x03) p->input->port3 = 0x0;
}

//OUT port
void output(state8080 *state, port *p)
{
    REG->PC++;  
    uint8_t data = get_PC_data(state);

    write_o_port(p, data, REG->A);
}

//EI
void enable_inter(state8080 *state)
{
   //state->inter_ind = 0;
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
    //if(rom_lock(state,addr, 0x1fff)) return;
    *r = state->RAM[addr];
}

//MOV M, r
void move_to_mem(state8080 *state, uint8_t *r)
{
    uint16_t addr = get_HL_addr(state);
    //if(rom_lock(state, addr, 0x1fff)) return;
    state->RAM[addr] = *r;
}

//MVI r, data
void move_immediate(state8080 *state, uint8_t *r)
{
    state->registers->PC++;

    //if(rom_lock(state, 0x1fff, REG->PC)) return;
    *r = state->RAM[state->registers->PC];
}

//MVI M, data
void move_to_mem_imed(state8080 *state)
{
    state->registers->PC++;
    uint8_t data = get_PC_data(state);

    uint16_t addr = get_HL_addr(state);
    //if(rom_lock(state, addr, 0x1fff)) return;
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



