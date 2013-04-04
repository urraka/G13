#pragma once

class SpriteBatch
{
public:
	~SpriteBatch();

	void clear();
	void add(const Sprite &sprite);
	size_t size() const;
	void texture(Texture *texture);
	Texture *texture() const;

private:
	SpriteBatch(Graphics *graphics);
	void create(size_t maxSize);
	void draw(size_t offset, size_t count);

	Graphics *graphics_;
	Texture *texture_;
	VBO<TextureVertex> *buffer_;
	size_t size_;
	size_t maxSize_;

	friend class Graphics;
};
