#include "Board.h"
#include <exception>

Board::Board(int x, int y, int tile_size)
{
	std::random_device rd;
	random_engine_ = std::mt19937(rd());
	ClearColors();
	x_ = x + tile_size;
	y_ = y + tile_size;
	tile_size_ = tile_size;

	piece_makers.push_back([](int x, int y, int rotation){return Piece::MakeI(x, y, rotation); });
	piece_makers.push_back([](int x, int y, int rotation){return Piece::MakeJ(x, y, rotation); });
	piece_makers.push_back([](int x, int y, int rotation){return Piece::MakeL(x, y, rotation); });
	piece_makers.push_back([](int x, int y, int rotation){return Piece::MakeO(x, y, rotation); });
	piece_makers.push_back([](int x, int y, int rotation){return Piece::MakeS(x, y, rotation); });
	piece_makers.push_back([](int x, int y, int rotation){return Piece::MakeT(x, y, rotation); });
	piece_makers.push_back([](int x, int y, int rotation){return Piece::MakeZ(x, y, rotation); });

	current_piece_ = RandomPiece();
	next_piece_ = RandomPiece();
}


void Board::Render(Window& window)
{
	ClearColors();
	RenderTiles();
	RenderLivePiece();
	RenderBoard(window);
}

void Board::RenderTiles()
{
	for (int x = 0; x < BOARD_WIDTH; ++x)
	{
		for (int y = 0; y < BOARD_HEIGHT; ++y)
		{
			if (tiles_[y * BOARD_WIDTH + x].occupied)
			{
				colors_[y * BOARD_WIDTH + x] = tiles_[y * BOARD_WIDTH + x].color;
			}
		}
	}
}

void Board::RenderLivePiece()
{
	Color color = current_piece_.GetColor();
	auto tiles = current_piece_.GetTiles();
	for (int i = 0; i < PIECE_SIZE; ++i)
	{
		for (int j = 0; j < PIECE_SIZE; ++j)
		{
			int x = current_piece_.GetX() + i - 2;
			int y = current_piece_.GetY() + j - 2;
			if (x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT && tiles[j * PIECE_SIZE + i] != 0)
			{
				SetColor(color, x, y);
			}
		}
	}
}

void Board::RenderBoard(Window& window)
{
	for (int x = 0; x < BOARD_WIDTH; ++x)
	{
		window.RenderRectangle(x_ + (x * tile_size_), y_ + (-1 * tile_size_), tile_size_, tile_size_, Color::white());
		window.RenderRectangle(x_ + (x * tile_size_), y_ + ((BOARD_HEIGHT) * tile_size_), tile_size_, tile_size_, Color::white());
	}

	for (int y = -1; y <= BOARD_HEIGHT; ++y)
	{
		window.RenderRectangle(x_ + (-1 * tile_size_), y_ + (y * tile_size_), tile_size_, tile_size_, Color::white());
		window.RenderRectangle(x_ + ((BOARD_WIDTH) * tile_size_), y_ + (y * tile_size_), tile_size_, tile_size_, Color::white());
	}

	for (int x = 0; x < BOARD_WIDTH; ++x)
	{
		for (int y = 0; y < BOARD_HEIGHT; ++y)
		{
			window.RenderRectangle(x_ + (x * tile_size_), y_ + (y * tile_size_), tile_size_, tile_size_, colors_[y * BOARD_WIDTH + x]);
		}
	}
}

void Board::ClearColors()
{
	for (auto& tile : colors_)
	{
		tile = Color::black();
	}
}

void Board::SetColor(Color color, int x, int y)
{
	if (x >= BOARD_WIDTH || y >= BOARD_HEIGHT || x < 0 || y < 0)
	{
		throw std::out_of_range("Tile must be inside the board");
	}
	colors_[y * BOARD_WIDTH + x] = color;
}

bool Board::TestCollision() const
{
	auto tiles = current_piece_.GetTiles();
	for (int i = 0; i < PIECE_SIZE; ++i)
	{
		for (int j = 0; j < PIECE_SIZE; ++j)
		{
			int x = current_piece_.GetX() + i - 2;
			int y = current_piece_.GetY() + j - 2;
			if (tiles[j * PIECE_SIZE + i] != 0)
			{
				if (y >= BOARD_HEIGHT)
				{
					return true;
				}
				else if (x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT && tiles_[y * BOARD_WIDTH + x].occupied)
				{
					return true;
				}
				else if (!(x >= 0 && x < BOARD_WIDTH))
				{
					return true;
				}
			}
		}
	}
	return false;
}

/*
	Returns true if the action was legal
*/
bool Board::PerformAction(Action action)
{
	if (action == Action::Rotate)
	{
		current_piece_.RotateRight();
		if (TestCollision())
		{
			current_piece_.RotateLeft();
			return false;
		}
	}
	else if (action == Action::Left)
	{
		current_piece_.Move(-1, 0);
		if (TestCollision())
		{
			current_piece_.Move(1, 0);
			return false;
		}
	}
	else if (action == Action::Right)
	{
		current_piece_.Move(1, 0);
		if (TestCollision())
		{
			current_piece_.Move(-1, 0);
			return false;
		}
	}
	return true;
}

/*
	Does a tick and returns how many rows were cleared in that tick, if any.
	Returns -1 if the game was lost this tick.
*/
int Board::Tick()
{
	current_piece_.Move(0, 1);
	bool collided = TestCollision();

	if (collided)
	{
		while (collided)
		{
			current_piece_.Move(0, -1);
			collided = TestCollision() && current_piece_.GetY() > -5;
		}

		if (ImprintLivePiece())
		{
			return ClearRows();
		}
		else
		{
			return -1;
		}
	}
	return 0;
}

/* Returns number of rows cleared. */
int Board::ClearRows()
{
	int cleared = 0;
	for (int row = 0; row < BOARD_HEIGHT; ++row)
	{
		bool occ = true;
		for (int col = 0; col < BOARD_WIDTH; ++col)
		{
			if (!tiles_[row * BOARD_WIDTH + col].occupied)
			{
				occ = false;
				break;
			}
		}
		if (occ)
		{
			++cleared;
			for (int col = 0; col < BOARD_WIDTH; ++col)
			{
				tiles_[row * BOARD_WIDTH + col].occupied = false;
			}
			for (int y = row; y >= 1; --y)
			{
				for (int x = 0; x < BOARD_WIDTH; ++x)
				{
					tiles_[y* BOARD_WIDTH + x] = tiles_[(y - 1) * BOARD_WIDTH + x];
				}
			}
		}
	}
	return cleared;
}

/* returns true if the piece was imprintable. Ie: if this function returns false,
	the game is lost */
bool Board::ImprintLivePiece()
{
	Color color = current_piece_.GetColor();
	auto tiles = current_piece_.GetTiles();
	for (int i = 0; i < PIECE_SIZE; ++i)
	{
		for (int j = 0; j < PIECE_SIZE; ++j)
		{
			int x = current_piece_.GetX() + i - 2;
			int y = current_piece_.GetY() + j - 2;
			if (x >= 0 && x < BOARD_WIDTH && y >= -1 && y < BOARD_HEIGHT && tiles[j * PIECE_SIZE + i] != 0)
			{
				if (y == -1)
				{
					return false;
				}
				tiles_[y * BOARD_WIDTH + x].occupied = true;
				tiles_[y * BOARD_WIDTH + x].color = color;
			}
		}
	}
	
	current_piece_ = next_piece_;
	next_piece_ = RandomPiece();
	return true;
}


int Board::NextRandom(int min, int max)
{
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(random_engine_);
}

Piece Board::RandomPiece()
{	
	int piece = NextRandom(0, piece_makers.size() - 1);

	while (true)
	{
		bool cont = false;
		int rotation = NextRandom(0, 3);

		int x = NextRandom(-2, BOARD_WIDTH + 1);	
		//scratch that
		x = 5;
		//that too
		rotation = 0;

		Piece ret = piece_makers[piece](x, -2, rotation);
		
		auto tiles = ret.GetTiles();
		for (int i = 0; i < PIECE_SIZE; ++i)
		{
			for (int j = 0; j < PIECE_SIZE; ++j)
			{
				int x = ret.GetX() + i - 2;
				int y = ret.GetY() + j - 2;
				if (tiles[j * PIECE_SIZE + i] != 0 && (x < 0  || x >= BOARD_WIDTH))
				{
					cont = true;
				}
			}
		}
		if (!cont)
		{
			return ret;
		}
	}
}