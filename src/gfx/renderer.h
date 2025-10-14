#ifndef GFXRENDERER_H
#define GFXRENDERER_H

#include <util/types.h>

struct renderer {
  u32 shaderProgram;
  u32 VBO, VAO, EBO, texture;
};

extern struct renderer renderer;

void renderer_init(void);
void renderer_destroy(void);
void renderer_update_texture(const u8 *buffer, u32 buffer_width,
                             u32 buffer_height);
void renderer_draw_buffer(void);

#endif
