#include "SpriteBatch.h"
#include "Sprite.h"
#include "IBO.h"
#include "VBO.h"
#include "vertex.h"
#include "gfx.h"
#include <assert.h>

namespace gfx {

IBO *SpriteBatch::ibo_ = 0;
int SpriteBatch::refcount_ = 0;

SpriteBatch::SpriteBatch(size_t maxSize)
	:	vbo_(new VBO()),
		texture_(0),
		size_(0),
		maxSize_(maxSize)
{
	if (ibo_ == 0)
		ibo_ = new IBO();

	if (ibo_->size() < maxSize * 6)
	{
		ibo_->allocate(maxSize * 6, Static);

		uint16_t indices[] = {0, 1, 2, 2, 3, 0};

		for (size_t i = 0; i < maxSize; i++)
		{
			ibo_->set(indices, 6 * i, 6);

			for (size_t j = 0; j < 6; j++)
				indices[j] += 4;
		}
	}

	vbo_ = new VBO(ibo_);
	vbo_->allocate<SpriteVertex>(maxSize * 4, Dynamic);

	refcount_++;
}

SpriteBatch::~SpriteBatch()
{
	refcount_--;

	if (refcount_ == 0)
	{
		delete ibo_;
		ibo_ = 0;
	}

	delete vbo_;
}

void SpriteBatch::clear()
{
	size_ = 0;
}

void SpriteBatch::add(const Sprite &sprite)
{
	assert(size_ < maxSize_);

	SpriteVertex v[4];
	sprite.vertices(v);
	vbo_->set(v, 4 * size_, 4);
	size_++;
}

void SpriteBatch::texture(Texture *texture)
{
	texture_ = texture;
}

void SpriteBatch::shader(Shader *shader)
{
	vbo_->shader(shader);
}

Texture *SpriteBatch::texture() const
{
	return texture_;
}

size_t SpriteBatch::size() const
{
	return size_;
}

void SpriteBatch::draw(size_t offset, size_t count)
{
	assert(offset + count <= size_);

	gfx::bind(texture_);
	gfx::draw(vbo_, 6 * offset, 6 * count);
}

} // gfx
