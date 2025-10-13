
#include "window.h"
#include <engine/chip8.h>
#include <gfx/gfx.h>
#include <stdio.h>
#include <stdlib.h>

// global window instance
struct Window window;

bool is_key_pressed(i32 key) {
  if (key < 0 || key >= GLFW_KEY_LAST) {
    return false;
  }
  return (window.keyboard.keys[key].pressed_frame == window.frames);
}
bool is_key_down(i32 key) { return (window.keyboard.keys[key].down); }

static void _key_callback(GLFWwindow *handle, int key, int scancode, int action,
                          int mods) {
  if (key < 0) {
    return;
  }

  switch (action) {
  case GLFW_PRESS:
    window.keyboard.keys[key].last = window.keyboard.keys[key].down;
    window.keyboard.keys[key].down = true;
    if (!window.keyboard.keys[key].last) {
      window.keyboard.keys[key].pressed_frame = window.frames;
    }
    break;
  case GLFW_REPEAT:
    break;
  case GLFW_RELEASE:
    window.keyboard.keys[key].last = window.keyboard.keys[key].down;
    window.keyboard.keys[key].down = false;
    break;
  default:
    break;
  }
}

void _error_callback(i32 error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

void window_create(FWindow init, FWindow destroy, FWindow update,
                   FWindow render) {
  window.init = init;
  window.destroy = destroy;
  window.update = update;
  window.render = render;

  glfwSetErrorCallback(_error_callback);

  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return;
  }

  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  window.size = (uv2){SCREEN_WIDTH * 10, SCREEN_HEIGHT * 10};
  window.handle =
      glfwCreateWindow(window.size.x, window.size.y, "CHIP-8", NULL, NULL);
  if (!window.handle) {
    fprintf(stderr, "Failed to create GLFW window\n");
    glfwTerminate();
    return;
  }
  glfwMakeContextCurrent(window.handle);

  glfwSetKeyCallback(window.handle, _key_callback);

  if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
    fprintf(stderr, "%s", "error initializing GLAD\n");
    glfwTerminate();
    exit(1);
  }

  glfwSwapInterval(1);
}

static void _init(void) { window.init(); }
static void _update(void) {
  glfwPollEvents();
  window.update();
}
static void _render(void) {
  window.frames++;
  window.render();
}

static void _destroy(void) {
  window.destroy();
  glfwTerminate();
}

void window_loop(void) {
  _init();
  while (!glfwWindowShouldClose(window.handle)) {
    _update();
    _render();
    glfwSwapBuffers(window.handle);
    glfwPollEvents();
  }
  _destroy();
  exit(EXIT_SUCCESS);
}
