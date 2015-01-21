#include "vesp/graphics/Window.hpp"
#include "vesp/math/Vector.hpp"

#include <SDL.h>
#include <SDL_syswm.h>

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

	IVec2 Window::GetPosition()
	{
		IVec2 ret;
		SDL_GetWindowPosition(this->window_, &ret.x, &ret.y);
		return ret;
	}

	IVec2 Window::GetSize()
	{
		IVec2 ret;
		SDL_GetWindowSize(this->window_, &ret.x, &ret.y);
		return ret;
	}

	void* Window::GetSystemRepresentation()
	{
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		// TODO error handling
		SDL_GetWindowWMInfo(this->window_, &info);
		return info.info.win.window;
	}

	bool Window::IsFullscreen()
	{
		return (SDL_GetWindowFlags(this->window_) & SDL_WINDOW_FULLSCREEN);
	}

	void Window::Pulse()
	{
	}
} }