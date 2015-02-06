#include "vesp/Main.hpp"
#include "vesp/Log.hpp"
#include "vesp/EventManager.hpp"
#include "vesp/FileSystem.hpp"
#include "vesp/InputManager.hpp"

#include "vesp/graphics/Engine.hpp"

#include <SDL.h>

namespace vesp
{
	bool Initialize(StringPtr name)
	{
		FileSystem::Create();

		Logger::Create("log.txt");
		LogInfo("Vespertine (%s %s)", __DATE__, __TIME__);

		EventManager::Create();

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			LogFatal("Failed to initialize SDL, error: %s", SDL_GetError());
			return false;
		}

		InputManager::Create();

		graphics::Engine::Create(name);
		graphics::Engine::Get()->Initialize();

		return true;
	}

	void Shutdown()
	{
		graphics::Engine::Destroy();
		InputManager::Destroy();

		SDL_Quit();

		EventManager::Destroy();

		LogInfo("Vespertine shutting down");
		Logger::Destroy();

		FileSystem::Destroy();
	}

	void Loop()
	{
		bool running = true;
		SDL_Event e;

		while (running)
		{
			InputManager::Get()->Pulse();

			while (SDL_PollEvent(&e))
			{
				switch (e.type)
				{
				case SDL_QUIT:
					running = false;
					EventManager::Get()->Fire("Quit");
					break;
				};

				InputManager::Get()->FeedEvent(&e);
			}

			graphics::Engine::Get()->Pulse();
		}
	}
}