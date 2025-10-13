#ifndef GFXRENDERER_H
#define GFXRENDERER_H

#include <util/types.h>
struct renderer {
  u32 shaderProgram;
  u32 VBO, VAO, EBO;
};

extern struct renderer renderer;

void renderer_init(void);
void renderer_destroy(void);

#endif
