#pragma once

#include "vesp/Types.hpp"
#include "vesp/Log.hpp"

namespace vesp
{
	bool Initialize(StringPtr name);
	void Shutdown();
	void Loop();
}