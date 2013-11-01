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

SpriteBatch::SpriteBatch(size_t maxSize, Usage usage)
	:	vbo_(new VBO()),
		texture_(0),
		size_(0),
		maxSize_(maxSize),
		usage_(usage)
{
	if (ibo_ == 0)
		ibo_ = new IBO();

	vbo_ = new VBO(ibo_);

	resize(maxSize);

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

void SpriteBatch::resize(size_t maxSize)
{
	clear();

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

	if (vbo_->size() < maxSize * 4)
		vbo_->allocate<SpriteVertex>(maxSize * 4, usage_);

	maxSize_ = maxSize;
}

void SpriteBatch::add(const Sprite &sprite)
{
	assert(size_ < maxSize_);

	SpriteVertex v[4];
	sprite.vertices(v);
	vbo_->set(v, 4 * size_, 4);
	size_++;
}

void SpriteBatch::add(const Sprite *sprites, size_t count)
{
	assert(size_ + count <= maxSize_);

	const size_t N = 128;
	size_t uploaded = 0;

	SpriteVertex v[N][4];

	while (uploaded < count)
	{
		size_t n = std::min(count - uploaded, N);

		for (size_t i = 0; i < n; i++)
			sprites[uploaded + i].vertices(v[i]);

		vbo_->set(&v[0][0], 4 * size_, 4 * n);

		uploaded += n;
		size_ += n;
	}
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

size_t SpriteBatch::capacity() const
{
	return maxSize_;
}

void SpriteBatch::draw(size_t offset, size_t count)
{
	assert(offset + count <= size_);

	if (count == 0)
		return;

	gfx::bind(texture_);
	gfx::draw(vbo_, 6 * offset, 6 * count);
}

} // gfx
