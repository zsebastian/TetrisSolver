#include "Piece.h"

/*
 For reference see: http://www.colinfahey.com/tetris/tetris_diagram_pieces_orientations_new.jpg
 */
Piece::Piece(std::array<int, PIECE_SIZE * PIECE_SIZE> tiles, Color color, int rotation, int x, int y, int max_rotations, bool sz_exception, bool reverse_rotate)
	: tiles_(tiles)
	, color_(color)
	, rotation_(rotation % max_rotations)
	, max_rotations_(max_rotations)
	, x_(x)
	, y_(y)
	, sz_exception_(sz_exception)
	, reverse_rotate_(reverse_rotate)
{}

void Piece::RotateLeft()
{
	if (reverse_rotate_)
	{
		rotation_++;
	}
	else
	{
		rotation_--;
	}

	if (rotation_ < 0)
	{
		rotation_ = max_rotations_ - 1;
	}
	rotation_ = rotation_ % max_rotations_;
}

void Piece::RotateRight()
{
	if (reverse_rotate_)
	{
		rotation_--;
	}
	else
	{
		rotation_++;
	}
	if (rotation_ < 0)
	{
		rotation_ = max_rotations_ - 1;
	}
	rotation_ = rotation_ % max_rotations_;
}

void Piece::Move(int dx, int dy)
{
	x_ += dx;
	y_ += dy;
}

std::array<int, PIECE_SIZE * PIECE_SIZE> Piece::GetTiles() const
{
	std::array<int, PIECE_SIZE * PIECE_SIZE> ret;
	int grid_x = 0;
	int grid_y = 0;
	int x_mod = 0;

	switch (rotation_)
	{
	case 0:
		for (int x = 0; x < PIECE_SIZE; ++x)
		{
			for (int y = 0; y < PIECE_SIZE; ++y)
			{
				ret[grid_y * PIECE_SIZE + grid_x] = tiles_[y * PIECE_SIZE + x];
				grid_y++;
			}
			grid_y = 0;
			grid_x++;
		}
		break;
	case 1:
		if (sz_exception_)
		{
			x_mod = 1;
			grid_x = x_mod;
			for (int x = 0; x < PIECE_SIZE; ++x)
			{
				for (int y = 0; y < PIECE_SIZE; ++y)
				{
					ret[x * PIECE_SIZE + (y)] = 0;
				}
			}
		}

		for (int x = x_mod; x < PIECE_SIZE; ++x)
		{
			for (int y = PIECE_SIZE - 1; y >= 0; --y)
			{
				ret[grid_y * PIECE_SIZE + (grid_x)] = tiles_[y * PIECE_SIZE + (x - x_mod)];
				grid_x++;
			}
			grid_x = x_mod;
			grid_y++;
		}
		break;
	case 2:
		for (int x = PIECE_SIZE - 1; x >= 0; --x)
		{
			for (int y = PIECE_SIZE - 1; y >= 0; --y)
			{
				ret[grid_y * PIECE_SIZE + grid_x] = tiles_[y * PIECE_SIZE + x];
				grid_y++;
			}
			grid_y = 0;
			grid_x++;
		}
		break;
	case 3:
		for (int x = PIECE_SIZE - 1; x >= 0; --x)
		{
			for (int y = 0; y < PIECE_SIZE; ++y)
			{	
				ret[grid_y * PIECE_SIZE + grid_x] = tiles_[y * PIECE_SIZE + x];
				grid_x++;
			}
			grid_x = 0;
			grid_y++;
		}
		break;
	}
	return ret;
}

Color Piece::GetColor() const
{
	return color_;
}

int Piece::GetX() const
{
	return x_;
}

int Piece::GetY() const
{
	return y_;
}

Piece Piece::MakeO(int x, int y, int rotation)
{
	return Piece(
	{ {
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 1, 1, 0, 0,
		0, 1, 1, 0, 0,
		0, 0, 0, 0, 0
		} },
	Color::make_from_bytes(255, 255, 0),
	rotation, x, y, 1, false, false);
}

Piece Piece::MakeI(int x, int y, int rotation)
{
	return Piece(
	{ {
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 1, 1, 1, 1,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0
		} },
		Color::make_from_bytes(0, 255, 255),
		rotation, x, y, 2, false, false);
}

Piece Piece::MakeS(int x, int y, int rotation)
{ 
	return Piece(
	{ {
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 1, 1, 0,
		0, 1, 1, 0, 0,
		0, 0, 0, 0, 0
		} },
		Color::make_from_bytes(191, 255, 0),
		rotation, x, y, 2, true, false);
}

Piece Piece::MakeZ(int x, int y, int rotation)
{
	return Piece(
	{ {
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 1, 1, 0, 0,
		0, 0, 1, 1, 0,
		0, 0, 0, 0, 0
		} },
		Color::make_from_bytes(255, 0, 0),
		rotation, x, y, 2, true, false);
}

Piece Piece::MakeL(int x, int y, int rotation)
{
	return Piece(
	{ {
		0, 0, 0, 0, 0,
		0, 0, 0, 1, 0,
		0, 1, 1, 1, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0
		} },
		Color::make_from_bytes(255, 127, 0),
		rotation, x, y, 4, false, true);
}

Piece Piece::MakeJ(int x, int y, int rotation)
{
	return Piece(
	{ {
		0, 0, 0, 0, 0,
		0, 1, 0, 0, 0,
		0, 1, 1, 1, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0
		} },
		Color::make_from_bytes(0, 0, 255),
		rotation, x, y, 4, false, false);
}

Piece Piece::MakeT(int x, int y, int rotation)
{
	return Piece(
	{ {
		0, 0, 0, 0, 0,
		0, 0, 1, 0, 0,
		0, 1, 1, 1, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0
		} },
		Color::make_from_bytes(255, 0, 255),
		rotation, x, y, 4, false, false);
}