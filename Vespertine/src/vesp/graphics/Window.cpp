#include "vesp/graphics/Window.hpp"
#include "vesp/math/Vector.hpp"

#include <SDL.h>

namespace vesp { namespace graphics {

	Window::Window(StringPtr title, IVec2 size)
	{
		this->window_ = SDL_CreateWindow(
			title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size.x, size.y, SDL_WINDOW_OPENGL);
	}

	Window::~Window()
	{
		if (this->window_)
			SDL_DestroyWindow(this->window_);
	}

	void Window::SetTitle(StringPtr title)
	{
		SDL_SetWindowTitle(this->window_, title);
	}

	void Window::SetPosition(IVec2 position)
	{
		SDL_SetWindowPosition(this->window_, position.x, position.y);
	}

	void Window::SetSize(IVec2 size)
	{
		SDL_SetWindowSize(this->window_, size.x, size.y);
	}

	void Window::Pulse()
	{
	}
} }