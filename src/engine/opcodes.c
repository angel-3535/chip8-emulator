#include "opcodes.h"
#include "chip8.h"
#include "engine/chip8.h"
#include "util/log.h"
#include <stdio.h>

struct chip8 c8;

u16 _get_x_register() { return c8.opcode & OPCODE_MASK_X; }
u16 _get_y_register() { return c8.opcode & OPCODE_MASK_Y; }
u16 _get_NN() { return c8.opcode & OPCODE_MASK_NN; }
u16 _get_RR() { return c8.opcode & OPCODE_MASK_NN; }
u16 _get_NNN() { return c8.opcode & OPCODE_MASK_NNN; }

void _get_next_opcode(void) {
  c8.opcode = c8.memory[c8.pc] << 8 | c8.memory[c8.pc + 1];

  LOG("=====================\n");
  LOG_INFO("PC: 0x%X\n", c8.pc);
  LOG_INFO("Opcode: 0x%X\n", c8.opcode);
}

void _process_opcode() {
  switch (c8.opcode & OPCODE_MASK) {
  case _0NNN:
    LOG_ERROR("Instruction 0x%X to be implemented", c8.opcode);
    c8.pc += 2;
    break;

  case _1NNN:
    c8.pc = c8.opcode & OPCODE_MASK_NNN;
    LOG_WARN("Jump to address 0x%X\n", c8.pc - START_ADDRESS);
    break;
  case _2NNN:
    LOG_ERROR("Instruction 0x%X to be implemented", c8.opcode);
    c8.pc += 2;
    break;
  case _3XRR:
    LOG_WARN("Skipping next instruction if V[%X] == 0x%X\n", _get_x_register(),
             _get_RR());
    if (c8.V[_get_x_register()] == _get_RR()) {
      c8.pc += 4;
    } else {
      c8.pc += 2;
    }
    break;

  case _4XRR:
    LOG_WARN("Skipping next instruction if V[%X] != 0x%X\n", _get_x_register(),
             _get_RR());
    if (c8.V[_get_x_register()] != _get_RR()) {
      c8.pc += 4;
    } else {
      c8.pc += 2;
    }
    break;

  case _5XYO:
    LOG_WARN("Skipping next instruction if V[%X] == V[%X]\n", _get_x_register(),
             _get_y_register());
    if (c8.V[_get_x_register()] == _get_y_register()) {
      c8.pc += 4;
    } else {
      c8.pc += 2;
    }

    break;

  case _6XRR:
    LOG_WARN("Setting V[%X] to 0x%X\n", _get_x_register(), _get_RR());
    c8.V[_get_y_register()] = _get_RR();
    c8.pc += 2;
    break;

  case _7XRR:
    c8.V[_get_x_register()] += _get_RR();
    break;
  case _ANNN:
    c8.I = c8.opcode & OPCODE_MASK_NNN;
    LOG_WARN("I set to 0x%X\n", c8.I);
    c8.pc += 2;
    break;
  default:
    LOG_ERROR("Unknown opcode: 0x%X\n", c8.opcode);
    c8.pc += 2;
    // exit(EXIT_FAILURE);
  }
}
