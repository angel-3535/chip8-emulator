#include "opcodes.h"
#include "chip8.h"
#include "engine/chip8.h"
#include "util/log.h"
#include <stdio.h>

u16 _get_x_register(chip8 *this) { return this->opcode & OPCODE_MASK_X; }
u16 _get_y_register(chip8 *this) { return this->opcode & OPCODE_MASK_Y; }
u16 _get_NN(chip8 *this) { return this->opcode & OPCODE_MASK_NN; }
u16 _get_RR(chip8 *this) { return this->opcode & OPCODE_MASK_NN; }
u16 _get_NNN(chip8 *this) { return this->opcode & OPCODE_MASK_NNN; }

void _get_next_opcode(chip8 *this) {
  this->opcode = this->memory[this->pc] << 8 | this->memory[this->pc + 1];

  LOG("=====================\n");
  LOG_INFO("PC: 0x%X\n", this->pc);
  LOG_INFO("Opcode: 0x%X\n", this->opcode);
}

void _process_opcode(chip8 *this) {
  switch (this->opcode & OPCODE_MASK) {
  case _0NNN:
    LOG_ERROR("Instruction 0x%X to be implemented", this->opcode);
    this->pc += 2;
    break;

  case _1NNN:
    this->pc = this->opcode & OPCODE_MASK_NNN;
    LOG_WARN("Jump to address 0x%X\n", this->pc - START_ADDRESS);
    break;
  case _2NNN:
    LOG_ERROR("Instruction 0x%X to be implemented", this->opcode);
    this->pc += 2;
    break;
  case _3XRR:
    if (this->V[_get_x_register(this)] == _get_RR(this)) {
      this->pc += 4;
    } else {
      this->pc += 2;
    }
    break;

  case _4XRR:
    if (this->V[_get_x_register(this)] != _get_RR(this)) {
      this->pc += 4;
    } else {
      this->pc += 2;
    }
    break;

  case _5XYO:
    if (this->V[_get_x_register(this)] == _get_y_register(this)) {
      this->pc += 4;
    } else {
      this->pc += 2;
    }

    break;

  case _6XRR:
    this->V[_get_y_register(this)] = _get_RR(this);
    this->pc += 2;
    break;
  case _ANNN:
    this->I = this->opcode & OPCODE_MASK_NNN;
    LOG_WARN("I set to 0x%X\n", this->I);
    this->pc += 2;
    break;
  default:
    LOG_ERROR("Unknown opcode: 0x%X\n", this->opcode);
    this->pc += 2;
    // exit(EXIT_FAILURE);
  }
}
