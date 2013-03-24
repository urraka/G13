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
	VertexBuffer *buffer_;
	size_t size_;
	size_t maxSize_;
	Vertex vertices_[4];

	friend class Graphics;
};
