#include "hardware.h"
#include "emulador.h"


void command_maker(state8080 *state, port *p)
{
    uint8_t sph, spl;
    uint16_t mem;
    uint8_t opcode = state->RAM[REG->PC];
    switch(opcode)
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
	    inc_reg_pair(&state->registers->B, &state->registers->C);
	    break;

	case 0x04: //INR B	1   Z, S, P, AC	B <- B+1

	    inc_register(state, &REG->B);
	    break;

	case 0x05: //DCR B	1   Z, S, P, AC	B <- B-1

	    dec_register(state, &REG->B);
	    break;
	    
	case 0x06: //MVI B, D8   2	    B <- byte 2

	    move_immediate(state, &state->registers->B);
	    break;

	case 0x07: //RLC 1	CY  A = A << 1; bit 0 = prev bit 7; CY = prev bit 7

	    rotate_left(state);
	    break;

	case 0x08: //-	    
	    break;

	case 0x09: //DAD B	1   CY	HL = HL + BC
	    //TODO
	    add_reg_pair_HL(state, &REG->B, &REG->C);
	    break;

	case 0x0a: //LDAX B	1	A <- (BC)

	    load_acc_indir(state, &state->registers->B, &state->registers->C);
	    break;

	case 0x0b: //DCX B	1	BC = BC-1

	    dec_reg_pair(&REG->B, &REG->C);
	    break;

	case 0x0c: //INR C	1   Z, S, P, AC	C <- C+1
	    inc_register(state, &REG->C);
	    break;

	case 0x0d: //DCR C	1   Z, S, P, AC	C <-C-1

	    dec_register(state, &REG->C);
	    break;

	case 0x0e: //MVI C,D8    2	    C <- byte 2

	    move_immediate(state, &state->registers->C);
	    break;

	case 0x0f: //RRC 1	CY  A = A >> 1; bit 7 = prev bit 0; CY = prev bit 0

	    rotate_right(state);
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

	    inc_reg_pair(&state->registers->D, &state->registers->E);
	    break;

	case 0x14: //INR D	1   Z, S, P, AC	D <- D+1

	    inc_register(state, &REG->D);
	    break;

	case 0x15: //DCR D	1   Z, S, P, AC	D <- D-1

	    dec_register(state, &REG->D);
	    break;

	case 0x16: //MVI D, D8   2	    D <- byte 2

	    move_immediate(state, &state->registers->D);
	    break;

	case 0x17: //RAL 1	CY  A = A << 1; bit 0 = prev CY; CY = prev bit 7
	    rotate_left_carry(state);
	    break;
	case 0x18: //-	    
	    
	    break;
	case 0x19: //DAD D	1   CY	HL = HL + DE

	    add_reg_pair_HL(state, &REG->D, &REG->E);
	    break;

	case 0x1a: //LDAX D	1	A <- (DE)

	    load_acc_indir(state, &state->registers->D, &state->registers->E);
	    break;

	case 0x1b: //DCX D	1	DE = DE-1

	    dec_reg_pair(&REG->B, &REG->C);
	    break;

	case 0x1c: //INR E	1   Z, S, P, AC	E <-E+1

	    inc_register(state, &REG->E);
	    break;

	case 0x1d: //DCR E	1   Z, S, P, AC	E <- E-1
	    dec_register(state, &REG->E);
	case 0x1e: //MVI E,D8    2	    E <- byte 2

	    move_immediate(state, &state->registers->E);
	    break;

	case 0x1f: //RAR 1	CY  A = A >> 1; bit 7 = prev bit 7; CY = prev bit 0
	    rotate_right_carry(state);
	    break;

	case 0x20: //-	    
	    break;

	case 0x21: //LXI H,D16   3	    H <- byte 3, L <- byte 2

	    load_reg_pair_imed(state, &state->registers->H, &state->registers->L);
	    break;

	case 0x22: //SHLD adr    3	    (adr) <-L; (adr+1)<-H

	    store_HL_dir(state);
	    break;

	case 0x23:

	    inc_reg_pair(&REG->H, &REG->L);
	    break;

	case 0x24: //INR H	1   Z, S, P, AC	H <- H+1
	    
	    inc_register(state, &REG->H);
	    break; 

	case 0x25: //DCR H	1   Z, S, P, AC	H <- H-1

	    dec_register(state, &REG->D);
	    break;

	case 0x26: //MVI H,D8    2	    H <- byte 2

	    move_immediate(state, &state->registers->H);
	    break;

	case 0x27: //DAA 1	    special
	    break;


	case 0x28: //-	    
	    break;

	case 0x29: //DAD H	1   CY	HL = HL + HI

	    add_reg_pair_HL(state, &REG->H, &REG->L);
	    break;

	case 0x2a: //LHLD adr    3	    L <- (adr); H<-(adr+1)

	    load_HL_dir(state);
	    break;

	case 0x2b: //DCX H	1	HL = HL-1
	    
	    dec_reg_pair(&REG->B, &REG->C);
	    break;

	case 0x2c: //INR L	1   Z, S, P, AC	L <- L+1

	    inc_register(state, &REG->L);
	    break;

	case 0x2d: //DCR L	1   Z, S, P, AC	L <- L-1

	    dec_register(state, &REG->L);
	    break;

	case 0x2e: //MVI L, D8   2	    L <- byte 2

	    move_immediate(state, &state->registers->L);
	    break;

	case 0x2f: //CMA 1	    A <- !A

	    complement_acc(state);
	    break;

	case 0x30: //-	    
	    break;

	case 0x31: //LXI SP, D16 3	    SP.hi <- byte 3, SP.lo <- byte 2
	    
	    disjoint(state->registers->SP, &sph, &spl);
	    load_reg_pair_imed(state, &sph, &spl);
	    state->registers->SP = joint(sph, spl);
	    break;

	case 0x32: //STA adr	3	(adr) <- A

	    store_acc_dir(state);
	    break;

	case 0x33: //INX SP	1	SP = SP + 1

	    disjoint(state->registers->SP, &sph, &spl);
	    inc_reg_pair(&sph, &spl);

	    state->registers->SP = joint(sph, spl);

	    break;

	case 0x34: //INR M	1   Z, S, P, AC	(HL) <- (HL)+1

	    inc_memory(state);
	    break;

	case 0x35: //DCR M	1   Z, S, P, AC	(HL) <- (HL)-1

	    dec_memory(state);
	    break;

	case 0x36: //MVI M,D8    2	    (HL) <- byte 2

	    move_to_mem_imed(state);
	    break;

	case 0x37: //STC 1	CY  CY = 1

	    set_carry(state);
	    break;

	case 0x38: //-	    
	    break;

	case 0x39: //DAD SP	1   CY	HL = HL + SP

	    disjoint(state->registers->SP, &sph, &spl);
	    add_reg_pair_HL(state, &sph, &spl);

	    break;
	case 0x3a: //LDA adr	3	A <- (adr)

	    load_acc_dir(state);
	    break;

	case 0x3b: //DCX SP	1	SP = SP-1

	    disjoint(state->registers->SP, &sph, &spl);
	    dec_reg_pair(&sph, &spl);
	    state->registers->SP = joint(sph, spl);
	    break;

	case 0x3c: //INR A	1   Z, S, P, AC	A <- A+1

	    inc_register(state, &REG->A);
	    break;
	    
	case 0x3d: //DCR A	1   Z, S, P, AC	A <- A-1

	    dec_register(state, &REG->D);
	    break;

	case 0x3e: //MVI A,D8    2	    A <- byte 2

	    move_immediate(state, &state->registers->A);
	    break;

	case 0x3f: //CMC 1	CY  CY=!CY

	    complement_carry(state);
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
	    move_register(&state->registers->D, &state->registers->C);
	    break;

	case 0x52: //MOV D,D	1	D <- D
	    move_register(&state->registers->D, &state->registers->D);
	    break;

	case 0x53: //MOV D,E	1	D <- E
	    move_register(&state->registers->D, &state->registers->E);
	    break;

	case 0x54: //MOV D,H	1	D <- H
	    move_register(&state->registers->D, &state->registers->H);
	    break;

	case 0x55: //MOV D,L	1	D <- L
	    move_register(&state->registers->D, &state->registers->L);
	    break;

	case 0x56: //MOV D,M	1	D <- (HL)
	    move_from_mem(state, &state->registers->D);
	    break;

	case 0x57: //MOV D,A	1	D <- A
	    move_register(&state->registers->D, &state->registers->A);
	    break;

	case 0x58: //MOV E,B	1	E <- B
	    move_register(&REG->E, &REG->B);
	    break;

	case 0x59: //MOV E,C	1	E <- C
	    move_register(&REG->E, &REG->C);
	    break;

	case 0x5a: //MOV E,D	1	E <- D
	    move_register(&REG->E, &REG->D);
	    break;

	case 0x5b: //MOV E,E	1	E <- E
	    move_register(&REG->E, &REG->E);
	    break;

	case 0x5c: //MOV E,H	1	E <- H
	    move_register(&REG->E, &REG->H);
	    break;

	case 0x5d: //MOV E,L	1	E <- L
	    move_register(&REG->E, &REG->L);
	    break;

	case 0x5e: //MOV E,M	1	E <- (HL)
	    move_from_mem(state, &REG->E);
	    break;

	case 0x5f: //MOV E,A	1	E <- A
	    move_register(&REG->E, &REG->A);
	    break;

	case 0x60: //MOV H,B	1	H <- B
	    move_register(&REG->H, &REG->B);
	    break;

	case 0x61: //MOV H,C	1	H <- C
	    move_register(&REG->H, &REG->C);
	    break;

	case 0x62: //MOV H,D	1	H <- D
	    move_register(&REG->H, &REG->D);
	    break;

	case 0x63: //MOV H,E	1	H <- E
	    move_register(&REG->H, &REG->E);
	    break;

	case 0x64: //MOV H,H	1	H <- H
	    move_register(&REG->H, &REG->H);
	    break;

	case 0x65: //MOV H,L	1	H <- L
	    move_register(&REG->H, &REG->L);
	    break;

	case 0x66: //MOV H,M	1	H <- (HL)
	    move_from_mem(state, &REG->H);
	    break;

	case 0x67: //MOV H,A	1	H <- A
	    move_register(&REG->H, &REG->A);
	    break;

	case 0x68: //MOV L,B	1	L <- B
	    move_register(&REG->L, &REG->B);
	    break;

	case 0x69: //MOV L,C	1	L <- C
	    move_register(&REG->L, &REG->C);
	    break;

	case 0x6a: //MOV L,D	1	L <- D
	    move_register(&REG->L, &REG->D);
	    break;

	case 0x6b: //MOV L,E	1	L <- E
	    move_register(&REG->L, &REG->E);
	    break;

	case 0x6c: //MOV L,H	1	L <- H
	    move_register(&REG->L, &REG->H);
	    break;

	case 0x6d: //MOV L,L	1	L <- L
	    move_register(&REG->L, &REG->L);
	    break;

	case 0x6e: //MOV L,M	1	L <- (HL)
	    move_from_mem(state, &REG->L);
	    break;

	case 0x6f: //MOV L,A	1	L <- A
	    move_register(&REG->L, &REG->A);
	    break;

	case 0x70: //MOV M,B	1	(HL) <- B
	    move_to_mem(state, &REG->B);
	    break;

	case 0x71: //MOV M,C	1	(HL) <- C
	    move_to_mem(state, &REG->C);
	    break;

	case 0x72: //MOV M,D	1	(HL) <- D
	    move_to_mem(state, &REG->D);
	    break;

	case 0x73: //MOV M,E	1	(HL) <- E
	    move_to_mem(state, &REG->E);
	    break;

	case 0x74: //MOV M,H	1	(HL) <- H
	    move_to_mem(state, &REG->H);
	    break;

	case 0x75: //MOV M,L	1	(HL) <- L
	    move_to_mem(state, &REG->L);
	    break;

	case 0x76: //HLT 1	    special

	    halt(state);
	    break;

	case 0x77: //MOV M,A	1	(HL) <- A

	    move_to_mem(state, &REG->A);
	    break;

	case 0x78: //MOV A,B	1	A <- B
	    move_register(&REG->A, &REG->B);
	    break;

	case 0x79: //MOV A,C	1	A <- C
	    move_register(&REG->A, &REG->C);
	    break;

	case 0x7a: //MOV A,D	1	A <- D
	    move_register(&REG->A, &REG->D);
	    break;

	case 0x7b: //MOV A,E	1	A <- E
	    move_register(&REG->A, &REG->E);
	    break;

	case 0x7c: //MOV A,H	1	A <- H
	    move_register(&REG->A, &REG->H);
	    break;

	case 0x7d: //MOV A,L	1	A <- L
	    move_register(&REG->A, &REG->L);
	    break;

	case 0x7e: //MOV A,M	1	A <- (HL)
	    move_from_mem(state, &REG->A);
	    break;

	case 0x7f: //MOV A,A	1	A <- A
	    move_register(&REG->A, &REG->A);
	    break;

	case 0x80: //ADD B	1   Z, S, P, CY, AC A <- A + B
	    add_register(state, REG->B);
	    break;

	case 0x81: //ADD C	1   Z, S, P, CY, AC A <- A + C
	    add_register(state, REG->C);
	    break;

	case 0x82: //ADD D	1   Z, S, P, CY, AC A <- A + D
	    add_register(state, REG->D);
	    break;

	case 0x83: //ADD E	1   Z, S, P, CY, AC A <- A + E
	    add_register(state, REG->E);
	    break;

	case 0x84: //ADD H	1   Z, S, P, CY, AC A <- A + H
	    add_register(state, REG->H);
	    break;

	case 0x85: //ADD L	1   Z, S, P, CY, AC A <- A + L
	    add_register(state, REG->L);
	    break;

	case 0x86: //ADD M	1   Z, S, P, CY, AC A <- A + (HL)
	    add_memory(state);
	    break;

	case 0x87: //ADD A	1   Z, S, P, CY, AC A <- A + A
	    add_register(state, REG->A);
	    break;

	case 0x88: //ADC B	1   Z, S, P, CY, AC A <- A + B + CY
	    add_register_carry(state, REG->B);
	    break;

	case 0x89: //ADC C	1   Z, S, P, CY, AC A <- A + C + CY
	    add_register_carry(state, REG->C);
	    break;

	case 0x8a: //ADC D	1   Z, S, P, CY, AC A <- A + D + CY
	    add_register_carry(state, REG->D);
	    break;

	case 0x8b: //ADC E	1   Z, S, P, CY, AC A <- A + E + CY
	    add_register_carry(state, REG->E);
	    break;

	case 0x8c: //ADC H	1   Z, S, P, CY, AC A <- A + H + CY
	    add_register_carry(state, REG->H);
	    break;

	case 0x8d: //ADC L	1   Z, S, P, CY, AC A <- A + L + CY
	    add_register_carry(state, REG->L);
	    break;

	case 0x8e: //ADC M	1   Z, S, P, CY, AC A <- A + (HL) + CY
	    add_memory_carry(state);
	    break;

	case 0x8f: //ADC A	1   Z, S, P, CY, AC A <- A + A + CY
	    add_register_carry(state, REG->A);
	    break;

	case 0x90: //SUB B	1   Z, S, P, CY, AC A <- A - B
	    sub_register(state, REG->B);
	    break;

	case 0x91: //SUB C	1   Z, S, P, CY, AC A <- A - C
	    sub_register(state, REG->C);
	    break;

	case 0x92: //SUB D	1   Z, S, P, CY, AC A <- A + D
	    sub_register(state, REG->D);
	    break;

	case 0x93: //SUB E	1   Z, S, P, CY, AC A <- A - E
	    sub_register(state, REG->E);
	    break;

	case 0x94: //SUB H	1   Z, S, P, CY, AC A <- A + H
	    sub_register(state, REG->H);
	    break;

	case 0x95: //SUB L	1   Z, S, P, CY, AC A <- A - L
	    sub_register(state, REG->L);
	    break;

	case 0x96: //SUB M	1   Z, S, P, CY, AC A <- A + (HL)
	    sub_memory(state);
	    break;

	case 0x97: //SUB A	1   Z, S, P, CY, AC A <- A - A
	    sub_register(state, REG->A);
	    break;

	case 0x98: //SBB B	1   Z, S, P, CY, AC A <- A - B - CY
	    sub_register_borrow(state, REG->B);
	    break;

	case 0x99: //SBB C	1   Z, S, P, CY, AC A <- A - C - CY
	    sub_register_borrow(state, REG->C);
	    break;

	case 0x9a: //SBB D	1   Z, S, P, CY, AC A <- A - D - CY
	    sub_register_borrow(state, REG->D);
	    break;

	case 0x9b: //SBB E	1   Z, S, P, CY, AC A <- A - E - CY
	    sub_register_borrow(state, REG->E);
	    break;

	case 0x9c: //SBB H	1   Z, S, P, CY, AC A <- A - H - CY
	    sub_register_borrow(state, REG->H);
	    break;

	case 0x9d: //SBB L	1   Z, S, P, CY, AC A <- A - L - CY
	    sub_register_borrow(state, REG->L);
	    break;

	case 0x9e: //SBB M	1   Z, S, P, CY, AC A <- A - (HL) - CY
	    sub_memory_borrow(state);
	    break;

	case 0x9f: //SBB A	1   Z, S, P, CY, AC A <- A - A - CY
	    sub_register_borrow(state, REG->A);
	    break;

	case 0xa0: //ANA B	1   Z, S, P, CY, AC A <- A & B
	    and_register(state, REG->B);
	    break;

	case 0xa1: //ANA C	1   Z, S, P, CY, AC A <- A & C
	    and_register(state, REG->C);
	    break;

	case 0xa2: //ANA D	1   Z, S, P, CY, AC A <- A & D
	    and_register(state, REG->D);
	    break;

	case 0xa3: //ANA E	1   Z, S, P, CY, AC A <- A & E
	    and_register(state, REG->E);
	    break;

	case 0xa4: //ANA H	1   Z, S, P, CY, AC A <- A & H
	    and_register(state, REG->H);
	    break;

	case 0xa5: //ANA L	1   Z, S, P, CY, AC A <- A & L
	    and_register(state, REG->L);
	    break;

	case 0xa6: //ANA M	1   Z, S, P, CY, AC A <- A & (HL)
	    and_memory(state);
	    break;

	case 0xa7: //ANA A	1   Z, S, P, CY, AC A <- A & A
	    and_register(state, REG->A);
	    break;

	case 0xa8: //XRA B	1   Z, S, P, CY, AC A <- A ^ B
	    xor_register(state, REG->B);
	    break;

	case 0xa9: //XRA C	1   Z, S, P, CY, AC A <- A ^ C
	    xor_register(state, REG->C);
	    break;

	case 0xaa: //XRA D	1   Z, S, P, CY, AC A <- A ^ D
	    xor_register(state, REG->D);
	    break;

	case 0xab: //XRA E	1   Z, S, P, CY, AC A <- A ^ E
	    xor_register(state, REG->E);
	    break;

	case 0xac: //XRA H	1   Z, S, P, CY, AC A <- A ^ H
	    xor_register(state, REG->H);
	    break;

	case 0xad: //XRA L	1   Z, S, P, CY, AC A <- A ^ L
	    xor_register(state, REG->L);
	    break;

	case 0xae: //XRA M	1   Z, S, P, CY, AC A <- A ^ (HL)
	    xor_memory(state);
	    break;

	case 0xaf: //XRA A	1   Z, S, P, CY, AC A <- A ^ A
	    xor_register(state, REG->A);
	    break;

	case 0xb0: //ORA B	1   Z, S, P, CY, AC A <- A | B
	    or_register(state, REG->B);
	    break;

	case 0xb1: //ORA C	1   Z, S, P, CY, AC A <- A | C
	    or_register(state, REG->C);
	    break;

	case 0xb2: //ORA D	1   Z, S, P, CY, AC A <- A | D
	    or_register(state, REG->D);
	    break;

	case 0xb3: //ORA E	1   Z, S, P, CY, AC A <- A | E
	    or_register(state, REG->E);
	    break;

	case 0xb4: //ORA H	1   Z, S, P, CY, AC A <- A | H
	    or_register(state, REG->H);
	    break;

	case 0xb5: //ORA L	1   Z, S, P, CY, AC A <- A | L
	    or_register(state, REG->L);
	    break;

	case 0xb6: //ORA M	1   Z, S, P, CY, AC A <- A | (HL)
	    or_memory(state);
	    break;

	case 0xb7: //ORA A	1   Z, S, P, CY, AC A <- A | A
	    or_register(state, REG->A);
	    break;

	case 0xb8: //CMP B	1   Z, S, P, CY, AC A - B
	    cmp_register(state, REG->B);
	    break;

	case 0xb9: //CMP C	1   Z, S, P, CY, AC A - C
	    cmp_register(state, REG->C);
	    break;

	case 0xba: //CMP D	1   Z, S, P, CY, AC A - D
	    cmp_register(state, REG->D);
	    break;

	case 0xbb: //CMP E	1   Z, S, P, CY, AC A - E
	    cmp_register(state, REG->E);
	    break;

	case 0xbc: //CMP H	1   Z, S, P, CY, AC A - H
	    cmp_register(state, REG->H);
	    break;

	case 0xbd: //CMP L	1   Z, S, P, CY, AC A - L
	    cmp_register(state, REG->L);
	    break;

	case 0xbe: //CMP M	1   Z, S, P, CY, AC A - (HL)
	    cmp_memory(state);
	    break;

	case 0xbf: //CMP A	1   Z, S, P, CY, AC A - A
	    cmp_register(state, REG->A);
	    break;

	case 0xc0: //RNZ 1	    if NZ, RET

	    cond_ret_op(state, !state->status_flags->Z);
	    break;

	case 0xc1: //POP B	1	C <- (sp); B <- (sp+1); sp <- sp+2

	    pop(state, &REG->B, &REG->C);
	    break;

	case 0xc2: //JNZ adr	3	if NZ, PC <- adr

	    cond_jump(state, !state->status_flags->Z);
	    break;

	case 0xc3: //JMP adr	3	PC <= adr

	    jump(state);
	    break;

	case 0xc4: //CNZ adr	3	if NZ, CALL adr

	    cond_call(state, !state->status_flags->Z);
	    break;

	case 0xc5: //PUSH B	1	(sp-2)<-C; (sp-1)<-B; sp <- sp - 2

	    push(state, REG->B, REG->C);
	    break;

	case 0xc6: //ADI D8	2   Z, S, P, CY, AC A <- A + byte

	    add_immediate(state);
	    break;

	case 0xc7: //RST 0	1	CALL $0
	    
	    restart(state, 2);
	    break;

	case 0xc8: //RZ  1	    if Z, RET

	    cond_ret_op(state, state->status_flags->Z);
	    break;

	case 0xc9: //RET 1	    PC.lo <- (sp); PC.hi<-(sp+1); SP <- SP+2

	    ret_op(state);
	    break;

	case 0xca: //JZ adr	3	if Z, PC <- adr

	    cond_jump(state, state->status_flags->Z);
	    break;

	case 0xcb: //-	    
	    break;

	case 0xcc: //CZ adr	3	if Z, CALL adr

	    cond_call(state, state->status_flags->Z);
	    break;

	case 0xcd: //CALL adr    3	    (SP-1)<-PC.hi;(SP-2)<-PC.lo;SP<-SP-2;PC=ad
	    
	    mem = get_bytes_addr(state);
	    if(mem  == 5)
	    {
		if(REG->C == 9)	
		{
		    uint16_t thing = joint(REG->D, REG->E);
		    printf("thin %hd\n", thing);
		    thing += 0x300;
		    char *str = &state->RAM[thing];
		    while(*str != '$')
		    {
			printf("%c", *str);
			str++;
		    }
		    printf("\n");
		}
	    }
	    else if (0 ==  mem)    
	    {    
		printf("ehehehehehehehehe\n");
		exit(0);    
	    } 
	    else
	    {
		REG->PC -= 2;
		call(state);
	    }

	    break;

	case 0xce: //ACI D8	2   Z, S, P, CY, AC A <- A + data + CY

	    add_immediate_carry(state);
	    break; 

	case 0xcf: //RST 1	1	CALL $8
	    
	    restart(state, 2);
	    break;

	case 0xd0: //RNC 1	    if NCY, RET

	    cond_ret_op(state, !state->status_flags->CY);
	    break;

	case 0xd1: //POP D	1	E <- (sp); D <- (sp+1); sp <- sp+2

	    pop(state, &REG->B, &REG->C);
	    break;

	case 0xd2: //JNC adr	3	if NCY, PC<-adr

	    cond_jump(state, !state->status_flags->CY);
	    break;

	case 0xd3: //OUT D8	2	special

	   // output(state);
	    REG->PC++; 
	    break;

	case 0xd4: //CNC adr	3	if NCY, CALL adr

	    cond_call(state, !state->status_flags->CY);
	    break;

	case 0xd5: //PUSH D	1	(sp-2)<-E; (sp-1)<-D; sp <- sp - 2

	    push(state, REG->D, REG->E);
	    break;

	case 0xd6: //SUI D8	2   Z, S, P, CY, AC A <- A - data

	    sub_immediate(state);
	    break;

	case 0xd7: //RST 2	1	CALL $10

	    restart(state, 2);
	    break;

	case 0xd8: //RC  1	    if CY, RET

	    cond_ret_op(state, state->status_flags->CY);
	    break;

	case 0xd9: //-	    
	    break;

	case 0xda: //JC adr	3	if CY, PC<-adr

	    cond_jump(state, state->status_flags->CY);
	    break;

	case 0xdb: //IN D8	2	special

	    //TODO
	    REG->PC++; 
	    input(state, p, get_PC_data(state));
	    break;

	case 0xdc: //CC adr	3	if CY, CALL adr

	    cond_call(state, state->status_flags->CY);
	    break;

	case 0xdd: //-	    
	    break;

	case 0xde: //SBI D8	2   Z, S, P, CY, AC A <- A - data - CY

	    sub_immediate_borrow(state);
	    break;

	case 0xdf: //RST 3	1	CALL $18
	    
	    restart(state, 3);
	    break;

	case 0xe0: //RPO 1	    if PO, RET

	    cond_ret_op(state, !state->status_flags->P);
	    break;

	case 0xe1: //POP H	1	L <- (sp); H <- (sp+1); sp <- sp+2
	    
	    pop(state, &REG->H, &REG->L);
	    break;

	case 0xe2: //JPO adr	3	if PO, PC <- adr

	    cond_jump(state, !state->status_flags->P);
	    break;

	case 0xe3: //XTHL	1	L <-> (SP); H <-> (SP+1)

	    exchange_HL_st(state);
	    break;

	case 0xe4: //CPO adr	3	if PO, CALL adr
	    
	    cond_call(state, !state->status_flags->P);
	    break;

	case 0xe5: //PUSH H	1	(sp-2)<-L; (sp-1)<-H; sp <- sp - 2

	    push(state, REG->H, REG->L);
	    break;

	case 0xe6: //ANI D8	2   Z, S, P, CY, AC A <- A & data

	    and_immediate(state);
	    break;

	case 0xe7: //RST 4	1	CALL $20

	    restart(state, 4);
	    break;

	case 0xe8: //RPE 1	    if PE, RET
	
	    cond_ret_op(state, state->status_flags->P);
	    break;

	case 0xe9: //PCHL	1	PC.hi <- H; PC.lo <- L

	    jump_HL_dir(state);
	    break;

	case 0xea: //JPE adr	3	if PE, PC <- adr

	    cond_jump(state, state->status_flags->P);
	    break;

	case 0xeb: //XCHG	1	H <-> D; L <-> E

	    exchange_HL_DE(state);
	    break;

	case 0xec: //CPE adr	3	if PE, CALL adr

	    cond_jump(state, state->status_flags->P);
	    break;
	    
	case 0xed: //-	    
	    break;

	case 0xee: //XRI D8	2   Z, S, P, CY, AC A <- A ^ data

	    xor_immediate(state);
	    break;

	case 0xef: //RST 5	1	CALL $28

	    restart(state, 5);
	    break;

	case 0xf0: //RP  1	    if P, RET

	    cond_ret_op(state, !state->status_flags->S);
	    break;
	    
	case 0xf1: //POP PSW	1	flags <- (sp); A <- (sp+1); sp <- sp+2

	    pop_psw(state);
	    break;

	case 0xf2: //JP adr	3	if P=1 PC <- adr

	    cond_jump(state, !state->status_flags->S);
	    break;

	case 0xf3: //DI  1	    special
	    //TODO
	    disable_inter(state);
	    break;

	case 0xf4: //CP adr	3	if P, PC <- adr

	    cond_call(state, !state->status_flags->S);
	    break;

	case 0xf5: //PUSH PSW    1	    (sp-2)<-flags; (sp-1)<-A; sp <- sp - 2

	    push_psw(state);
	    break;

	case 0xf6: //ORI D8	2   Z, S, P, CY, AC A <- A | data

	    or_immediate(state);
	    break;

	case 0xf7: //RST 6	1	CALL $30
	    
	    restart(state, 6);
	    break;

	case 0xf8: //RM  1	    if M, RET

	    cond_ret_op(state, state->status_flags->S);
	    break;

	case 0xf9: //SPHL	1	SP=HL

	    move_HL_SP(state);
	    break;

	case 0xfa: //JM adr	3	if M, PC <- adr

	    cond_jump(state, state->status_flags->S);
	    break;

	case 0xfb: //EI  1	    special

	    //TODO
	    enable_inter(state);
	    break;

	case 0xfc: //CM adr	3	if M, CALL adr

	    cond_call(state, state->status_flags->S);
	    break;

	case 0xfd: //-	    
	    break;

	case 0xfe: //CPI D8	2   Z, S, P, CY, AC A - data

	    cmp_immediate(state);
	    break;

	case 0xff: //RST 7	1	CALL $38
	    restart(state, 7);
	    break;
	
	default:
	    printf("opcode not found: %#04x\n", opcode);
    }
    
    //printf("opcode %#04x\n", opcode);
    if(state->status_flags->jmp) state->status_flags->jmp = 0;
    else REG->PC++;
}


