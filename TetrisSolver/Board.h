#pragma once

#include "Window.h"
#include <array>
#include "Piece.h"
#include <random>
#include <functional>


#define BOARD_HEIGHT 20
#define BOARD_WIDTH 10

class Board
{
public:
	enum Action { Rotate, Left, Right };

	Board(int x, int y, int tile_size);
	void Render(Window& window);

	bool PerformAction(Action action);
	int Tick();
	
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

	void RenderBoard(Window& window);
	void RenderTiles();
	void RenderLivePiece();
	void SetColor(Color color, int x, int y);
	void ClearColors();
	bool ImprintLivePiece();
	int ClearRows();

	bool TestCollision() const;

	Piece RandomPiece();
	int NextRandom(int min, int max);

	int x_, y_, tile_size_;
	std::array<Color, BOARD_HEIGHT * BOARD_WIDTH> colors_;
	std::array<Tile, BOARD_HEIGHT * BOARD_WIDTH> tiles_;

	std::mt19937 random_engine_;
	std::vector<std::function<Piece(int, int, int)>> piece_makers;

	Piece current_piece_;
	Piece next_piece_;
};