#include "vesp/graphics/Engine.hpp"
#include "vesp/graphics/Window.hpp"
#include "vesp/math/Vector.hpp"

namespace vesp { namespace graphics {

	Engine::Engine(StringPtr title)
	{
		this->window_ = std::make_unique<Window>(title, IVec2(1280, 800));
	}

	Engine::~Engine()
	{
	}

	void Engine::Pulse()
	{
		this->window_->Pulse();
	}

} }