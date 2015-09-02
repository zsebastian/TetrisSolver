#include "PlayField.h"

PlayField::PlayField(int w, int h)
	:w_(w)
	,h_(h)
	,tiles_(h * w, false)
	, cleared_rows_(0)
{

}

void PlayField::set(int x, int y, bool occupied)
{
	int index = index_of(x, y);
	if (index != -1)
	{
		if (occupied == true)
		{
			occupied = true;
		}
		if (tiles_[index] != occupied)
		{
			tiles_[index].flip();
		}
	}
}

bool PlayField::get(int x, int y) const
{
	int index = index_of(x, y);
	if (index != -1)
	{
		return tiles_[index];
	}
	return false;
}

bool PlayField::test_collision(const Piece& piece) const
{
	auto tiles = piece.get_tiles();
	for (int i = 0; i < PIECE_SIZE; ++i)
	{
		for (int j = 0; j < PIECE_SIZE; ++j)
		{
			int x = piece.get_x() + i - 2;
			int y = piece.get_y() + j - 2;
			if (tiles[j * PIECE_SIZE + i] != 0)
			{
				if (y >= h_)
				{
					return true;
				}
				else if (x >= 0 && x < w_ && y >= 0 && y < h_ && tiles_[y * w_ + x])
				{
					return true;
				}
				else if (!(x >= 0 && x < w_))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool PlayField::imprint(const Piece& piece)
{
	auto tiles = piece.get_tiles();
	for (int i = 0; i < PIECE_SIZE; ++i)
	{
		for (int j = 0; j < PIECE_SIZE; ++j)
		{
			int x = piece.get_x() + i - 2;
			int y = piece.get_y() + j - 2;
			if (x >= 0 && x < w_ && y >= -1 && y < h_ && tiles[j * PIECE_SIZE + i] != 0)
			{
				if (y == -1)
				{
					return false;
				}
				tiles_[y * w_ + x] = true;
			}
		}
	}

	cleared_rows_ += clear_rows();
	return true;
}

/* Returns number of rows cleared. */
int PlayField::clear_rows()
{
	int cleared = 0;
	for (int row = 0; row < h_; ++row)
	{
		bool occ = true;
		for (int col = 0; col < w_; ++col)
		{
			if (!tiles_[row * w_ + col])
			{
				occ = false;
				break;
			}
		}
		if (occ)
		{
			++cleared;
			for (int col = 0; col < w_; ++col)
			{
				tiles_[row * w_ + col] = false;
			}
			for (int y = row; y >= 1; --y)
			{
				for (int x = 0; x < w_; ++x)
				{
					tiles_[y* w_ + x] = tiles_[(y - 1) * w_ + x];
				}
			}
		}
	}
	return cleared;
}

int PlayField::index_of(int x, int y) const
{
	return y * w_ + x;
}