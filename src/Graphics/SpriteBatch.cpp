#include <System/System.h>
#include <Graphics/Graphics.h>

SpriteBatch::SpriteBatch(Graphics *graphics)
	:	graphics_(graphics),
		texture_(0),
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
	assert(size_ < maxSize_);

	TextureVertex vertices[4];
	sprite.vertices(vertices);
	buffer_->set(vertices, 4 * size_, 4);
	size_++;
}

void SpriteBatch::create(size_t maxSize)
{
	maxSize_ = maxSize;
	buffer_ = graphics_->buffer<TextureVertex>(vbo_t::Triangles, vbo_t::DynamicDraw, vbo_t::StaticDraw, 4 * maxSize, 6 * maxSize);

	uint16_t indices[] = { 0, 1, 2, 2, 3, 0};

	for (size_t i = 0; i < maxSize; i++)
	{
		buffer_->set(indices, 6 * i, 6);

		for (size_t j = 0; j < 6; j++)
			indices[j] += 4;
	}
}

void SpriteBatch::draw(size_t offset, size_t count)
{
	assert(offset + count <= maxSize_);

	graphics_->bind(Graphics::TextureShader);
	graphics_->bind(texture_);
	graphics_->draw(buffer_, 6 * offset, 6 * count);
}

size_t SpriteBatch::size() const
{
	return size_;
}

void SpriteBatch::texture(Texture *texture)
{
	texture_ = texture;
}

Texture *SpriteBatch::texture() const
{
	return texture_;
}
