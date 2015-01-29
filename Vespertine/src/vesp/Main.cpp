#include "vesp/Main.hpp"
#include "vesp/Log.hpp"
#include "vesp/EventManager.hpp"
#include "vesp/Filesystem.hpp"

#include "vesp/graphics/Engine.hpp"

#include <SDL.h>

namespace vesp
{
	bool Initialize(StringPtr name)
	{
		Filesystem::Create();

		Logger::Create("log.txt");
		Log(LogType::Info, "Vespertine (%s %s)", __DATE__, __TIME__);

		EventManager::Create();

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			Log(LogType::Fatal, "Failed to initialize SDL, error: %s", SDL_GetError());
			return false;
		}

		graphics::Engine::Create(name);
		graphics::Engine::Get()->Initialize();

		return true;
	}

	void Shutdown()
	{
		graphics::Engine::Destroy();

		SDL_Quit();

		EventManager::Destroy();

		Log(LogType::Info, "Vespertine shutting down");
		Logger::Destroy();

		Filesystem::Destroy();
	}

	void Loop()
	{
		bool running = true;
		SDL_Event e;

		while (running)
		{
			while (SDL_PollEvent(&e))
			{
				switch (e.type)
				{
				case SDL_QUIT:
					running = false;
					EventManager::Get()->Fire("Quit");
					break;
				};
			}

			graphics::Engine::Get()->Pulse();
		}
	}
}