#pragma once

#include "vesp/Types.hpp"
#include "vesp/Log.hpp"

#include "vesp/util/Timer.hpp"

namespace vesp
{
	bool Initialize(RawStringPtr name);
	void Shutdown();
	void Loop();
	void Quit();

	util::Timer const& GetGlobalTimer();
}