
#include "engine/chip8.h"
#include "glad/gl.h"
#include <gfx/gfx.h>
#include <gfx/renderer.h>
#include <gfx/window.h>
#include <stdio.h>

struct renderer renderer;
struct Window window;

const char *vertexShaderSource =
    "#version 400 core\n"
    "layout (location = 0) in vec2 a_pos;\n"
    "layout (location = 1) in vec3 a_uv;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(a_pos.x, a_pos.y, 0.0, 1.0);\n"
    "   TexCoord = vec2(a_uv.x, a_uv.y);\n"
    "}\0";

const char *fragmentShaderSource =
    "#version 400 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D ourTexture;\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(ourTexture, TexCoord);\n"
    "   FragColor = vec4(FragColor.r,FragColor.r,FragColor.r, 1.0);\n"
    "}\n\0";

void create_shader(void);
void renderer_init(void) {
  create_shader();
  float quadVertices[] = {
      // positions   // texture coords
      -1.0f, 1.0f,  0.0f, 0.0f, // top left
      -1.0f, -1.0f, 0.0f, 1.0f, // bottom left
      1.0f,  -1.0f, 1.0f, 1.0f, // bottom right
      1.0f,  1.0f,  1.0f, 0.0f  // top right
  };

  unsigned int indices[] = {
      0, 1, 2, // first triangle
      0, 2, 3  // second triangle
  };

  glGenVertexArrays(1, &renderer.VAO);
  glGenBuffers(1, &renderer.VBO);
  glGenBuffers(1, &renderer.EBO);

  glBindVertexArray(renderer.VAO);

  glBindBuffer(GL_ARRAY_BUFFER, renderer.VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Texture coordinate attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glGenTextures(1, &renderer.texture);
  glBindTexture(GL_TEXTURE_2D, renderer.texture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RED,
               GL_UNSIGNED_BYTE, c8.gfx);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void renderer_destroy(void) {
  glDeleteVertexArrays(1, &renderer.VAO);
  glDeleteBuffers(1, &renderer.VBO);
  glDeleteBuffers(1, &renderer.EBO);
  glDeleteProgram(renderer.shaderProgram);
}

void renderer_update_texture(const u8 *buffer, u32 buffer_width,
                             u32 buffer_height) {
  for (u32 i = 0; i < buffer_width * buffer_height; i++) {
    if (buffer[i] != 0) {
      ((u8 *)buffer)[i] = 255;
    }
  }

  glBindTexture(GL_TEXTURE_2D, renderer.texture);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, buffer_width, buffer_height, GL_RED,
                  GL_UNSIGNED_BYTE, buffer);
}

void renderer_draw_buffer(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(renderer.shaderProgram);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, renderer.texture);

  glBindVertexArray(renderer.VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void create_shader(void) {
  u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
  }

  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
  }

  renderer.shaderProgram = glCreateProgram();
  glAttachShader(renderer.shaderProgram, vertexShader);
  glAttachShader(renderer.shaderProgram, fragmentShader);
  glLinkProgram(renderer.shaderProgram);

  glGetProgramiv(renderer.shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(renderer.shaderProgram, 512, NULL, infoLog);
    printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glUseProgram(renderer.shaderProgram);
}
