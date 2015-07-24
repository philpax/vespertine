#include "vesp/Main.hpp"
#include "vesp/Log.hpp"
#include "vesp/EventManager.hpp"
#include "vesp/FileSystem.hpp"
#include "vesp/InputManager.hpp"

#include "vesp/graphics/Engine.hpp"
#include "vesp/graphics/Window.hpp"

#include "vesp/math/Vector.hpp"

namespace vesp
{
	bool Initialize(RawStringPtr name)
	{
		FileSystem::Create();

		Logger::Create("log.txt");
		LogInfo("Vespertine (%s %s)", __DATE__, __TIME__);

		EventManager::Create();
		InputManager::Create();

		graphics::Engine::Create(name);
		graphics::Engine::Get()->Initialize();

		return true;
	}

	void Shutdown()
	{
		graphics::Engine::Destroy();
		InputManager::Destroy();

		EventManager::Destroy();

		LogInfo("Vespertine shutting down");
		Logger::Destroy();

		FileSystem::Destroy();
	}

	void Loop()
	{
		bool running = true;

		while (running)
		{
			InputManager::Get()->Pulse();

			MSG msg{};
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE) > 0)
			{
				switch (msg.message)
				{
				case WM_QUIT:
					running = false;
					EventManager::Get()->Fire("Engine.Quit");
					break;
				}
				
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				InputManager::Get()->FeedEvent(&msg);
			}

			graphics::Engine::Get()->Pulse();
		}
	}
}