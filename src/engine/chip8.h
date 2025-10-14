#ifndef CHIP8_H
#define CHIP8_H

#include "../util/types.h"
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

#define MEMORY_SIZE 4096
#define START_ADDRESS 0x200
#define END_ADDRESS 0xEA0

#define CALL_STACK_START_ADDRESS 0xEA0
#define CALL_STACK_SIZE 96
#define CALL_STACK_END_ADDRESS 0xEFF

#define FONTSET_START_ADDRESS 0x50
#define FONTSET_SIZE 80

#define NUMBER_OF_KEYS 16

typedef struct chip8 {
  u16 opcode;                           // current opcode
  u8 memory[MEMORY_SIZE];               // 4K memory
  u8 V[16];                             // CPU registers V0 to VF
  u16 I;                                // Index register
  u16 pc;                               // Program counter
  u8 gfx[SCREEN_WIDTH * SCREEN_HEIGHT]; // total pixels: 2048
  u8 delay_timer;                       // delay timer
  u8 sound_timer;                       // sound timer
  u16 stack[16];                        // stack (16 levels)
  u16 sp;                               // stack pointer
  u8 key[NUMBER_OF_KEYS];               // HEX based keypad (0x0-0xF)
  u8 last_byte;
  bool draw_flag;
  bool step_engine;
} chip8;

void chip8_init(chip8 *self);
void chip8_destroy(chip8 *self);
void chip8_emulateCycle(chip8 *self);
void chip8_drawGraphics(chip8 *self);
void chip8_loadProgram(chip8 *self, const char *filename);
void chip8_setKeys(chip8 *self);

// global chip8 instance
extern struct chip8 c8;

#endif // INCLUDE/Users/angel/dev/personal/CHIP-8-emulator/srcchip8chip8.h_
