#pragma once

#include "vesp/Types.hpp"
#include "vesp/Log.hpp"

namespace vesp
{
	bool Initialize(RawStringPtr name);
	void Shutdown();
	void Loop();
	void Quit();
}