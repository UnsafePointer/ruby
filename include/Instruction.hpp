#pragma once
#include <cstdint>
#include "RegisterIndex.hpp"

/*
Primary opcode field (Bit 26..31)
00h=SPECIAL 08h=ADDI  10h=COP0 18h=N/A   20h=LB   28h=SB   30h=LWC0 38h=SWC0
01h=BcondZ  09h=ADDIU 11h=COP1 19h=N/A   21h=LH   29h=SH   31h=LWC1 39h=SWC1
02h=J       0Ah=SLTI  12h=COP2 1Ah=N/A   22h=LWL  2Ah=SWL  32h=LWC2 3Ah=SWC2
03h=JAL     0Bh=SLTIU 13h=COP3 1Bh=N/A   23h=LW   2Bh=SW   33h=LWC3 3Bh=SWC3
04h=BEQ     0Ch=ANDI  14h=N/A  1Ch=N/A   24h=LBU  2Ch=N/A  34h=N/A  3Ch=N/A
05h=BNE     0Dh=ORI   15h=N/A  1Dh=N/A   25h=LHU  2Dh=N/A  35h=N/A  3Dh=N/A
06h=BLEZ    0Eh=XORI  16h=N/A  1Eh=N/A   26h=LWR  2Eh=SWR  36h=N/A  3Eh=N/A
07h=BGTZ    0Fh=LUI   17h=N/A  1Fh=N/A   27h=N/A  2Fh=N/A  37h=N/A  3Fh=N/A

Secondary opcode field (Bit 0..5) (when Primary opcode = 00h)
00h=SLL   08h=JR      10h=MFHI 18h=MULT  20h=ADD  28h=N/A  30h=N/A  38h=N/A
01h=N/A   09h=JALR    11h=MTHI 19h=MULTU 21h=ADDU 29h=N/A  31h=N/A  39h=N/A
02h=SRL   0Ah=N/A     12h=MFLO 1Ah=DIV   22h=SUB  2Ah=SLT  32h=N/A  3Ah=N/A
03h=SRA   0Bh=N/A     13h=MTLO 1Bh=DIVU  23h=SUBU 2Bh=SLTU 33h=N/A  3Bh=N/A
04h=SLLV  0Ch=SYSCALL 14h=N/A  1Ch=N/A   24h=AND  2Ch=N/A  34h=N/A  3Ch=N/A
05h=N/A   0Dh=BREAK   15h=N/A  1Dh=N/A   25h=OR   2Dh=N/A  35h=N/A  3Dh=N/A
06h=SRLV  0Eh=N/A     16h=N/A  1Eh=N/A   26h=XOR  2Eh=N/A  36h=N/A  3Eh=N/A
07h=SRAV  0Fh=N/A     17h=N/A  1Fh=N/A   27h=NOR  2Fh=N/A  37h=N/A  3Fh=N/A

Opcode/Parameter Encoding
31..26 |25..21|20..16|15..11|10..6 |  5..0  |
 6bit  | 5bit | 5bit | 5bit | 5bit |  6bit  |
-------+------+------+------+------+--------+------------
000000 | N/A  | rt   | rd   | imm5 | 0000xx | shift-imm
000000 | rs   | rt   | rd   | N/A  | 0001xx | shift-reg
000000 | rs   | N/A  | N/A  | N/A  | 001000 | jr
000000 | rs   | N/A  | rd   | N/A  | 001001 | jalr
000000 | <-----comment20bit------> | 00110x | sys/brk
000000 | N/A  | N/A  | rd   | N/A  | 0100x0 | mfhi/mflo
000000 | rs   | N/A  | N/A  | N/A  | 0100x1 | mthi/mtlo
000000 | rs   | rt   | N/A  | N/A  | 0110xx | mul/div
000000 | rs   | rt   | rd   | N/A  | 10xxxx | alu-reg
000001 | rs   | 00000| <--immediate16bit--> | bltz
000001 | rs   | 00001| <--immediate16bit--> | bgez
000001 | rs   | 10000| <--immediate16bit--> | bltzal
000001 | rs   | 10001| <--immediate16bit--> | bgezal
00001x | <---------immediate26bit---------> | j/jal
00010x | rs   | rt   | <--immediate16bit--> | beq/bne
00011x | rs   | N/A  | <--immediate16bit--> | blez/bgtz
001xxx | rs   | rt   | <--immediate16bit--> | alu-imm
001111 | N/A  | rt   | <--immediate16bit--> | lui-imm
100xxx | rs   | rt   | <--immediate16bit--> | load rt,[rs+imm]
101xxx | rs   | rt   | <--immediate16bit--> | store rt,[rs+imm]
x1xxxx | <------coprocessor specific------> | coprocessor (see below)

Coprocessor Opcode/Parameter Encoding
31..26 |25..21|20..16|15..11|10..6 |  5..0  |
 6bit  | 5bit | 5bit | 5bit | 5bit |  6bit  |
-------+------+------+------+------+--------+------------
0100nn |0|0000| rt   | rd   | N/A  | 000000 | MFCn rt,rd_dat  ;rt = dat
0100nn |0|0010| rt   | rd   | N/A  | 000000 | CFCn rt,rd_cnt  ;rt = cnt
0100nn |0|0100| rt   | rd   | N/A  | 000000 | MTCn rt,rd_dat  ;dat = rt
0100nn |0|0110| rt   | rd   | N/A  | 000000 | CTCn rt,rd_cnt  ;cnt = rt
0100nn |0|1000|00000 | <--immediate16bit--> | BCnF target ;jump if false
0100nn |0|1000|00001 | <--immediate16bit--> | BCnT target ;jump if true
0100nn |1| <--------immediate25bit--------> | COPn imm25
010000 |1|0000| N/A  | N/A  | N/A  | 000001 | COP0 01h  ;=TLBR
010000 |1|0000| N/A  | N/A  | N/A  | 000010 | COP0 02h  ;=TLBWI
010000 |1|0000| N/A  | N/A  | N/A  | 000110 | COP0 06h  ;=TLBWR
010000 |1|0000| N/A  | N/A  | N/A  | 001000 | COP0 08h  ;=TLBP
010000 |1|0000| N/A  | N/A  | N/A  | 010000 | COP0 10h  ;=RFE
1100nn | rs   | rt   | <--immediate16bit--> | LWCn rt_dat,[rs+imm]
1110nn | rs   | rt   | <--immediate16bit--> | SWCn rt_dat,[rs+imm]
*/
struct Instruction {
    Instruction(uint32_t data);
    ~Instruction();
    uint32_t dat() const;
    uint32_t funct() const;
    uint32_t subfunct() const;
    RegisterIndex rs() const;
    RegisterIndex rt() const;
    RegisterIndex rd() const;
    uint32_t imm() const;
    uint32_t immSE() const;
    uint32_t shiftimm() const;
    uint32_t immjump() const;
    uint32_t copcode() const;
private:
    uint32_t data;
};
