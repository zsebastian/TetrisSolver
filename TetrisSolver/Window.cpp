#include "Window.h"
#include <exception>
#include <SDL_opengl.h>
#include <vector>
#include "Color.h"
#include <functional>

Window::Window(const std::string& title, int w, int h)
{
	
	open_ = false;
	if (!SDL_Init(SDL_INIT_EVERYTHING))
	{
		win_ = SDL_CreateWindow(title.c_str(), 100, 100, w, h, SDL_WINDOW_SHOWN);
		renderer_ = SDL_CreateRenderer(win_, -1, SDL_RENDERER_ACCELERATED);
		open_ = win_ != nullptr && renderer_ != nullptr;
		if (open_)
		{
			width_ = w;
			height_ = h;
		}
	}
}

Window::~Window()
{
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(win_);
	SDL_Quit();
}

bool Window::Open() const
{
	return win_ != nullptr && open_;
}

void Window::Exit()
{
	open_ = false;
}

void Window::PollEvents()
{
	SDL_Event ev;
	while (SDL_PollEvent(&ev))
	{
		HandleEvent(ev);
	}
}

void Window::Display()
{
	SDL_RenderPresent(renderer_);
	SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0);
	SDL_RenderClear(renderer_);
	for (auto& key : keys_)
	{
		key.second.pressed = false;
		key.second.released = false;
	}
}

void Window::HandleEvent(SDL_Event& ev)
{
	switch (ev.type)
	{
	case SDL_WINDOWEVENT:
		if (ev.window.event == SDL_WINDOWEVENT_CLOSE)
		{
			open_ = false;
		}
		break;
	case SDL_KEYDOWN:
		{
			auto map = keymaps_.find(ev.key.keysym.sym);
			if (map != keymaps_.end())
			{
				keys_[map->second].down = true;
				keys_[map->second].pressed = true;
			}
			break;
		}
	case SDL_KEYUP:
		{
			auto map = keymaps_.find(ev.key.keysym.sym);
			if (map != keymaps_.end())
			{
				keys_[map->second].down = false;
				keys_[map->second].released = true;
			}
			break;
		}
	}
}

void Window::MapKey(SDL_Keycode sym, std::string str)
{
	Key key;
	key.valid = true;
	keys_.insert(std::make_pair(str, key));
	keymaps_.insert(std::make_pair(sym, str));
}

const Key& Window::GetKey(std::string mapping)
{
	auto key = keys_.find(mapping);
	if (key != keys_.end())
	{
		return key->second;
	}
	else
	{
		throw std::out_of_range("No such key mapped: \"" + mapping + "\"");
	}
}

void Window::RenderRectangle(int x, int y, int w, int h, Color color)
{
	try
	{
		SDL_Surface* surface = NewSurface(w, h);
		DrawRectangle(surface, w, h, ToPixel(color));
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
		SDL_FreeSurface(surface);
		SDL_Rect target;
		target.x = x;
		target.y = y;
		target.w = w;
		target.h = h;
		SDL_RenderCopy(renderer_, texture, nullptr, &target);
		SDL_DestroyTexture(texture);
	}
	catch (std::exception& e)
	{
		const char* what = e.what();
		std::cout << what << std::endl;
		throw;
	}
}


Uint32 Window::ToPixel(Color color)
{
	Uint32 ret = 0;
#if SDL_BYTEORDER != SDL_BIG_ENDIAN
	ret |= static_cast<Uint32>(color.get_alpha_byte()) << 24;
	ret |= static_cast<Uint32>(color.get_blue_byte()) << 16;
	ret |= static_cast<Uint32>(color.get_green_byte()) << 8;
	ret |= static_cast<Uint32>(color.get_red_byte()) << 0;;
#else
	ret |= static_cast<Uint32>(color.get_red_byte()) << 24;
	ret |= static_cast<Uint32>(color.get_green_byte()) << 16;
	ret |= static_cast<Uint32>(color.get_blue_byte()) << 8;
	ret |= static_cast<Uint32>(color.get_alpha_byte()) << 0;
#endif
	return ret;
}

SDL_Surface* Window::NewSurface(int w, int h)
{
	/* Create a 32-bit surface with the bytes of each pixel in R,G,B,A order,
		  as expected by OpenGL for textures */
	SDL_Surface *surface;
	Uint32 rmask, gmask, bmask, amask;

	/* SDL interprets each pixel as a 32-bit number, so our masks must depend
	   on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	surface = SDL_CreateRGBSurface(0, w, h, 32,
		rmask, gmask, bmask, amask);

	return surface;
}

/*
* This is a 32-bit pixel function created with help from this
* website: http://www.libsdl.org/intro.en/usingvideo.html
*
* You will need to make changes if you want it to work with
* 8-, 16- or 24-bit surfaces.  Consult the above website for
* more information.
*/
void Window::SetPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	Uint8 *target_pixel = (Uint8 *)surface->pixels + y * surface->pitch + x * 4;
	*(Uint32 *)target_pixel = pixel;
}

/*
* This is an implementation of the Midpoint Circle Algorithm
* found on Wikipedia at the following link:
*
*   http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
*
* The algorithm elegantly draws a circle quickly, using a
* set_pixel function for clarity.
*/
void Window::DrawRectangle(SDL_Surface *surface, int w, int h, Uint32 pixel)
{
	std::function<void(int, int)> set = [surface, pixel, this](int x, int y){ SetPixel(surface, x, y, pixel); };

	for (int i = 0; i < w; ++i)
	{
		for (int j = 0; j < h; ++j)
		{
			set(i, j);
		}
	}
}