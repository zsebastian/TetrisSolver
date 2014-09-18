#include <SDL.h>
#include "Window.h"
#include "Board.h"
#include "Timer.h"

int main(int argc, char *argv[])
{
	Window win("Tetris!", 800, 600);
	win.MapKey(SDLK_UP, "up");
	win.MapKey(SDLK_DOWN, "down");
	win.MapKey(SDLK_LEFT, "left");
	win.MapKey(SDLK_RIGHT, "right");
	Board board(0, 0, 16);

	float rot = 0.0f;

	Timer timer;
	timer.Start();
	int tick_time = 1000;

	while (win.Open())
	{
		win.PollEvents();
		
		if (win.GetKey("up").pressed)
		{
			board.PerformAction(Board::Rotate);
		}

		if (win.GetKey("down").down)
		{
			tick_time = 100;
		}
		else
		{
			tick_time = 500;
		}

		if (win.GetKey("left").pressed)
		{
			board.PerformAction(Board::Left);
		}
		if (win.GetKey("right").pressed)
		{
			board.PerformAction(Board::Right);
		}
		
		if (timer.ElapsedMilliseconds() > tick_time)
		{
			int tick = board.Tick();

			if (tick == -1)
			{
				win.Exit();
			}
			timer.Start();
		}

		board.Render(win);

		win.Display();
	}
	return 0;
}

