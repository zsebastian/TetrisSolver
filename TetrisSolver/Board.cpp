#include "Board.h"
#include <exception>

Board::Board(int x, int y, int tile_size)
{
	piece_count_ = 0;

	std::random_device rd;
	random_engine_ = std::mt19937(rd());
	clear_colors();
	x_ = x + tile_size;
	y_ = y + tile_size;
	tile_size_ = tile_size;

	piece_makers.push_back([](int x, int y, int rotation){return Piece::make_I(x, y, rotation); });
	piece_makers.push_back([](int x, int y, int rotation){return Piece::make_J(x, y, rotation); });
	piece_makers.push_back([](int x, int y, int rotation){return Piece::make_L(x, y, rotation); });
	piece_makers.push_back([](int x, int y, int rotation){return Piece::make_O(x, y, rotation); });
	piece_makers.push_back([](int x, int y, int rotation){return Piece::make_S(x, y, rotation); });
	piece_makers.push_back([](int x, int y, int rotation){return Piece::make_T(x, y, rotation); });
	piece_makers.push_back([](int x, int y, int rotation){return Piece::make_Z(x, y, rotation); });

	current_piece_ = random_piece();
	next_piece_queue_.push_back(random_piece());
}

void Board::render(Window& window)
{
	clear_colors();
	render_tiles();
	render_live_piece();
	render_board(window);
}

void Board::render_tiles()
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

void Board::render_live_piece()
{
	Color color = current_piece_.get_color();
	auto tiles = current_piece_.get_tiles();
	for (int i = 0; i < PIECE_SIZE; ++i)
	{
		for (int j = 0; j < PIECE_SIZE; ++j)
		{
			int x = current_piece_.get_x() + i - 2;
			int y = current_piece_.get_y() + j - 2;
			if (x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT && tiles[j * PIECE_SIZE + i] != 0)
			{
				set_color(color, x, y);
			}
		}
	}
}

void Board::render_board(Window& window)
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

void Board::clear_colors()
{
	for (auto& tile : colors_)
	{
		tile = Color::black();
	}
}

void Board::set_color(Color color, int x, int y)
{
	if (x >= BOARD_WIDTH || y >= BOARD_HEIGHT || x < 0 || y < 0)
	{
		throw std::out_of_range("Tile must be inside the board");
	}
	colors_[y * BOARD_WIDTH + x] = color;
}

bool Board::test_collision(const Piece& piece) const
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
bool Board::perform_action(Action action)
{
	if (action == Action::Rotate)
	{
		current_piece_.rotate_right();
		if (test_collision(current_piece_))
		{
			current_piece_.rotate_left();
			return false;
		}
	}
	else if (action == Action::Left)
	{
		current_piece_.move(-1, 0);
		if (test_collision(current_piece_))
		{
			current_piece_.move(1, 0);
			return false;
		}
	}
	else if (action == Action::Right)
	{
		current_piece_.move(1, 0);
		if (test_collision(current_piece_))
		{
			current_piece_.move(-1, 0);
			return false;
		}
	}
	return true;
}

/*
	Does a tick and returns how many rows were cleared in that tick, if any.
	Returns -1 if the game was lost this tick.
*/
int Board::tick()
{
	current_piece_.move(0, 1);
	bool collided = test_collision(current_piece_);

	if (collided)
	{
		while (collided)
		{
			current_piece_.move(0, -1);
			collided = test_collision(current_piece_) && current_piece_.get_y() > -5;
		}

		if (imprint_live_piece())
		{
			return clear_rows();
		}
		else
		{
			return -1;
		}
	}
	return 0;
}

/* Returns number of rows cleared. */
int Board::clear_rows()
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
bool Board::imprint_live_piece()
{
	Color color = current_piece_.get_color();
	auto tiles = current_piece_.get_tiles();
	for (int i = 0; i < PIECE_SIZE; ++i)
	{
		for (int j = 0; j < PIECE_SIZE; ++j)
		{
			int x = current_piece_.get_x() + i - 2;
			int y = current_piece_.get_y() + j - 2;
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
	
	current_piece_ = next_piece_queue_.front();
	next_piece_queue_.pop_front();

	next_piece_queue_.push_back(random_piece());
	++piece_count_;
	return true;
}


int Board::next_random(int min, int max)
{
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(random_engine_);
}

Piece Board::random_piece()
{	
	int piece = next_random(0, piece_makers.size() - 1);

	while (true)
	{
		bool cont = false;
		int rotation = next_random(0, 3);

		int x = next_random(-2, BOARD_WIDTH + 1);	
		//scratch that
		x = 5;
		//that too
		rotation = 0;

		Piece ret = piece_makers[piece](x, 0, rotation);
		
		auto tiles = ret.get_tiles();
		for (int i = 0; i < PIECE_SIZE; ++i)
		{
			for (int j = 0; j < PIECE_SIZE; ++j)
			{
				int x = ret.get_x() + i - 2;
				int y = ret.get_y() + j - 2;
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

int Board::get_piece_count() const
{
	return piece_count_;
}

PlayField Board::create_play_field() const
{
	PlayField ret(BOARD_WIDTH, BOARD_HEIGHT);
	for (int y = 0; y < BOARD_HEIGHT; ++y)
	{
		for (int x = 0; x < BOARD_WIDTH; ++x)
		{
			ret.set(x, y, tiles_[y * BOARD_WIDTH + x].occupied);
		}
	}
	return std::move(ret);
}

const Piece& Board::get_current_piece() const
{
	return current_piece_;
}

const Piece& Board::get_next_piece(int i)
{
	while (next_piece_queue_.size()  <= i)
	{
		next_piece_queue_.push_back(random_piece());
	}
	return next_piece_queue_[i];
}