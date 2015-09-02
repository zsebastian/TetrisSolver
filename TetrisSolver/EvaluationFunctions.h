#pragma once

#include "PlayField.h"

template<int ID>
struct EvaluationFunction
{

};

template<>
struct EvaluationFunction<0>
{
	/* Total Lines Cleared
		This is the total number of lines cleared as a consequence of introducing the 2 Tetriminos.
	*/
	double operator()(const PlayField& from, const PlayField& to, const std::vector<Piece>& locked_pieces)
	{
		int lines_cleared = from.get_cleared_rows() - to.get_cleared_rows();
		if (lines_cleared == 0)
		{
			return -1.0f;
		}
		else if (lines_cleared == 1)
		{
			return 0.5f;
		}
		else if (lines_cleared == 2)
		{
			return 1.5f;
		}
		else if (lines_cleared == 3)
		{
			return 4.0f;
		}
		else if (lines_cleared >= 4)
		{
			return 8.0f;
		}
		return static_cast<double>(from.get_cleared_rows() - to.get_cleared_rows());
	};
	
	static double weight()
	{
		return -4.0f;
	}
};

template<>
struct EvaluationFunction < 1 >
{
	/* Total Lock Height
		Lock height is the height above the playfield floor where a piece locked. 
		It is the vertical distance that a locked piece would drop if all the other solid squares 
		in the playfield were removed and the orientation of the piece was maintained. Total lock 
		height is the sum of the lock heights of the 2 Tetriminos.
	*/
	double operator()(const PlayField& from, const PlayField& to, const std::vector<Piece>& locked_pieces)
	{
		int lock_height = 0;
		for (auto& p : locked_pieces)
		{
			lock_height = get_lock_height(p, to.get_height());
		}
		return lock_height;
	};

	int get_lock_height(const Piece& piece, int height)
	{
		auto tiles = piece.get_tiles();
		int lowest_y = piece.get_y() - 2;
		for (int x = 0; x < PIECE_SIZE; ++x)
		{
			for (int y = 0; y < PIECE_SIZE; ++y)
			{
				bool occ = tiles[y * PIECE_SIZE + x];
				if (occ)
				{
					lowest_y = (piece.get_y() - 2) + y;
				}
			}
		}
		return (height - lowest_y) + 1;
	}

	static double weight()
	{
		return 12.885008263218383;
	}
};


template<>
struct EvaluationFunction<2>
{
	/*
		Total Well Cells
		A well cell is an empty cell located above all the solid cells within its 
		column such that its left and right neighbors are both solid cells;
		the playfield walls are treated as solid cells in this determination.
		The idea is that a well is a structure open at the top, sealed at the bottom 
		and surrounded by walls on both sides.The possibility of intermittent gaps 
		in the well walls means that well cells do not necessarily appear in a contiguous
		stack within a column.
	*/
	double operator()(const PlayField& from, const PlayField& to, const std::vector<Piece>& locked_pieces)
	{
		int wall_cells = 0;
		for (int x = 0; x < to.get_width(); ++x)
		{
			int first_top = -1;
			for (int y = 0; y < to.get_height(); ++y)
			{
				if (to.get(x, y))
				{
					first_top = y - 1;
					break;
				}
			}
			if (first_top != -1 && first_top != to.get_height() - 1)
			{
				if (x == 0 || x == to.get_width() - 1)
				{
					wall_cells++;
				}
				else if (!to.get(x - 1, first_top) && !to.get(x + 1, first_top))
				{
					wall_cells++;
				}
			}
		}
		return static_cast<double>(wall_cells);
	};


	static double weight()
	{
		return 15.842707182438396;
	}
};


template<>
struct EvaluationFunction<3>
{
	/*
	Total Column Holes
		A column hole is an empty cell directly beneath a solid cell. 
		The playfield floor is not compared to the cell directly above it. Empty columns contain no holes.
	*/
	double operator()(const PlayField& from, const PlayField& to, const std::vector<Piece>& locked_pieces)
	{
		int holes = 0;
		for (int x = 0; x < to.get_width(); ++x)
		{
			for (int y = 1; y < to.get_height() - 1; ++y)
			{
				if (!to.get(x, y) && to.get(x, y - 1))
				{
					holes++;
				}
			}
			
		}
		return static_cast<double>(holes);
	};


	static double weight()
	{
		return 26.894496507795950;
	}
};


template<>
struct EvaluationFunction<4>
{
	/*
	Total Column Transitions
	A column transition is an empty cell adjacent to a solid cell (or vice versa) within the same column. 
	The changeover from the highest solid block in the column to the empty space above it is not considered a transition. 
	Similarly, the playfield floor is not compared to the cell directly above it. As a result, a completely empty column has no transitions.
	*/
	double operator()(const PlayField& from, const PlayField& to, const std::vector<Piece>& locked_pieces)
	{
		int transitions = 0;
		for (int x = 0; x < to.get_width(); ++x)
		{
			for (int y = 1; y < to.get_height(); ++y)
			{
				if (!to.get(x, y - 1) && to.get(x, y)) 
				{
					transitions++;
				}
			}

		}
		return static_cast<double>(transitions);
	};


	static double weight()
	{
		return 27.616914062397015;
	}
};

template<>
struct EvaluationFunction<5>
{
	/*
	Total Row Transitions
	A row transition is an empty cell adjacent to a solid cell (or vice versa) within the same row.
	Empty cells adjoining playfield walls are considered transitions. The total is computed across all rows in the playfield. 
	However, rows that are completely empty do not contribute to the sum.
	*/
	double operator()(const PlayField& from, const PlayField& to, const std::vector<Piece>& locked_pieces)
	{
		int transitions = 0;
		for (int y = 1; y < to.get_height() - 1; ++y)
		{
			int this_row = 0;
			bool empty = true;
			for (int x = 0; x < to.get_width(); ++x)
			{
				if (to.get(x, y))
				{
					if (!to.get(x - 1, y))
					{
						transitions++;
					}
					empty = false;
					break;
				}
			}
			if (!empty)
			{
				transitions += this_row;
			}

		}
		return static_cast<double>(transitions);
	};


	static double weight()
	{
		return 27.616914062397015;
	}
};

template<>
struct EvaluationFunction<6>
{
	/*
	Pile Height
		Solid blocks can be suspended over empty cells producing overhangs and holes; 
		however, it is not possible to fix solid blocks above completely empty rows. 
		Consequentially, pile height is the number of rows that contain at least one solid block
	*/
	double operator()(const PlayField& from, const PlayField& to, const std::vector<Piece>& locked_pieces)
	{
		int pile_height = 0;
		for (int y = 1; y < to.get_height() - 1; ++y)
		{
			for (int x = 0; x < to.get_width(); ++x)
			{
				if (to.get(x, y))
				{
					pile_height++;
					break;
				}
			}
		}
		return static_cast<double>(pile_height);
	};


	static double weight()
	{
		return 20.0000000000000;
	}
};
