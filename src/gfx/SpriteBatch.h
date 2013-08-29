#pragma once

#include "enums.h"
#include "vertex.h"
#include "Sprite.h"
#include "VBO.h"

#include <stddef.h>
#include <assert.h>

namespace gfx {

class IBO;
class Texture;
class Shader;

class SpriteBatch
{
public:
	SpriteBatch(size_t maxSize, Usage usage = Dynamic);
	~SpriteBatch();

	void clear();
	void resize(size_t maxSize);
	void add(const Sprite &sprite);
	void add(const Sprite *sprites, size_t count);
	void texture(Texture *texture);
	void shader(Shader *shader);

	template<size_t N> inline void add(const Sprite (&sprites)[N])
	{
		assert(size_ + N <= maxSize_);

		SpriteVertex v[N][4];

		for (size_t i = 0; i < N; i++)
			sprites[i].vertices(v[i]);

		vbo_->set(&v[0][0], 4 * size_, 4 * N);
		size_ += N;
	}

	Texture *texture() const;
	size_t size() const;
	size_t capacity() const;

private:
	VBO *vbo_;
	Texture *texture_;
	size_t size_;
	size_t maxSize_;
	Usage usage_;

	// ibo is shared across instances, released when refcount is 0
	static IBO *ibo_;
	static int refcount_;

	void draw(size_t offset, size_t count);

	friend void draw(SpriteBatch *spriteBatch);
	friend void draw(SpriteBatch *spriteBatch, size_t count);
	friend void draw(SpriteBatch *spriteBatch, size_t offset, size_t count);
};

} // gfx
