#pragma once

class SpriteBatch
{
public:
	~SpriteBatch();

	void clear();
	void add(const Sprite &sprite);
	size_t size() const;

private:
	SpriteBatch(Graphics *graphics);
	void create(size_t maxSize);
	void draw(size_t offset, size_t count);

	Graphics *graphics_;
	VBO<TextureVertex> *buffer_;
	size_t size_;
	size_t maxSize_;
	TextureVertex vertices_[4];

	friend class Graphics;
};
