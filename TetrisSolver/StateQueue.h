#pragma once

#include "State.h"
class StateQueue
{
public:
	void enqueue(State::ptr state)
	{
		if (head == nullptr)
		{
			head = state;
			tail = state;
		}
		else
		{
			tail->next = state;
			tail = state;
		}
		state->next.lock() = nullptr;
	}

	State::ptr dequeue()
	{
		auto state = head;
		if (head != nullptr)
		{
			if (head == tail)
			{
				head = nullptr;
				tail = nullptr;
			}
			else
			{
				head = head->next.lock();
			}
		}
		return state;
	}

	bool is_empty()
	{
		return head == nullptr;
	}
	
	void clear()
	{
		while (!is_empty())
		{
			dequeue();
		}
	}

private:
	State::ptr head;
	State::ptr tail;
};