#include "Font.h"
#include "Texture.h"
#include "Context.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H
#include FT_STROKER_H

#include <iostream>
#include <assert.h>

namespace gfx {

static const int hres = 100;
static const int depth = 3;
static const gfx::TexFilter tex_filter = gfx::Linear;

// -----------------------------------------------------------------------------
// Font
// -----------------------------------------------------------------------------

Font::Font(const char *filename, int atlasSize)
	:	face_(0),
		glyphTable_(0),
		outlineTable_(0),
		size_(0),
		curSize_(0),
		thickness_(0.0f),
		curThickness_(0.0f),
		atlasSize_(atlasSize)
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

	uint8_t weights[] = {0x10, 0x40, 0x70, 0x40, 0x10};

	FT_Library_SetLcdFilter(context->freetype, FT_LCD_FILTER_LIGHT);
	FT_Library_SetLcdFilterWeights(context->freetype, weights);

	face_ = face;
}

Font::~Font()
{
	if (face_ != 0)
		FT_Done_Face((FT_Face)face_);

	for (SizeTable::iterator i = glyphs_.begin(); i != glyphs_.end(); ++i)
	{
		if (i->second != 0)
			delete i->second;
	}

	for (size_t i = 0; i < atlases_.size(); i++)
		delete atlases_[i];
}

void Font::size(uint32_t size)
{
	size_ = size;

	if (glyphTable_ == 0)
		updateGlyphTable();
}

void Font::outlineWidth(float width)
{
	thickness_ = (uint32_t)(width * 64);
}

float Font::kerning(uint32_t a, uint32_t b)
{
	assert(face_ != 0);

	updateGlyphTable();

	FT_Face face = (FT_Face)face_;

	if (a == 0 || b == 0 || !FT_HAS_KERNING(face))
		return 0.0f;

	FT_UInt index1 = FT_Get_Char_Index(face, a);
	FT_UInt index2 = FT_Get_Char_Index(face, b);

	FT_Vector kerning;
	FT_Get_Kerning(face, index1, index2, FT_KERNING_UNFITTED, &kerning);

	if (kerning.x != 0)
		return kerning.x / (hres * 64.0f);

	return 0.0f;
}

int Font::linespacing()
{
	if (face_ != 0)
	{
		updateGlyphTable();
		return FT_Face(face_)->size->metrics.height >> 6;
	}

	return 0;
}

Texture *Font::texture(int atlas)
{
	assert(atlas >= 0);

	if (atlas < (int)atlases_.size())
		return atlases_[atlas]->texture();

	return 0;
}

const Font::Glyph *Font::glyph(uint32_t codepoint)
{
	assert(glyphTable_ != 0);

	updateGlyphTable();

	GlyphTable::iterator i = glyphTable_->find(codepoint);

	if (i == glyphTable_->end())
		i = glyphTable_->insert(std::make_pair(codepoint, load(codepoint))).first;

	return &i->second;
}

void Font::updateGlyphTable()
{
	if (size_ != curSize_)
	{
		FT_Matrix matrix = {
			(int)((1.0 / hres) * 0x10000L),
			(int)((0.0)        * 0x10000L),
			(int)((0.0)        * 0x10000L),
			(int)((1.0)        * 0x10000L)
		};

		FT_Set_Char_Size((FT_Face)face_, size_ * 64, 0, 96 * hres, 96);
		FT_Set_Transform((FT_Face)face_, &matrix, 0);

		// update outline and glyph tables

		OutlineTable *&table = glyphs_[size_];

		if (table == 0)
		{
			table = new OutlineTable();
			table->push_back(std::make_pair((uint32_t)0, GlyphTable()));

			outlineTable_ = table;
			glyphTable_ = &table->front().second;

			curThickness_ = 0;

			std::cout << glyphTable_ << std::endl;
		}
		else
		{
			outlineTable_ = table;
			glyphTable_ = &table->front().second;
			curThickness_ = 0;
		}

		curSize_ = size_;
	}

	if (thickness_ != curThickness_)
	{
		OutlineTable::iterator i = outlineTable_->begin();

		while (i != outlineTable_->end())
		{
			if (i->first == thickness_)
			{
				glyphTable_ = &i->second;
				curThickness_ = thickness_;
				return;
			}

			++i;
		}

		if (i == outlineTable_->end())
		{
			outlineTable_->push_back(std::make_pair(thickness_, GlyphTable()));
			glyphTable_ = &outlineTable_->back().second;
			curThickness_ = thickness_;
		}
	}
}

Font::Glyph Font::load(uint32_t codepoint)
{
	Glyph glyph;

	FT_Face face = (FT_Face)face_;

	if (face == 0)
		return glyph;

	updateGlyphTable();

	FT_UInt iGlyph = FT_Get_Char_Index(face, codepoint);
	FT_Int32 flags = FT_LOAD_TARGET_LCD;

	if (thickness_ != 0)
		flags |= FT_LOAD_NO_BITMAP;
	else
		flags |= FT_LOAD_RENDER;

	if (FT_Load_Glyph(face, iGlyph, flags) != 0)
		return glyph;

	FT_Bitmap bitmap;
	FT_Glyph ftglyph;

	int top, left;

	if (thickness_ != 0)
	{
		FT_Stroker stroker;

		if (FT_Stroker_New(context->freetype, &stroker) != 0)
			return glyph;

		FT_Stroker_Set(stroker, thickness_, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

		if (FT_Get_Glyph(face->glyph, &ftglyph) != 0)
		{
			FT_Stroker_Done(stroker);
			return glyph;
		}

		// if (FT_Glyph_StrokeBorder(&ftglyph, stroker, 0, 1) != 0)
		if (FT_Glyph_Stroke(&ftglyph, stroker, 1) != 0)
		{
			FT_Stroker_Done(stroker);
			FT_Done_Glyph(ftglyph);
			return glyph;
		}

		if (FT_Glyph_To_Bitmap(&ftglyph, FT_RENDER_MODE_LCD, 0, 1) != 0)
		{
			FT_Stroker_Done(stroker);
			FT_Done_Glyph(ftglyph);
			return glyph;
		}

		bitmap = ((FT_BitmapGlyph)ftglyph)->bitmap;
		top    = ((FT_BitmapGlyph)ftglyph)->top;
		left   = ((FT_BitmapGlyph)ftglyph)->left;

		FT_Stroker_Done(stroker);
	}
	else
	{
		bitmap  = face->glyph->bitmap;
		top     = face->glyph->bitmap_top;
		left    = face->glyph->bitmap_left;
	}

	if (bitmap.width <= 0 || bitmap.rows <= 0)
	{
		if (thickness_ != 0)
			FT_Done_Glyph(ftglyph);

		FT_Load_Glyph(face, iGlyph, FT_LOAD_RENDER | FT_LOAD_NO_HINTING);
		glyph.advance = face->glyph->advance.x / 64.0;
		return glyph;
	}

	const int padding = 1;
	const int width  = bitmap.width / depth;
	const int height = bitmap.rows;
	const int wBounds = width  + padding; // bottom-right padding
	const int hBounds = height + padding;

	Atlas *atlas = 0;
	Atlas::Region region;

	if (atlases_.size() > 0)
	{
		atlas = atlases_.back();
		region = atlas->region(wBounds, hBounds);
	}

	if (region.width == 0)
	{
		atlas = new Atlas(atlasSize_);

		if (atlas->texture() == 0)
		{
			delete atlas;
			return glyph;
		}

		atlases_.push_back(atlas);
		region = atlas->region(wBounds, hBounds);

		if (region.width == 0)
			return glyph;
	}

	glyph.atlas = (int)atlases_.size() - 1;

	glyph.x =  left;
	glyph.y = -top;
	glyph.w =  region.width; // includes right padding
	glyph.h =  region.height - padding;

	glyph.u0 = region.x;
	glyph.v0 = region.y;
	glyph.u1 = glyph.u0 + region.width; // includes right padding
	glyph.v1 = glyph.v0 + region.height - padding;

	region.width -= padding;
	region.height -= padding;

	atlas->set(region, bitmap.buffer, bitmap.pitch);

	FT_Load_Glyph(face, iGlyph, FT_LOAD_RENDER | FT_LOAD_NO_HINTING);
	glyph.advance = face->glyph->advance.x / 64.0;

	if (thickness_ != 0)
		FT_Done_Glyph(ftglyph);

	return glyph;
}

// -----------------------------------------------------------------------------
// Font::Atlas
// -----------------------------------------------------------------------------

Font::Atlas::Atlas(int size) : texture_(0), buffer_(0)
{
	int width  = size;
	int height = size;

	texture_ = new Texture(width, height, depth, false);
	texture_->filter(tex_filter);

	if (texture_->id() == 0)
	{
		delete texture_;
		texture_ = 0;
		return;
	}

	buffer_ = new uint8_t[depth * width * height];
	memset(buffer_, 0, depth * width * height);

	texture_->update(0, 0, width, height, buffer_);

	nodes_.push_back(Node(1, 1, width - 1)); // 1px top-left padding
}

Font::Atlas::~Atlas()
{
	if (texture_ != 0)
		delete texture_;

	if (buffer_ != 0)
		delete[] buffer_;
}

void Font::Atlas::set(const Region &region, const uint8_t *data, size_t srcPitch)
{
	assert(texture_ != 0);
	assert(region.x >= 0 && region.y >= 0 && region.width >= 0 && region.height >= 0);
	assert(region.x + region.width <= texture_->width());
	assert(region.y + region.height <= texture_->height());

	uint8_t *buf = new uint8_t[depth * region.width * region.height];
	uint8_t *pbuf = buf;

	if (buffer_ != 0)
	{
		size_t dstPitch = texture_->width() * depth;
		size_t rowSize = region.width * depth;

		const uint8_t *src = data;
		uint8_t *dst = &buffer_[region.y * dstPitch];

		for (int y = 0; y < region.height; y++)
		{
			memcpy(&dst[region.x * depth], src, rowSize);
			memcpy(pbuf, src, rowSize);

			src += srcPitch;
			dst += dstPitch;
			pbuf += rowSize;
		}
	}

	texture_->update(region.x, region.y, region.width, region.height, buf);

	delete[] buf;
}

Texture *Font::Atlas::texture()
{
	return texture_;
}

Font::Atlas::Region Font::Atlas::region(int width, int height)
{
	Region reg(0, 0, width, height);

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

				reg.x = nodes_[i].x;
				reg.y = y;
			}
		}
	}

	if (bestIndex == -1)
	{
		if (enlarge())
			return region(width, height);
		else
			return Region();
	}

	nodes_.insert(nodes_.begin() + bestIndex, Node(reg.x, reg.y + height, width));

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

	return reg;
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

bool Font::Atlas::enlarge()
{
	int width = texture_->width();
	int height = texture_->height();

	int newWidth = width;
	int newHeight = height;

	if (width > height)
		newHeight *= 2;
	else
		newWidth *= 2;

	if (newWidth > context->maxTextureSize || newHeight > context->maxTextureSize)
		return false;

	Texture *tx = new Texture(newWidth, newHeight, depth, false);
	tx->filter(tex_filter);

	if (tx->id() == 0)
	{
		delete tx;
		return false;
	}

	delete texture_;
	texture_ = tx;

	uint8_t *buffer = new uint8_t[depth * newWidth * newHeight];

	if (width > height)
	{
		size_t size = depth * width * height;

		memcpy(buffer, buffer_, size);
		memset(&buffer[size], 0, size);
	}
	else
	{
		uint8_t *src = buffer_;
		uint8_t *dst = buffer;

		size_t srcRowSize = depth * width;
		size_t dstRowSize = depth * newWidth;

		for (int y = 0; y < height; y++)
		{
			memcpy(dst, src, srcRowSize);
			memset(&dst[srcRowSize], 0, srcRowSize);

			src += srcRowSize;
			dst += dstRowSize;
		}

		// keep top padding
		nodes_.push_back(Node(width, 1, width));
	}

	delete[] buffer_;
	buffer_ = buffer;

	texture_->update(0, 0, newWidth, newHeight, buffer_);

	if (newWidth > newHeight)
		newHeight *= 2;
	else
		newWidth *= 2;

	if (newWidth > context->maxTextureSize || newHeight > context->maxTextureSize)
		delete[] buffer_, buffer_ = 0;

	return true;
}

} // gfx
