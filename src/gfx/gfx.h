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

#include <stddef.h>
#include <glm/glm.hpp>

namespace gfx {

extern Shader *ColorShader;
extern Shader *SpriteShader;
extern Shader *TextShader;

void initialize();
void terminate();
void viewport(int width, int height, int rotation);
void clear();
void bgcolor(Color color);
void wireframe(bool enable);

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

void matrix(const glm::mat4 &matrix);
const glm::mat4 &matrix();

void translate(float x, float y);
void rotate(float angle);
void scale(float width, float height);
void transform(const glm::mat4 &matrix);

template<typename T> const Attributes *attributes();
template<typename T> Shader *default_shader();

} // gfx
