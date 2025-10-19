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
  u16 y = (c8.opcode & OPCODE_MASK_Y) >> 4;
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

bool _draw_pixel(u8 x, u8 y) {
  if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {
    return false;
  }
  u32 index = x + (y * SCREEN_WIDTH);
  c8.gfx[index] ^= 1;
  return !c8.gfx[index];
}

u16 _get_sprite_addr(u8 vx) { return 0; }

void _process_opcode() {

  u16 x, y, n, rr, nnn, sub_r;
  u8 vx, vy, rand_num;
  bool sprite_pixel[8 * 15]; // max sprite size 8x15
  bool overflow;

  switch (c8.opcode & OPCODE_MASK) {
  case _0NNN:
    switch (c8.opcode & OPCODE_MASK_N) {
    case _00E0:
      LOG_INFO("_00E0\n");
      LOG_WARN("clearing screen\n");
      for (u32 i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        c8.gfx[i] = 0;
      }
      c8.draw_flag = true;
      c8.pc += 2;
      break;
    case _00EE:
      sub_r = c8.stack[c8.sp];
      LOG_INFO("_00EE\n");
      LOG_WARN("Return from Subroutine to 0x%X\n", sub_r);
      LOG_INFO("Stack dump:\n");
      for (u32 i = 0; i < 16; i++) {
        LOG_INFO("Stack[%d]: 0x%X\n", i, c8.stack[i]);
        if (i == c8.sp)
          LOG_INFO("  ^ SP\n");
        break;
      }
      c8.sp--;
      c8.pc = sub_r;
      break;
    default:
      LOG_ERROR("Unknown opcode: 0x%X\n", c8.opcode);
      c8.pc += 2;
      break;
    }
    break;

  case _1NNN:
    nnn = _get_NNN();
    LOG_INFO("_1NNN\n");
    LOG_WARN("Jump to address 0x%X\n", nnn);
    c8.pc = nnn;
    break;
  case _2NNN:
    nnn = _get_NNN();
    LOG_INFO("_2NNN\n");
    LOG_WARN("Calling Subroutine at 0x%X\n", nnn);
    c8.sp++;
    c8.stack[c8.sp] = c8.pc;
    c8.pc = nnn;
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
    c8.V[0xF] = 0;

    for (u32 row = 0; row < n; row++) {
      for (u32 col = 0; col < 8; col++) {
        sprite_pixel[row * 8 + col] =
            (c8.memory[c8.I + row] >> (7 - col)) & 0x1;
      }
      LOG_INFO("Sprite row:[ %d,%d,%d,%d,%d,%d,%d,%d]:",
               sprite_pixel[row * 8 + 0], sprite_pixel[row * 8 + 1],
               sprite_pixel[row * 8 + 2], sprite_pixel[row * 8 + 3],
               sprite_pixel[row * 8 + 4], sprite_pixel[row * 8 + 5],
               sprite_pixel[row * 8 + 6], sprite_pixel[row * 8 + 7]);
      LOG_NEWLINE();
    }

    for (u32 row = 0; row < n; row++) {
      for (u32 col = 0; col < 8; col++) {
        if (sprite_pixel[row * 8 + col]) {
          if (_draw_pixel((vx + col) % SCREEN_WIDTH,
                          (vy + row) % SCREEN_HEIGHT)) {
            c8.V[0xF] = 1;
          }
        }
      }
    }

    c8.draw_flag = true;
    c8.pc += 2;
    break;
  case _EXXX:
    switch (c8.opcode & OPCODE_MASK_N) {
    case _EX9E &OPCODE_MASK_N:
      x = _get_x_register();
      vx = c8.V[x];
      LOG_INFO("_EX9E\n");
      LOG_WARN("Skips if the key at V[%d](0x%X) is press \n", x, vx);
      c8.pc += c8.key[vx] ? 4 : 2;
      break;
    case _EXA1 &OPCODE_MASK_N:
      x = _get_x_register();
      vx = c8.V[x];
      LOG_INFO("_EX9E\n");
      LOG_WARN("Skips if the key at V[%d](0x%X) is not press \n", x, vx);
      c8.pc += c8.key[vx] ? 4 : 2;
      break;
    default:
      LOG_ERROR("Unknown opcode: 0x%X\n", c8.opcode);
      c8.pc += 2;
    }
    break;
  case _FXXX:
    switch (c8.opcode & OPCODE_MASK_NN) {
    case _FX07 &OPCODE_MASK_NN:
      x = _get_x_register();
      vx = c8.V[x];
      LOG_INFO("_FX07\n");
      LOG_WARN("Sets V[%d](0x%X) to val of Delay(0x%X)\n", x, vx,
               c8.delay_timer);
      c8.V[x] = c8.delay_timer;
      c8.pc += 2;
      break;
    case _FX0A &OPCODE_MASK_NN:
      x = _get_x_register();
      LOG_INFO("_FX0A\n");
      LOG_WARN("Awaiting key press and storing it in V[%d] \n", x);
      for (u32 i; i < NUMBER_OF_KEYS; i++) {
        if (c8.key[i]) {
          c8.V[x] = i;
          c8.pc += 2;
        }
      }
      break;
    case _FX15 &OPCODE_MASK_NN:
      x = _get_x_register();
      vx = c8.V[x];
      LOG_INFO("_FX15\n");
      LOG_WARN("Setting Delay Timer to V[%d](0x%X)\n", x, vx);
      c8.delay_timer = vx;
      c8.pc += 2;
      break;
    case _FX18 &OPCODE_MASK_NN:
      x = _get_x_register();
      vx = c8.V[x];
      LOG_INFO("_FX15\n");
      LOG_WARN("Setting Sound Timer to V[%d](0x%X)\n", x, vx);
      c8.sound_timer = vx;
      c8.pc += 2;
      break;
    case _FX1E &OPCODE_MASK_NN:
      x = _get_x_register();
      vx = c8.V[x];
      LOG_INFO("_FX1E\n");
      LOG_WARN("Adding V[%d](0x%X) I(0x%X)\n", x, vx, c8.I);
      c8.I += vx;
      LOG_WARN("Result: 0x%X\n", c8.I);
      c8.pc += 2;
      break;
    case _FX29 &OPCODE_MASK_NN:
      x = _get_x_register();
      vx = c8.V[x];
      LOG_INFO("_FX1E\n");
      LOG_WARN("Setting I to the location of sprite in V[%d](0x%X) \n", x, vx);
      LOG_ERROR("Function not yet implemented\n");
      c8.I += _get_sprite_addr(vx);
      LOG_WARN("Result: 0x%X\n", c8.I);
      c8.pc += 2;
      break;
    case _FX33 &OPCODE_MASK_NN:
      x = _get_x_register();
      vx = c8.V[x];
      LOG_INFO("_FX33\n");
      LOG_WARN("Storing V[%d](0x%X) as binary-coded decimal\n", x, vx);
      c8.memory[c8.I] = vx / 100;
      c8.memory[c8.I + 1] = (vx / 10) % 10;
      c8.memory[c8.I + 2] = (vx % 100) % 10;
      c8.pc += 2;
      break;
    case _FX55 &OPCODE_MASK_NN:
      x = _get_x_register();
      vx = c8.V[x];
      LOG_INFO("_FX55\n");
      LOG_WARN("Storing from V0 to V[%d](0x%X) starting at  I(0x%X)\n", x, vx,
               c8.I);

      for (u32 i = 0; i <= x; i++) {
        c8.memory[c8.I + i] = c8.V[i];
      }
      c8.pc += 2;
      break;
    case _FX65 &OPCODE_MASK_NN:
      x = _get_x_register();
      vx = c8.V[x];
      LOG_INFO("_FX65\n");
      LOG_WARN("Loading from memory V0 to V[%d](0x%X) from address  I(0x%X)\n",
               x, vx, c8.I);
      for (u32 i = 0; i <= x; i++) {
        c8.V[i] = c8.memory[c8.I + i];
      }
      c8.pc += 2;
      break;
    default:
      LOG_ERROR("Unknown opcode: 0x%X\n", c8.opcode);
      c8.pc += 2;
      break;
    }
    break;

  default:
    LOG_ERROR("Unknown opcode: 0x%X\n", c8.opcode);
    c8.pc += 2;
    // exit(EXIT_FAILURE);
  }
}
