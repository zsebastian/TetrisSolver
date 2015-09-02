#pragma once

#include "Window.h"
#include <array>
#include "Piece.h"
#include <random>
#include <functional>
#include "PlayField.h"
#include <deque>

#define BOARD_HEIGHT 20
#define BOARD_WIDTH 10

class Board
{
public:
	enum Action { Rotate, Left, Right };

	Board(int x, int y, int tile_size);
	void render(Window& window);

	bool perform_action(Action action);

	int tick();

	bool test_collision(const Piece& piece) const;
	
	const Piece& get_current_piece() const;
	const Piece& get_next_piece(int index);
	int get_piece_count() const;

	PlayField create_play_field() const;

	int get_width() const { return BOARD_WIDTH; }
	int get_height() const { return BOARD_HEIGHT; }
private:
	struct  Tile
	{
		Tile()
			:color(Color::black()), occupied(false)
		{}

		Tile(Color color_, bool occupied_)
			:color(color_), occupied(occupied_)
		{}
		Color color;
		bool occupied;
	};

	void render_board(Window& window);
	void render_tiles();
	void render_live_piece();
	void set_color(Color color, int x, int y);
	void clear_colors();
	bool imprint_live_piece();
	int clear_rows();

	Piece random_piece();
	int next_random(int min, int max);

	int x_, y_, tile_size_;
	std::array<Color, BOARD_HEIGHT * BOARD_WIDTH> colors_;
	std::array<Tile, BOARD_HEIGHT * BOARD_WIDTH> tiles_;

	std::mt19937 random_engine_;
	std::vector<std::function<Piece(int, int, int)>> piece_makers;
	
	Piece current_piece_;
	std::deque<Piece> next_piece_queue_;
	int piece_count_;
};