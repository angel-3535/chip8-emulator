#include "opcodes.h"
#include "chip8.h"
#include "engine/chip8.h"
#include "util/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct chip8 c8;

u16 _get_x_register(void) {
  u16 x = (c8.opcode & OPCODE_MASK_X) >> 8;
  LOG_INFO("accessing register V[%d]\n", x);
  return x;
}
u16 _get_y_register(void) {
  u16 y = (c8.opcode & OPCODE_MASK_X) >> 8;
  LOG_INFO("accessing register V[%d]\n", y);
  return y;
}
u16 _get_N(void) { return c8.opcode & OPCODE_MASK_N; }
u16 _get_NN(void) { return c8.opcode & OPCODE_MASK_NN; }
u16 _get_RR(void) { return c8.opcode & OPCODE_MASK_NN; }
u16 _get_NNN(void) { return c8.opcode & OPCODE_MASK_NNN; }
u8 _get_random_u8(void) {
  u8 r = rand() % 256;
  LOG_INFO("generating random number: 0x%X\n", r);
  return r;
}

void _get_next_opcode(void) {
  c8.opcode = c8.memory[c8.pc] << 8 | c8.memory[c8.pc + 1];

  LOG("=====================\n");
  LOG_INFO("PC: 0x%X\n", c8.pc);
  LOG_INFO("Opcode: 0x%X\n", c8.opcode);
}

void _process_opcode() {

  u16 x, y, n, rr, nnn;
  u8 vx, vy, rand_num;
  bool overflow;

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
    x = _get_x_register();
    rr = _get_RR();
    vx = c8.V[x];
    LOG_INFO("_3XRR\n");
    LOG_WARN("Skipping next instruction if V[%d](0x%X) == 0x%X\n", x, vx, rr);
    if (vx == rr) {
      c8.pc += 4;
    } else {
      c8.pc += 2;
    }
    break;

  case _4XRR:
    x = _get_x_register();
    rr = _get_RR();
    vx = c8.V[x];
    LOG_INFO("_4XRR\n");
    LOG_WARN("Skipping next instruction if V[%d](0x%X) != 0x%X\n", x, vx, rr);
    if (vx != rr) {
      c8.pc += 4;
    } else {
      c8.pc += 2;
    }
    break;

  case _5XY0:
    x = _get_x_register();
    y = _get_y_register();
    vx = c8.V[x];
    vy = c8.V[y];
    LOG_INFO("_5XRR\n");
    LOG_WARN("Skipping next instruction if V[%d](0x%X) == V[%d](0x%X)\n", x, vx,
             y, vy);
    if (vx == vy) {
      c8.pc += 4;
    } else {
      c8.pc += 2;
    }

    break;

  case _6XRR:
    x = _get_x_register();
    vx = c8.V[x];
    rr = _get_RR();
    LOG_INFO("_6XRR\n");
    LOG_WARN("Setting V[%d](0x%X) to 0x%X\n", x, vx, rr);
    c8.V[x] = rr;
    c8.pc += 2;
    break;

  case _7XRR:
    x = _get_x_register();
    vx = c8.V[x];
    rr = _get_RR();
    LOG_INFO("_7XRR\n");
    LOG_WARN("Adding 0x%X to V[%d](0x%X) \n", rr, x, vx);
    c8.V[x] += rr;
    c8.pc += 2;
    LOG_WARN("Result: 0x%X\n", c8.V[x]);
    break;
  case _8XY0:
    switch (c8.opcode & OPCODE_MASK_NXYN) {
    case _8XY0:
      x = _get_x_register();
      y = _get_y_register();
      vx = c8.V[x];
      vy = c8.V[y];
      LOG_INFO("_8XY0\n");
      LOG_WARN("Setting V[%d](0x%X) to  V[%d](0x%X) \n", x, vx, y, vy);
      c8.V[_get_x_register()] = c8.V[_get_y_register()];
      LOG_WARN("Result: 0x%X\n", c8.V[x]);
      c8.pc += 2;
      break;
    case _8XY1:
      x = _get_x_register();
      y = _get_y_register();
      vx = c8.V[x];
      vy = c8.V[y];
      LOG_INFO("_8XY1\n");
      LOG_WARN("Setting V[%d](0x%X) |=  V[%d](0x%X) \n", x, vx, y, vy);
      c8.V[x] |= c8.V[y];
      LOG_WARN("Result: 0x%X\n", c8.V[x]);
      c8.pc += 2;
      break;
    case _8XY2:
      x = _get_x_register();
      y = _get_y_register();
      vx = c8.V[x];
      vy = c8.V[y];
      LOG_INFO("_8XY2\n");
      LOG_WARN("Setting V[%d](0x%X) &=  V[%d](0x%X) \n", x, vx, y, vy);
      c8.V[x] &= c8.V[y];
      LOG_WARN("Result: 0x%X\n", c8.V[x]);
      c8.pc += 2;
      break;
    case _8XY3:
      x = _get_x_register();
      y = _get_y_register();
      vx = c8.V[x];
      vy = c8.V[y];
      LOG_INFO("_8XY3\n");
      LOG_WARN("Setting V[%d](0x%X) ^=  V[%d](0x%X) \n", x, vx, y, vy);
      c8.V[x] ^= c8.V[y];
      LOG_WARN("Result: 0x%X\n", c8.V[x]);
      c8.pc += 2;
      break;
    case _8XY4:
      x = _get_x_register();
      y = _get_y_register();
      vx = c8.V[x];
      vy = c8.V[y];
      overflow = vy > 0xFF - vx;
      LOG_INFO("_8XY4\n");
      LOG_WARN("Setting V[%d](0x%X) +=  V[%d](0x%X) \n", x, vx, y, vy);
      c8.V[x] += vy;
      c8.V[0xF] = overflow ? 1 : 0;
      LOG_WARN("Result: 0x%X\n", c8.V[x]);
      c8.pc += 2;
      break;
    case _8XY5:
      x = _get_x_register();
      y = _get_y_register();
      vx = c8.V[x];
      vy = c8.V[y];
      LOG_INFO("_8XY5\n");
      LOG_WARN("Setting V[%d](0x%X) -=  V[%d](0x%X) \n", x, vx, y, vy);
      c8.V[0xF] = vx >= vy ? 1 : 0;
      c8.V[x] -= vy;
      LOG_WARN("Result: 0x%X\n", c8.V[x]);
      c8.pc += 2;
      break;
    case _8XY6:
      x = _get_x_register();
      vx = c8.V[x];
      LOG_INFO("_8XY6\n");
      LOG_WARN("Shifting >> 1 V[%d](0x%X)\n", x, vx);
      c8.V[x] >>= 1;
      c8.V[0xF] = vx & L_8BIT_MASK;
      LOG_WARN("VF: 0x%X\n", c8.V[0xF]);
      LOG_WARN("Result: 0x%X\n", c8.V[x]);
      c8.pc += 2;
      break;
    case _8XY7:
      x = _get_x_register();
      y = _get_y_register();
      vx = c8.V[x];
      vy = c8.V[y];
      LOG_INFO("_8XY7\n");
      LOG_WARN("Setting V[%d](0x%X) to V[%d](0x%X) - V[%d](0x%X)\n", x, vx, y,
               vy, x, vx);
      c8.V[x] = vy - vx;
      c8.V[0xF] = vy >= vx ? 1 : 0;
      LOG_WARN("Result: 0x%X\n", c8.V[x]);
      c8.pc += 2;
      break;
    case _8XYE:
      x = _get_x_register();
      vx = c8.V[x];
      LOG_INFO("_8XYE\n");
      LOG_WARN("Shifting << 1 V[%d](0x%X)\n", x, vx);
      c8.V[x] <<= 1;
      c8.V[0xF] = vx & M_8BIT_MASK;
      LOG_WARN("Result: 0x%X\n", c8.V[x]);
      c8.pc += 2;
      break;
    default:
      LOG_ERROR("Unknown opcode: 0x%X\n", c8.opcode);
      c8.pc += 2;
      break;
    }
    break;
  case _9XY0:
    x = _get_x_register();
    y = _get_y_register();
    vx = c8.V[x];
    vy = c8.V[y];
    LOG_INFO("_9XY0\n");
    LOG_WARN("Skipping next instruction if V[%d](0x%X) != V[%d](0x%X)\n", x, vx,
             y, vy);
    if (vx != vy) {
      c8.pc += 4;
    } else {
      c8.pc += 2;
    }
    break;
  case _ANNN:
    c8.I = c8.opcode & OPCODE_MASK_NNN;
    LOG_INFO("_ANNN\n");
    LOG_WARN("I set to 0x%X\n", c8.I);
    c8.pc += 2;
    break;
  case _BNNN:
    nnn = _get_NNN();
    LOG_INFO("_BNNN\n");
    LOG_WARN("Jump to address 0x%X + V0(0x%X)\n", nnn, c8.V[0]);
    c8.pc += nnn + c8.V[0];
    break;
  case _CXRR:
    x = _get_x_register();
    vx = c8.V[x];
    rr = _get_RR();
    rand_num = _get_random_u8();
    LOG_INFO("_CXRR\n");
    LOG_WARN("Sets V[%d](0x%X) to (0x%X) & 0x%X\n", x, vx, vx, rand_num);
    c8.V[x] &= rand_num;
    LOG_WARN("Result: 0x%X\n", c8.V[x]);
    c8.pc += 2;
    break;
  case _DXYN:
    x = _get_x_register();
    y = _get_y_register();
    vx = c8.V[x];
    vy = c8.V[y];
    n = _get_N();
    LOG_INFO("_DXYN\n");
    LOG_WARN("Draw sprite at <V[%d](0x%X), V[%d](0x%X)> width 8 height %d\n", x,
             vx, y, vy, n);
    LOG_ERROR("TO BE IMPLEMENTED\n");
    c8.pc += 2;
    break;
  default:
    LOG_ERROR("Unknown opcode: 0x%X\n", c8.opcode);
    c8.pc += 2;
    // exit(EXIT_FAILURE);
  }
}
