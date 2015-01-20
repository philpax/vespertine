#pragma once

#include "vesp/Types.hpp"

struct SDL_Window;

namespace vesp { namespace graphics {

	class Window
	{
	public:
		Window(StringPtr title, IVec2 size);
		~Window();

		void SetTitle(StringPtr title);
		void SetPosition(IVec2 position);
		void SetSize(IVec2 size);

		void Pulse();

	private:
		SDL_Window* window_;
	};

} }