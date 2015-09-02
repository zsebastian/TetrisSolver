#pragma once

#include <array>
#include "Color.h"
#include <bitset>

#define PIECE_SIZE 5

class Piece
{
public:
	Piece() {};

	static Piece make_O(int x, int y, int rotation);
	static Piece make_I(int x, int y, int rotation);
	static Piece make_S(int x, int y, int rotation);
	static Piece make_Z(int x, int y, int rotation);
	static Piece make_L(int x, int y, int rotation);
	static Piece make_J(int x, int y, int rotation);
	static Piece make_T(int x, int y, int rotation);

	void rotate_left();
	void rotate_right();
	void move(int dx, int dy);

	std::array<int, PIECE_SIZE * PIECE_SIZE> get_tiles() const;
	Color get_color() const;
	int get_x() const;
	int get_y() const;
	int get_rotation() const;
	int get_max_rotations() const;

	void set(int x, int y, int rotation);
private:
	static void setup_O();
	static void setup_I();
	static void setup_S();
	static void setup_Z();
	static void setup_L();
	static void setup_J();
	static void setup_T();

	Piece(int rotation, int x, int y, int type);

	struct Settings
	{
		Settings()
			:setup(false)
		{}

		Settings(std::array<int, PIECE_SIZE * PIECE_SIZE> tiles_, Color color_, int max_rotations_, bool sz_exception_, bool reverse_rotate_)
			: color(color_)
			, max_rotations(max_rotations_)
			, sz_exception(sz_exception_)
			, reverse_rotate(reverse_rotate_)
			, setup(true)
		{
			for (int i = 0; i < PIECE_SIZE * PIECE_SIZE; ++i)
			{
				tiles[i] = tiles_[i] == 1;
			}
		}
		//These can easily be feather weighted.
		std::bitset<PIECE_SIZE * PIECE_SIZE> tiles;
		Color color;
		bool sz_exception;
		bool reverse_rotate;
		int max_rotations;
		bool setup;
	};

	static std::array<Settings, 7U> settings_;

	int rotation_, x_, y_, type_;

};