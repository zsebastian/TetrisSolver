#include "Piece.h"

std::array<Piece::Settings, 7U> Piece::settings_;

/*
 For reference see: http://www.colinfahey.com/tetris/tetris_diagram_pieces_orientations_new.jpg
 */
Piece::Piece( int rotation, int x, int y, int type)
	: rotation_(rotation % settings_[type].max_rotations)
	, x_(x)
	, y_(y)
	, type_(type)
{

}

void Piece::rotate_left()
{
	if (settings_[type_].reverse_rotate)
	{
		rotation_++;
	}
	else
	{
		rotation_--;
	}

	if (rotation_ < 0)
	{
		rotation_ = settings_[type_].max_rotations - 1;
	}
	rotation_ = rotation_ % settings_[type_].max_rotations;
}

void Piece::rotate_right()
{
	if (settings_[type_].reverse_rotate)
	{
		rotation_--;
	}
	else
	{
		rotation_++;
	}
	if (rotation_ < 0)
	{
		rotation_ = settings_[type_].max_rotations - 1;
	}
	rotation_ = rotation_ % settings_[type_].max_rotations;
}

void Piece::move(int dx, int dy)
{
	x_ += dx;
	y_ += dy;
}

std::array<int, PIECE_SIZE * PIECE_SIZE> Piece::get_tiles() const
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
				ret[grid_y * PIECE_SIZE + grid_x] = settings_[type_].tiles[y * PIECE_SIZE + x] ? 1 : 0;
				grid_y++;
			}
			grid_y = 0;
			grid_x++;
		}
		break;
	case 1:
		if (settings_[type_].sz_exception)
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
				ret[grid_y * PIECE_SIZE + (grid_x)] = settings_[type_].tiles[y * PIECE_SIZE + (x - x_mod)] ? 1 : 0;
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
				ret[grid_y * PIECE_SIZE + grid_x] = settings_[type_].tiles[y * PIECE_SIZE + x] ? 1 : 0;
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
				ret[grid_y * PIECE_SIZE + grid_x] = settings_[type_].tiles[y * PIECE_SIZE + x] ? 1 : 0;
				grid_x++;
			}
			grid_x = 0;
			grid_y++;
		}
		break;
	}
	return ret;
}

Color Piece::get_color() const
{
	return settings_[type_].color;
}

int Piece::get_x() const
{
	return x_;
}

int Piece::get_y() const
{
	return y_;
}

Piece Piece::make_O(int x, int y, int rotation)
{
	setup_O();
	return Piece(rotation, x, y, 0);
}

void Piece::setup_O()
{
	if (!settings_[0].setup)
	{
		settings_[0] = Settings({ {
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0,
			0, 1, 1, 0, 0,
			0, 1, 1, 0, 0,
			0, 0, 0, 0, 0
			} },
			Color::make_from_bytes(255, 255, 0),
			1, false, false);
	}
}

Piece Piece::make_I(int x, int y, int rotation)
{
	setup_I();
	return Piece(rotation, x, y, 1);
}

void Piece::setup_I()
{
	if (!settings_[1].setup)
	{
		settings_[1] = Settings({ {
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 1, 1, 1, 1,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0
			} },
			Color::make_from_bytes(0, 255, 255),
			2, false, false);
	}
}

Piece Piece::make_S(int x, int y, int rotation)
{ 
	setup_S();
	return Piece(rotation, x, y, 2);
}

void Piece::setup_S()
{
	if (!settings_[2].setup)
	{
		settings_[2] = Settings({ {
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0,
			0, 0, 1, 1, 0,
			0, 1, 1, 0, 0,
			0, 0, 0, 0, 0
			} },
			Color::make_from_bytes(191, 255, 0),
			2, true, false);
	}
}

Piece Piece::make_Z(int x, int y, int rotation)
{
	setup_Z();
	return Piece(rotation, x, y, 3);
}

void Piece::setup_Z()
{
	if (!settings_[3].setup)
	{
		settings_[3] = Settings({ {
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0,
			0, 1, 1, 0, 0,
			0, 0, 1, 1, 0,
			0, 0, 0, 0, 0
				} },
				Color::make_from_bytes(255, 0, 0),
				2, true, false);
	}
}

Piece Piece::make_L(int x, int y, int rotation)
{
	setup_L();
	return Piece(rotation, x, y, 4);
}

void Piece::setup_L()
{
	if (!settings_[4].setup)
	{
		settings_[4] = Settings({ {
			0, 0, 0, 0, 0,
			0, 0, 0, 1, 0,
			0, 1, 1, 1, 0,
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0
				} },
				Color::make_from_bytes(255, 127, 0),
				4, false, true);
	}
}

Piece Piece::make_J(int x, int y, int rotation)
{
	setup_J();
	return Piece(rotation, x, y, 5);
}

void Piece::setup_J()
{
	if (!settings_[5].setup)
	{
		settings_[5] = Settings({ {
			0, 0, 0, 0, 0,
			0, 1, 0, 0, 0,
			0, 1, 1, 1, 0,
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0
				} },
				Color::make_from_bytes(0, 0, 255),
				4, false, false);
	}
}

Piece Piece::make_T(int x, int y, int rotation)
{
	setup_T();
	return Piece(rotation, x, y, 6);
}

void Piece::setup_T()
{
	if (!settings_[6].setup)
	{
		settings_[6] = Settings({ {
			0, 0, 0, 0, 0,
			0, 0, 1, 0, 0,
			0, 1, 1, 1, 0,
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0
				} },
				Color::make_from_bytes(255, 0, 255),
				4, false, false);
	}
}

int Piece::get_rotation() const
{
	return rotation_;
}

int Piece::get_max_rotations() const
{
	return settings_[type_].max_rotations;
}

void Piece::set(int x, int y, int rotation)
{
	x_ = x;
	y_ = y;
	rotation_ = rotation;
}