#pragma once

class Geometry
{
public:
	Geometry()
		:	vertices(0),
			indices(0),
			vcount(0),
			icount(0)
	{
	}

	Geometry(size_t nVertices, Vertex *v, size_t nIndices, uint16_t *i)
		:	vertices(v),
			indices(i),
			vcount(nVertices),
			icount(nIndices)
	{
	}

	size_t vcount;
	Vertex *vertices;

	size_t icount;
	uint16_t *indices;
};
