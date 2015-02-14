#pragma once

#include "vesp/Types.hpp"
#include <intrin.h>

namespace vesp 
{
	void AssertFail(StringPtr error, StringPtr file, U32 line);
}

#define VESP_BREAK() __debugbreak()

#define VESP_ENFORCE(cond) \
	do \
	{ \
		if (!(cond)) \
		{ \
			vesp::AssertFail(#cond, __FILE__, __LINE__); \
			VESP_BREAK(); \
		} \
	} while(__LINE__ == -1)

#ifdef VESP_ASSERT_ENABLED
	#define VESP_ASSERT(cond) VESP_ENFORCE(cond)
#else
	#define VESP_ASSERT(cond) do { (void)sizeof(cond); } while(0)
#endif