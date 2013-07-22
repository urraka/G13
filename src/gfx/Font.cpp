#include "Font.h"
#include "Texture.h"
#include "Context.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BITMAP_H

#include <iostream>

namespace gfx {

// -----------------------------------------------------------------------------
// Font
// -----------------------------------------------------------------------------

Font::Font(const char *filename)
	:	face_(0),
		currentTable_(0),
		currentSize_(0)
{
	FT_Face face;

	if (FT_New_Face(context->freetype, filename, 0, &face) != 0)
	{
		std::cerr << "Failed to load font: " << filename << std::endl;
		return;
	}

	if (FT_Select_Charmap(face, FT_ENCODING_UNICODE) != 0)
	{
		std::cerr << "Font doesn't support unicode: " << filename << std::endl;
		return;
	}

	face_ = face;
}

Font::~Font()
{
	if (face_ != 0)
		FT_Done_Face((FT_Face)face_);

	for (PageTable::iterator i = glyphs_.begin(); i != glyphs_.end(); ++i)
		delete i->second;

	for (size_t i = 0; i < atlases_.size(); i++)
		delete atlases_[i];
}

void Font::size(uint32_t size)
{
	assert(size > 0);

	GlyphTable *&table = glyphs_[size]; // will insert the first time

	if (table == 0)
		table = new GlyphTable();

	currentSize_ = size;
	currentTable_ = table;
}

Texture *Font::texture(const Glyph *glyph)
{
	assert(glyph->atlas >= 0 && glyph->atlas < (int)atlases_.size());

	return atlases_[glyph->atlas]->texture();
}

const Font::Glyph *Font::glyph(uint32_t codepoint, bool bold)
{
	assert(currentTable_ != 0);

	uint32_t key = codepoint | (bold ? (1U << 31) : 0);

	GlyphTable::iterator i = currentTable_->find(key);

	if (i == currentTable_->end())
		i = currentTable_->insert(std::make_pair(key, load(codepoint, bold))).first;

	return &i->second;
}

Font::Glyph Font::load(uint32_t codepoint, bool bold)
{
	struct GlyphAutoRelease
	{
		FT_Glyph *glyph;
		GlyphAutoRelease(FT_Glyph *g) : glyph(g) {}
		~GlyphAutoRelease() { FT_Done_Glyph(*glyph); }
	};

	Glyph glyph;

	FT_Face face = (FT_Face)face_;

	if (face == 0)
		return Glyph();

	if (currentSize_ != face->size->metrics.x_ppem)
	{
		if (FT_Set_Pixel_Sizes(face, 0, currentSize_) != 0)
			return Glyph();
	}

	if (FT_Load_Char(face, codepoint, FT_LOAD_TARGET_NORMAL) != 0)
		return Glyph();

	FT_Glyph desc;

	if (FT_Get_Glyph(face->glyph, &desc) != 0)
		return Glyph();

	GlyphAutoRelease glyphAutoRelease(&desc);

	FT_Pos weight = 1 << 6;
	bool outline = (desc->format == FT_GLYPH_FORMAT_OUTLINE);

	if (bold && outline)
	{
		FT_OutlineGlyph outlineGlyph = (FT_OutlineGlyph)desc;
		FT_Outline_Embolden(&outlineGlyph->outline, weight);
	}

	FT_Glyph_To_Bitmap(&desc, FT_RENDER_MODE_NORMAL, 0, 1);
	FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)desc;
	FT_Bitmap& bitmap = bitmapGlyph->bitmap;

	if (bold && !outline)
		FT_Bitmap_Embolden(context->freetype, &bitmap, weight, weight);

	if (bitmap.width <= 0 || bitmap.rows <= 0)
		return Glyph();

	const int padding = 1;
	const int width  = bitmap.width;
	const int height = bitmap.rows;
	const int wBounds = width  + 2 * padding;
	const int hBounds = height + 2 * padding;

	Atlas *atlas = 0;
	Atlas::Region region;

	if (atlases_.size() > 0)
	{
		atlas = atlases_.back();
		region = atlas->region(wBounds, hBounds);
	}

	// TODO: resize atlas, create new if unable to do it

	if (region.isnull())
	{
		int w = 512;
		int h = 512;

		while (w < wBounds) w *= 2;
		while (h < hBounds) h *= 2;

		// TODO: check max texture width/height

		atlas = new Atlas(w, h);

		if (atlas->texture() == 0)
		{
			delete atlas;
			return Glyph();
		}

		atlases_.push_back(atlas);
		region = atlas->region(wBounds, hBounds);

		if (region.isnull())
			return Glyph();
	}

	glyph.atlas = (int)atlases_.size() - 1;

	glyph.x =  bitmapGlyph->left - padding;
	glyph.y = -bitmapGlyph->top  - padding;
	glyph.w =  wBounds;
	glyph.h =  hBounds;

	glyph.tx = region.x;
	glyph.ty = region.y;
	glyph.tw = region.width;
	glyph.th = region.height;

	glyph.advance = (desc->advance.x >> 16) + bold ? (weight >> 6) : 0;

	uint8_t *data = new uint8_t[width * height];

	const uint8_t *src = bitmap.buffer;
	uint8_t *dst = data;

	if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
				dst[x] = ((src[x / 8]) & (1 << (7 - (x % 8)))) ? 255 : 0;

			src += bitmap.pitch;
			dst += width;
		}
	}
	else
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
				dst[x] = src[x];

			src += bitmap.pitch;
			dst += width;
		}
	}

	region.x += padding;
	region.y += padding;
	region.width -= padding * 2;
	region.height -= padding * 2;

	atlas->set(region, data);

	delete[] data;

	return glyph;
}

// -----------------------------------------------------------------------------
// Font::Atlas
// -----------------------------------------------------------------------------

Font::Atlas::Atlas(int width, int height) : texture_(0)
{
	texture_ = new Texture(width, height, 1, false);

	if (texture_->id() == 0)
	{
		delete texture_;
		texture_ = 0;
		return;
	}

	uint8_t *data = new uint8_t[width * height];
	memset(data, 0, width * height);

	texture_->update(0, 0, width, height, data);

	delete[] data;

	nodes_.push_back(Node(0, 0, width));
}

Font::Atlas::~Atlas()
{
	if (texture_ != 0)
		delete texture_;
}

void Font::Atlas::set(const Region &region, uint8_t *data)
{
	assert(texture_ != 0);

	texture_->update(region.x, region.y, region.width, region.height, data);
}

Texture *Font::Atlas::texture()
{
	return texture_;
}

Font::Atlas::Region Font::Atlas::region(int width, int height)
{
	Region region(0, 0, width, height);

	int y;
	int bestIndex = -1;
	int bestWidth = INT_MAX;
	int bestHeight = INT_MAX;

	for (size_t i = 0; i < nodes_.size(); i++)
	{
		if (fits(i, width, height, &y))
		{
			if (y + height < bestHeight || (y + height == bestHeight && nodes_[i].width < bestWidth))
			{
				bestIndex = i;
				bestWidth = nodes_[i].width;
				bestHeight = y + height;

				region.x = nodes_[i].x;
				region.y = y;
			}
		}
	}

	if (bestIndex == -1)
		return Region();

	nodes_.insert(nodes_.begin() + bestIndex, Node(region.x, region.y + height, width));

	for (size_t i = bestIndex + 1; i < nodes_.size(); i++)
	{
		Node &node = nodes_[i];
		Node &prev = nodes_[i - 1];

		if (node.x >= prev.x + prev.width)
			break;

		int shrink = prev.x + prev.width - node.x;

		node.x += shrink;
		node.width -= shrink;

		if (node.width > 0)
			break;

		nodes_.erase(nodes_.begin() + i--);
	}

	merge();

	return region;
}

bool Font::Atlas::fits(int index, int width, int height, int *y)
{
	int x = nodes_[index].x;

	if (x + width > texture_->width())
		return false;

	*y = nodes_[index].y;

	int i = index;
	int widthLeft = width;

	while (widthLeft > 0)
	{
		if (nodes_[i].y > *y)
			*y = nodes_[i].y;

		if (*y + height > texture_->height())
			return false;

		widthLeft -= nodes_[i++].width;
	}

	return true;
}

void Font::Atlas::merge()
{
	for (size_t i = 0; i < nodes_.size() - 1; i++)
	{
		if (nodes_[i].y == nodes_[i + 1].y)
		{
			nodes_[i].width += nodes_[i + 1].width;
			nodes_.erase(nodes_.begin() + (i-- + 1));
		}
	}
}

} // gfx
