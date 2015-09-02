#pragma once

/*	A simplistic, less memory-intense version of Board.
	For use with the Solver
*/

#include "Piece.h"
#include <vector>

class PlayField
{
public:
	PlayField(int w, int h);
	void set(int x, int y, bool occupied);
	bool get(int x, int y) const;

	bool test_collision(const Piece& piece) const;

	/* This returns true if the piece was imprintable.
	 * If this returns false, but test_collision returns true
	 * this means the player has lost the game.
	 */
	bool imprint(const Piece& piece);

	int get_width() const { return w_; }
	int get_height() const { return h_; }
	int get_cleared_rows() const { return cleared_rows_; };

private:
	int index_of(int x, int y) const;
	/* Returns number of rows cleared. */
	int clear_rows();

	std::vector<bool> tiles_;
	int cleared_rows_;
	int w_, h_;
};