
#include "glad/gl.h"
#include "util/log.h"
#include <engine/chip8.h>
#include <engine/opcodes.h>
#include <gfx/renderer.h>
#include <gfx/window.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct renderer renderer;
struct Window window;

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

void chip8_init(chip8 *this) {

  renderer_init();
  this->step_engine = false;
  this->draw_flag = false;
  this->pc = START_ADDRESS; // Program counter starts at 0x200
  this->opcode = 0;
  this->I = 0;
  this->sp = -1;
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

  if (this->draw_flag)
    renderer_update_texture(this->gfx, SCREEN_WIDTH, SCREEN_HEIGHT);
  this->draw_flag = false;
  renderer_draw_buffer();
}

void chip8_setKeys(chip8 *this) {
  this->key[0x1] = is_key_down(GLFW_KEY_1);
  this->key[0x2] = is_key_down(GLFW_KEY_2);
  this->key[0x3] = is_key_down(GLFW_KEY_3);
  this->key[0xC] = is_key_down(GLFW_KEY_4);

  this->key[0x4] = is_key_down(GLFW_KEY_Q);
  this->key[0x5] = is_key_down(GLFW_KEY_W);
  this->key[0x6] = is_key_down(GLFW_KEY_E);
  this->key[0xD] = is_key_down(GLFW_KEY_R);

  this->key[0x7] = is_key_down(GLFW_KEY_A);
  this->key[0x8] = is_key_down(GLFW_KEY_S);
  this->key[0x9] = is_key_down(GLFW_KEY_D);
  this->key[0xE] = is_key_down(GLFW_KEY_F);

  this->key[0xA] = is_key_down(GLFW_KEY_Z);
  this->key[0x0] = is_key_down(GLFW_KEY_X);
  this->key[0xB] = is_key_down(GLFW_KEY_C);
  this->key[0xF] = is_key_down(GLFW_KEY_V);
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
  LOG_NEWLINE();
  LOG_GOOD("Program loaded...\n");
}
