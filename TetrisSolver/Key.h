#pragma once

struct Key
{
public:
	Key()
		: down(false), released(false), pressed(false), valid(false)
	{}

	bool down;
	bool released;
	bool pressed;
	bool valid;
};