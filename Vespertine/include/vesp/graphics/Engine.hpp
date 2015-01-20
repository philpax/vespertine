#pragma once

#include "vesp/util/GlobalSystem.hpp"
#include "vesp/Types.hpp"

namespace vesp { namespace graphics {

	class Window;

	class Engine : public util::GlobalSystem<Engine>
	{
	public:
		Engine(StringPtr title);
		~Engine();

		void Pulse();

	private:
		std::unique_ptr<Window> window_;
	};

} }