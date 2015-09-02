#include "Solver.h"
#include "EvaluationFunctions.h"

Solver::Solver()
	:current_piece_count_(-1)
{
	evaluations_.emplace_back(EvaluationFunction<0>(), EvaluationFunction<0>::weight());
	evaluations_.emplace_back(EvaluationFunction<1>(), EvaluationFunction<1>::weight());
	evaluations_.emplace_back(EvaluationFunction<2>(), EvaluationFunction<2>::weight());
	evaluations_.emplace_back(EvaluationFunction<3>(), EvaluationFunction<3>::weight());
	evaluations_.emplace_back(EvaluationFunction<4>(), EvaluationFunction<4>::weight());
	evaluations_.emplace_back(EvaluationFunction<5>(), EvaluationFunction<5>::weight());
	evaluations_.emplace_back(EvaluationFunction<6>(), EvaluationFunction<6>::weight());
}

void Solver::update(Board& board)
{
	if (board.get_piece_count() != current_piece_count_)
	{
		auto& current_piece = board.get_current_piece();
		if (!board.test_collision(current_piece))
		{
			current_piece_count_ = board.get_piece_count();
			start_search(board);
		}
	}

	play_recorded_actions(board);
}

void Solver::play_recorded_actions(Board& board)
{
	if (action_recording_.size() == 0)
	{
		board.tick();
		return;
	}
	else
	{
		auto& top_frame = action_recording_.front();
		if (top_frame.size() == 0)
		{
			board.tick();
			action_recording_.pop_front();
		}
		else
		{
			board.perform_action(top_frame.front());
			top_frame.pop_front();
		}
	}
}

void Solver::start_search(Board& board)
{
	PlayField play_field = board.create_play_field();
	
	std::vector<Piece> piece_queue;
	piece_queue.push_back(board.get_current_piece());
	for (int i = 0; i < 1; ++i)
	{
		piece_queue.push_back(board.get_next_piece(i));
	}
	StateArray states = build_states(play_field, piece_queue);
	auto& start_piece = board.get_current_piece();
	auto start = states[start_piece.get_x()][start_piece.get_y()][start_piece.get_rotation()][0];
	auto best = search(states, play_field, play_field, 0, piece_queue, std::vector<Piece>());
	action_recording_ = std::move(make_recording(best, start));
}

State::ptr Solver::search(StateArray& states, PlayField& original_play_field, PlayField& play_field, int depth, const std::vector<Piece>& piece_queue, const std::vector<Piece>& locked_pieces)
{
	if (depth == piece_queue.size())
	{
		return nullptr;
	}

	StateQueue queue;
	const Piece& current = piece_queue[depth];

	queue.enqueue(states[current.get_x()][current.get_y()][current.get_rotation()][depth]);
	State::ptr best_state;
	double best_state_value;

	while (!queue.is_empty())
	{
		auto state = queue.dequeue();
		const Piece& current = state->piece;

		Piece move_left = current;
		Piece move_right = current;
		Piece rotate_right = current;
		Piece move_down = current;
		move_left.move(-1, 0);
		move_right.move(1, 0);

		rotate_right.rotate_right();
		move_down.move(0, 1);

		if (current.get_max_rotations() != 1)
		{
			add_state_to_queue(states, queue, state, play_field, rotate_right, depth);
		}
		add_state_to_queue(states, queue, state, play_field, move_left, depth);
		add_state_to_queue(states, queue, state, play_field, move_right, depth);
		
		if (!add_state_to_queue(states, queue, state, play_field, move_down, depth))
		{
			PlayField next_play_field = play_field;
			next_play_field.imprint(current);
			std::vector<Piece> locked = locked_pieces;
			locked.push_back(current);

			State::ptr next_search = search(states, original_play_field, next_play_field, depth + 1, piece_queue, locked);
			if (next_search == nullptr)
			{
				
				double eval = evaluate_play_field(original_play_field, next_play_field, locked);
				if (best_state == nullptr || best_state_value > eval)
				{
					best_state = states[current.get_x()][current.get_y()][current.get_rotation()][depth];
					best_state_value = eval;
				}
			}
		}
	}
	return best_state;
}

Solver::StateArray Solver::build_states(const PlayField& play_field, const std::vector<Piece>& piece_queue) const
{
	int w = play_field.get_width();
	int h = play_field.get_height();
	int d = 4;
	auto ret = StateArray(w, h, d, piece_queue.size());
	
	for (int z = 0; z < d; ++z)
	{
		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				for (int s = 0; s < piece_queue.size(); ++s)
				{
					ret[x][y][z][s] = std::make_shared<State>();
					ret[x][y][z][s]->visited = false;
					ret[x][y][z][s]->piece = piece_queue[s];
					ret[x][y][z][s]->piece.set(x, y, z);
				}
			}
		}
	}
	return std::move(ret);
}

bool Solver::add_state_to_queue(StateArray& states, StateQueue& queue, State::ptr prev_state, const PlayField& play_field, const Piece& piece, int depth)
{
	int x = piece.get_x();
	int y = piece.get_y();
	int z = piece.get_rotation();

	if (play_field.test_collision(piece))
	{
		return false;
	}

	auto& state = states[x][y][z][depth];
	if (x == prev_state->piece.get_x() &&
		y == prev_state->piece.get_y() &&
		z == prev_state->piece.get_rotation())
	{
		return true;
	}
	if (state->visited)
	{
		return true;
	}

	state->visited = true;
	state->predecessor = prev_state;
	queue.enqueue(state);
	return true;
}

double Solver::evaluate_play_field(const PlayField& from, const PlayField& to, const std::vector<Piece>& piece_queue) const
{
	double total = 0;
	for (auto& eval : evaluations_)
	{
		total += eval.first(from, to, piece_queue) * eval.second;
	}
	return total;
}

Solver::Recording Solver::make_recording(State::ptr state, State::ptr start) const
{
	Recording ret;
	
	State::ptr current = state;
	while (current != nullptr && !current->predecessor.expired() && start != current)
	{
		ret.emplace_front();
		State::ptr prev = current->predecessor.lock();
		auto current_piece = current->piece;
		auto prev_piece = prev->piece;

			//get in rotation
		while (current_piece.get_rotation() != prev_piece.get_rotation())
		{
			current_piece.rotate_left();
			ret.front().emplace_front(Board::Action::Rotate);
		}

		//move to the right x
		while (current_piece.get_x() < prev_piece.get_x())
		{
			current_piece.move(1, 0);
			ret.front().emplace_front(Board::Action::Left);
		}
		while (current_piece.get_x() > prev_piece.get_x())
		{
			current_piece.move(-1, 0);
			ret.front().emplace_front(Board::Action::Right);
		}

		current = prev;
	}

	return std::move(ret);
}
