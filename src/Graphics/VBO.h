#pragma once

typedef void* vbo_t;

template<class VertexT> class VBO
{
public:
	enum Type
	{
		Vertices = 0,
		Elements = 1
	};

	enum Mode
	{
		Points = GL_POINTS,
		LineStrip = GL_LINE_STRIP,
		LineLoop = GL_LINE_LOOP,
		Lines = GL_LINES,
		TriangleStrip = GL_TRIANGLE_STRIP,
		TriangleFan = GL_TRIANGLE_FAN,
		Triangles = GL_TRIANGLES
	};

	enum Usage
	{
		StreamDraw = GL_STREAM_DRAW,
		StaticDraw = GL_STATIC_DRAW,
		DynamicDraw = GL_DYNAMIC_DRAW
	};

	~VBO();

	Mode mode() const;
	void mode(Mode mode);
	size_t size() const;
	void set(VertexT *vertices, size_t offset, size_t count);
	void set(uint16_t *indices, size_t offset, size_t count);

private:
	VBO(Graphics *graphics);
	void create(Mode mode, Usage vboUsage, Usage iboUsage, size_t vboSize, size_t iboSize);
	GLuint id(Type type) const;
	vbo_t handle();

	Graphics *graphics_;
	Mode mode_;
	GLuint id_[2];
	size_t size_[2];

	friend class Graphics;
};
