#pragma once

#include <array>
#include "Color.h"

#define PIECE_SIZE 5

class Piece
{
public:
	Piece() {};
	Piece(std::array<int, PIECE_SIZE * PIECE_SIZE> tiles, Color color, int rotation, int x, int y, int max_rotations, bool sz_exception, bool reverse_rotate);
	
	static Piece MakeO(int x, int y, int rotation);
	static Piece MakeI(int x, int y, int rotation);
	static Piece MakeS(int x, int y, int rotation);
	static Piece MakeZ(int x, int y, int rotation);
	static Piece MakeL(int x, int y, int rotation);
	static Piece MakeJ(int x, int y, int rotation);
	static Piece MakeT(int x, int y, int rotation);

	void RotateLeft();
	void RotateRight();
	void Move(int dx, int dy);

	std::array<int, PIECE_SIZE * PIECE_SIZE> GetTiles() const;
	Color GetColor() const;
	int GetX() const;
	int GetY() const;
private:
	std::array<int, PIECE_SIZE * PIECE_SIZE> tiles_;
	Color color_;
	int rotation_, x_, y_, max_rotations_;
	bool sz_exception_;
	bool reverse_rotate_;
};