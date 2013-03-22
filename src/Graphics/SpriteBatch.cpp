#include <pch.h>
#include <Graphics/Graphics.h>

SpriteBatch::SpriteBatch(Graphics *graphics)
	:	graphics_(graphics),
		buffer_(0),
		size_(0),
		maxSize_(0)
{
}

SpriteBatch::~SpriteBatch()
{
	if (buffer_)
		delete buffer_;
}

void SpriteBatch::clear()
{
	size_ = 0;
}

void SpriteBatch::add(const Sprite &sprite)
{
	sprite.vertices(vertices_);
	buffer_->set(vertices_, 4 * size_, 4);
	size_++;
}

void SpriteBatch::create(size_t maxSize)
{
	maxSize_ = maxSize;
	buffer_ = graphics_->buffer(VertexBuffer::kTriangles, VertexBuffer::kDynamicDraw, VertexBuffer::kStaticDraw, 4 * maxSize, 6 * maxSize);

	uint16_t indices[] = { 0, 1, 2, 2, 3, 0};

	for (size_t i = 0; i < maxSize; i++)
	{
		buffer_->set(indices, 6 * i, 6);

		for (size_t j = 0; j < 6; j++)
			indices[j] += 4;
	}
}

void SpriteBatch::draw()
{
	graphics_->draw(buffer_, 0, 6 * size_);
}