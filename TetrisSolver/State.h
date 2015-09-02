#pragma once

/*
 *	http://meatfighter.com/nintendotetrisai/#The_Algorithm
 */

#include <utility>
#include <memory>
#include "Board.h"

struct State
{
	typedef std::shared_ptr<State> ptr;
	typedef std::weak_ptr<State> weak_ptr;

	std::deque<std::deque<Board::Action>> recording_;

	Piece piece;
	bool visited;
	weak_ptr predecessor;
	weak_ptr next;
};