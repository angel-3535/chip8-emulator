
#include "glad/gl.h"
#include "util/log.h"
#include <engine/chip8.h>
#include <engine/opcodes.h>
#include <gfx/renderer.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct renderer renderer;

u8 chip8_fontset[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
void _clear_screen(chip8 *self) {
  for (i32 i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
    self->gfx[i] = 0;
  }
}

void _draw_pixel(chip8 *this, u8 x, u8 y) {
  if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {
    return;
    LOG_ERROR("Pixel out of bounds: (%d, %d)\n", x, y);
  }
  u32 index = x + (y * SCREEN_WIDTH);
  this->V[0xF] = this->gfx[index] & 1; // Set VF to 1 if pixel is erased
  this->gfx[index] = 1;
  LOG_INFO("Drew pixel at (%d, %d)\n", x, y);
}

void _draw_rectangle(chip8 *this, u8 x, u8 y, u8 w, u8 h) {
  for (u8 i = 0; i < w; ++i) {
    for (u8 j = 0; j < h; ++j) {
      _draw_pixel(this, x + i, y + j);
    }
  }
}

void chip8_init(chip8 *this) {
  renderer_init();
  this->pc = START_ADDRESS; // Program counter starts at 0x200
  this->opcode = 0;
  this->I = 0;
  this->sp = 0;
  this->last_byte = 0;

  // clear display
  for (i32 i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
    this->gfx[i] = 0;
  }

  // clear stack
  // clear registers V0-VF
  for (i32 i = 0; i < 16; ++i) {
    this->stack[i] = 0;
    this->V[i] = 0;
    this->key[i] = 0;
  }

  for (i32 i = 0; i < MEMORY_SIZE; ++i) {
    this->memory[i] = 0;
  }
  // clear memory

  for (i32 i = 0; i < FONTSET_SIZE; ++i) {
    this->memory[FONTSET_START_ADDRESS + i] = chip8_fontset[i];
  }
}

void chip8_destroy(chip8 *this) { renderer_destroy(); }

void chip8_emulateCycle(chip8 *this) {
  _get_next_opcode();
  _process_opcode();

  if (this->delay_timer > 0)
    --this->delay_timer;
  if (this->sound_timer > 0) {
    if (this->sound_timer == 1) {
      LOG_INFO("BEEP!\n");
    }
    --this->sound_timer;
  }
}
void chip8_drawGraphics(chip8 *this) {
  renderer_update_texture(this->gfx, SCREEN_WIDTH, SCREEN_HEIGHT);
  renderer_draw_buffer();
}

void chip8_loadProgram(chip8 *this, const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    LOG_ERROR("Failed to open file: %s\n", filename);
    exit(EXIT_FAILURE);
  }

  size_t byte_read =
      fread(&this->memory[START_ADDRESS], 1, END_ADDRESS - START_ADDRESS, file);

  fclose(file);

  for (size_t i = 0; i < byte_read; i += 2) {
    u16 ins = this->memory[START_ADDRESS + i] << 8 |
              this->memory[START_ADDRESS + i + 1];
    printf("0x%X ", ins);
    if ((i / 2 + 1) % 8 == 0)
      printf("\n");
  }
  LOG_NEWLINE()
  LOG_GOOD("Program loaded...\n");
}
