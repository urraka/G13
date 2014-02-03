#pragma once

#include "gl.h"
#include "enums.h"
#include "vertex.h"
#include "attributes.h"
#include "Color.h"
#include "IBO.h"
#include "VBO.h"
#include "Shader.h"
#include "Image.h"
#include "Texture.h"
#include "Sprite.h"
#include "SpriteBatch.h"
#include "Font.h"
#include "Text.h"
#include "mat2d.h"

#include <stddef.h>

namespace gfx {

void  initialize();
void  terminate();
void  viewport(int width, int height, int rotation);
void  clear();
void  bgcolor(Color color);
void  wireframe(bool enable);
void  line_width(float width);
float line_width();

void bind(Shader *shader);
void bind(Texture *texture, int unit = 0);
void bind(VBO *vbo);
void bind(IBO *ibo);

void draw(VBO *vbo);
void draw(VBO *vbo, size_t count);
void draw(VBO *vbo, size_t offset, size_t count);

void draw(SpriteBatch *spriteBatch);
void draw(SpriteBatch *spriteBatch, size_t count);
void draw(SpriteBatch *spriteBatch, size_t offset, size_t count);

void draw(const Sprite &sprite);
void draw(Text *text);

inline void bind(Shader &shader) { bind(&shader); }
inline void bind(Texture &texture, int unit = 0) { bind(&texture, unit); }
inline void bind(VBO &vbo) { bind(&vbo); }
inline void bind(IBO &ibo) { bind(&ibo); }

inline void draw(VBO &vbo) { draw(&vbo); }
inline void draw(VBO &vbo, size_t count) { draw(&vbo, count); }
inline void draw(VBO &vbo, size_t offset, size_t count) { draw(&vbo, offset, count); }

inline void draw(SpriteBatch &batch) { draw(&batch); }
inline void draw(SpriteBatch &batch, size_t count) { draw(&batch, count); }
inline void draw(SpriteBatch &batch, size_t offset, size_t count) { draw(&batch, offset, count); }

inline void draw(Text &text) { draw(&text); }

void matrix(const mat2d &matrix);
const mat2d &matrix();

void identity();
void translate(float x, float y);
void rotate(float angle);
void scale(float x, float y);
void transform(const mat2d &matrix);

template<typename T> const Attributes *attributes();
template<typename T> Shader *default_shader();

ColorVertex  color_vertex(float x, float y, Color color);
SpriteVertex sprite_vertex(float x, float y, float u, float v, Color color);
TextVertex   text_vertex(float x, float y, uint16_t u, uint16_t v);

} // gfx
