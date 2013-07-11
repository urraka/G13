#pragma once

#include "Sprite.h"
#include <stddef.h>

namespace gfx {

class IBO;
class VBO;
class Texture;
class Shader;

class SpriteBatch
{
public:
	SpriteBatch(size_t maxSize);
	~SpriteBatch();

	void clear();
	void add(const Sprite &sprite);
	void texture(Texture *texture);
	void shader(Shader *shader);

	Texture *texture() const;
	size_t   size   () const;

private:
	VBO *vbo_;
	Texture *texture_;
	size_t size_;
	size_t maxSize_;

	// ibo is shared across instances, released when refcount is 0
	static IBO *ibo_;
	static int refcount_;

	friend void draw(SpriteBatch *spriteBatch, size_t offset, size_t count);
};

} // gfx
