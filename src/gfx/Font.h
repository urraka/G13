#pragma once

#include <map>
#include <vector>
#include <stdint.h>
#include <stddef.h>

namespace gfx {

class Texture;

class Font
{
public:
	struct Glyph
	{
		Glyph()
			:	advance(0),
				atlas(0),
				x(0), y(0), w(0), h(0),
				u0(0), v0(0), u1(0), v1(0) {}

		int advance;        // horizontal offset to next character
		int atlas;          // page->atlas[ atlas ]
		int x, y, w, h;     // glyph bounds relative to baseline
		int u0, v0, u1, v1; // non-normalized texcoords (texture size can change)
	};

	class Atlas
	{
	public:
		Atlas();
		~Atlas();

		struct Region
		{
			Region() : x(0), y(0), width(0), height(0) {}
			Region(int X, int Y, int W, int H) : x(X), y(Y), width(W), height(H) {}

			int x;
			int y;
			int width;
			int height;
		};

		struct Node
		{
			Node() : x(0), y(0), width(0) {}
			Node(int X, int Y, int W) : x(X), y(Y), width(W) {}

			int x;
			int y;
			int width;
		};

		Region region(int width, int height);
		void set(const Region &region, uint8_t *data);
		Texture *texture();

	private:
		std::vector<Node> nodes_;
		Texture *texture_;
		uint8_t *buffer_;

		bool fits(int index, int width, int height, int *y);
		void merge();
		bool enlarge();
	};

	Font(const char *filename);
	~Font();

	void size(uint32_t fontsize);
	Texture *texture(int atlas);
	const Glyph *glyph(uint32_t codepoint, bool bold);

private:
	typedef std::map<uint32_t, Glyph>       GlyphTable;
	typedef std::map<uint32_t, GlyphTable*> PageTable;

	void *face_;
	PageTable glyphs_;
	std::vector<Atlas*> atlases_;

	GlyphTable *currentTable_;
	uint32_t    currentSize_;

	Glyph load(uint32_t codepoint, bool bold);
};

} // gfx
