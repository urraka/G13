#pragma once

class VertexBuffer
{
public:
	enum Type
	{
		VBO = 0,
		IBO = 1
	};

	enum Mode
	{
		kPoints = GL_POINTS,
		kLineStrip = GL_LINE_STRIP,
		kLineLoop = GL_LINE_LOOP,
		kLines = GL_LINES,
		kTriangleStrip = GL_TRIANGLE_STRIP,
		kTriangleFan = GL_TRIANGLE_FAN,
		kTriangles = GL_TRIANGLES
	};

	enum Usage
	{
		kNone = 0,
		kStreamDraw = GL_STREAM_DRAW,
		kStaticDraw = GL_STATIC_DRAW,
		kDynamicDraw = GL_DYNAMIC_DRAW
	};

	~VertexBuffer();

	Mode mode() const;
	void mode(Mode mode);
	void set(Vertex *vertices, size_t offset, size_t count);
	void set(uint16_t *indices, size_t offset, size_t count);
	size_t size() const;

private:
	VertexBuffer(Graphics *graphics);
	void create(Mode mode, Usage vboUsage, Usage iboUsage, size_t vboSize, size_t iboSize);
	GLuint id(Type type) const;

	Graphics *graphics_;
	Mode mode_;
	GLuint id_[2];
	size_t size_[2];

	friend class Graphics;
};
