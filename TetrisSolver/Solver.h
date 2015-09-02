#pragma once

#include "Board.h"
#include <deque>
#include "MultiArray.h"
#include "StateQueue.h"

class Solver
{
public:
	Solver();
	void update(Board& board);
private:
	typedef std::deque<std::deque<Board::Action>> Recording;
	typedef MultiArray<State::ptr, 4> StateArray;
	State::ptr search(StateArray& states, PlayField& original_play_field, PlayField& play_field, int depth, const std::vector<Piece>& piece_queue, const std::vector<Piece>& locked_pieces);
	void start_search(Board& board);
	void play_recorded_actions(Board& board);
	StateArray build_states(const PlayField& play_field, const std::vector<Piece>& piece_queue) const;
	Recording make_recording(State::ptr prev_state, State::ptr start) const;

	double evaluate_play_field(const PlayField& from, const PlayField& to, const std::vector<Piece>& piece_queue) const;

	typedef std::function<double(const PlayField& from, const PlayField& to, const std::vector<Piece>& piece_queue)> evaluation_function;
	std::vector<std::pair<evaluation_function, double>> evaluations_;

	/* return true if the state is valid, regardless if the state to the queue was added or not */
	bool add_state_to_queue(StateArray& states, StateQueue& queue, State::ptr prev_state, const PlayField& board, const Piece& piece, int depth);

	Recording action_recording_;
	int current_piece_count_;
};