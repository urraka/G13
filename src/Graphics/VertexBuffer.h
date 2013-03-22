#pragma once

class VertexBuffer
{
public:
	enum Type
	{
		VBO,
		IBO
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
		kStreamRead = GL_STREAM_READ,
		kStreamCopy = GL_STREAM_COPY,
		kStaticDraw = GL_STATIC_DRAW,
		kStaticRead = GL_STATIC_READ,
		kStaticCopy = GL_STATIC_COPY,
		kDynamicDraw = GL_DYNAMIC_DRAW,
		kDynamicRead = GL_DYNAMIC_READ,
		kDynamicCopy = GL_DYNAMIC_COPY
	};

	~VertexBuffer();

	void bind();
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
	GLuint vboId_;
	GLuint iboId_;
	size_t vboSize_;
	size_t iboSize_;

	friend class Graphics;
};
