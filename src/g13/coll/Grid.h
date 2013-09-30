#pragma once

namespace g13 {
namespace coll {

template<typename T> class Grid
{
public:
	struct Cell
	{
		std::vector<T> items;
		std::vector<int> shared;
	};

	struct Location
	{
		Location() : cell(0), index(-1) {}
		const Cell *cell;
		int index;
	};

	Grid(int cols, int rows, const fixrect &bounds)
		:	cols_(cols),
			rows_(rows),
			cellWidth_(bounds.width() / cols),
			cellHeight_(bounds.height() / rows),
			bounds_(bounds),
			cells_(cols * rows)
	{
	}

	Location push(const T &item)
	{
		Location location;

		fixrect bounds = Grid<T>::bounds(item);

		const fixed &w = cellWidth_;
		const fixed &h = cellHeight_;

		int x0 = std::max(    0, fpm::floor(bounds.tl.x / w).to_int());
		int y0 = std::max(    0, fpm::floor(bounds.tl.y / h).to_int());
		int x1 = std::min(cols_, fpm::ceil (bounds.br.x / w).to_int());
		int y1 = std::min(rows_, fpm::ceil (bounds.br.y / h).to_int());

		int N = (x1 - x0) * (y1 - y0);

		if (N > 1)
		{
			bounds.tl.x = 0;
			bounds.tl.y = 0;
			bounds.br.x = w;
			bounds.br.y = h;

			location.index = items_.size();
			items_.push_back(item);

			for (int x = x0; x < x1; x++)
			{
				for (int y = y0; y < y1; y++)
				{
					if (intersects(item, bounds + fixvec2(w * x, h * y)))
						cell(x, y).shared.push_back(location.index);
				}
			}
		}
		else if (N == 1)
		{
			Cell &cell = this->cell(x0, y0);

			location.cell  = &cell;
			location.index = cell.items.size();

			cell.items.push_back(item);
		}

		return location;
	}

	void clear()
	{
		items_.clear();

		for (int i = 0; i < (int)cells_.size(); i++)
			cells_[i].items.clear();
	}

	Cell &cell(int x, int y)      { return cells_[y * cols_ + x]; }
	std::vector<Cell> &cells()    { return cells_; }
	std::vector<T> &sharedItems() { return items_; }

	const Cell &cell(int x, int y)      const { return cells_[y * cols_ + x]; }
	const std::vector<Cell> &cells()    const { return cells_; }
	const std::vector<T> &sharedItems() const { return items_; }

	int cols() const { return cols_; }
	int rows() const { return rows_; }

	const fixed &cellWidth()  const { return cellWidth_;  }
	const fixed &cellHeight() const { return cellHeight_; }

	static inline fixrect bounds(const T &item) { return T::MUST_SPECIALIZE; }
	static inline bool intersects(const T &item, const fixrect &bounds) { return T::MUST_SPECIALIZE; }

private:
	int     cols_;
	int     rows_;
	fixed   cellWidth_;
	fixed   cellHeight_;
	fixrect bounds_;

	std::vector<T>    items_;
	std::vector<Cell> cells_;
};

}} // g13::coll
