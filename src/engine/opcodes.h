#ifndef OPCODES_H
#define OPCODES_H

#include "chip8.h"

#define OPCODE_MASK_NNN 0x0FFF
#define OPCODE_MASK_NN 0x00FF
#define OPCODE_MASK_RR 0x00FF
#define OPCODE_MASK_N 0x000F
#define OPCODE_MASK_X 0x0F00
#define OPCODE_MASK_Y 0x00F0
#define OPCODE_MASK_NXYN 0xF00F

#define OPCODE_MASK 0xF000

#define M_8BIT_MASK 0b10000000
#define L_8BIT_MASK 0b00000001

// NNN = address
// RR = 8-bit constant
// N = 4-bit constant
// X, Y = 4-bit register identifier
#define _0NNN 0x0000
#define _00E0 0x0000
#define _00EE 0x000E
#define _1NNN 0x1000 // Jump to address NNN
#define _2NNN 0x2000 // Call subroutine at NNN
#define _3XRR 0x3000 // Skip next instruction if VX == RR
#define _4XRR 0x4000 // Skip next instruction if VX == RR
#define _5XY0 0x5000
#define _6XRR 0x6000
#define _7XRR 0x7000
#define _8XY0 0x8000
#define _8XY1 0x8001
#define _8XY2 0x8002
#define _8XY3 0x8003
#define _8XY4 0x8004
#define _8XY5 0x8005
#define _8XY6 0x8006
#define _8XY7 0x8007
#define _8XYE 0x800E
#define _9XY0 0x9000
#define _ANNN 0xA000
#define _BNNN 0xB000
#define _CXRR 0xC000
#define _DXYN 0xD000
#define _EXXX 0xE000
#define _EX9E 0xE00E
#define _EXA1 0xE001
#define _FXXX 0xF000
#define _FX07 0xF007
#define _FX0A 0xF00A
#define _FX15 0xF015
#define _FX18 0xF018
#define _FX1E 0xF01E
#define _FX29 0xF029
#define _FX33 0xF033
#define _FX55 0xF055
#define _FX65 0xF065

void _get_next_opcode(void);
void _process_opcode(void);

#endif
