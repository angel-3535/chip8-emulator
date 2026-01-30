
#include "util/log.h"
#define GLFW_INCLUDE_NONE
#include "gfx/window.h"
#include "util/types.h"
#include <GLFW/glfw3.h>
#include <engine/chip8.h>
#include <stdio.h>
#include <stdlib.h>

// global chip8 instance
chip8 c8;
struct Window window;

void init(void) {
  chip8_init(&c8);
  chip8_loadProgram(&c8, "../breakout.ch8");
}
void destroy(void) { chip8_destroy(&c8); }

void update(void) {
  if (window.keyboard.keys[GLFW_KEY_ESCAPE].down)
    glfwSetWindowShouldClose(window.handle, GLFW_TRUE);

  chip8_setKeys(&c8);
  chip8_emulateCycle(&c8);

  if (is_key_pressed(GLFW_KEY_P)) {
    LOG_INFO("Stack dump:\n");
    for (u32 i = 0; i < 16; i++) {
      LOG_INFO("Stack[%d]: 0x%X\n", i, c8.stack[i]);
      if (i == c8.sp) {
        LOG_INFO("SP = %d\n", c8.sp);
        break;
      }
    }
  }
}

void render(void) { chip8_drawGraphics(&c8); }

i32 main(i32 argc, char *argv[]) {
  window_create(init, destroy, update, render);
  window_loop();

  return EXIT_SUCCESS;
}
