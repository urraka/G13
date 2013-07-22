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
	// -----------------------------------------------------------------------------
	// Glyph
	// -----------------------------------------------------------------------------

	struct Glyph
	{
		int advance;        // horizontal offset to next character
		int atlas;          // page->atlas[ atlas ]
		int x, y, w, h;     // glyph bounds relative to baseline
		int tx, ty, tw, th; // non-normalized texcoords (texture size can change)
	};

	// -----------------------------------------------------------------------------
	// Atlas
	// -----------------------------------------------------------------------------

	class Atlas
	{
	public:
		Atlas(int width, int height);
		~Atlas();

		struct Region
		{
			Region() : x(0), y(0), width(0), height(0) {}
			Region(int X, int Y, int W, int H) : x(X), y(Y), width(W), height(H) {}

			bool isnull() { return width == 0 || height == 0; }

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

		bool fits(int index, int width, int height, int *y);
		void merge();
	};

	// -----------------------------------------------------------------------------
	// Page
	// -----------------------------------------------------------------------------

	class Page
	{
	public:
		Page(void *face, uint32_t fontsize);
		~Page();

		const Glyph *glyph(uint32_t codepoint, bool bold);
		Texture *texture(int atlas);

	private:
		typedef std::map<uint32_t, Glyph> GlyphTable;
		typedef std::vector<Atlas*>       AtlasArray;

		void      *face_;
		uint32_t   fontsize_;
		GlyphTable glyphs_;
		AtlasArray atlases_;

		Glyph load(uint32_t codepoint, bool bold);
	};

	// -----------------------------------------------------------------------------
	// Font
	// -----------------------------------------------------------------------------

	Font(const char *filename);
	~Font();

	Page *page(uint32_t size);

private:
	typedef std::map<uint32_t, Page*> PageTable;

	void *face_;
	PageTable pages_;
};

} // gfx
