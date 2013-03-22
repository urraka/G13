#pragma once

class SpriteBatch
{
public:
	void clear();
	void add(const Sprite &sprite);

	~SpriteBatch();

private:
	SpriteBatch(Graphics *graphics);
	void create(size_t maxSize);
	void draw();

	Graphics *graphics_;
	VertexBuffer *buffer_;
	size_t size_;
	size_t maxSize_;
	Vertex vertices_[4];

	friend class Graphics;
};
