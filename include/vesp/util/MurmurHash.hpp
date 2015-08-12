#pragma once

#include "vesp/Types.hpp"

namespace vesp { class StringView; }

namespace vesp { namespace util {

	void MurmurHash(const void* key, S32 len, U32 seed, void* out);
	U32 MurmurHash(const void* key, S32 len, U32 seed = 0x6598f0a1);
	U32 MurmurHash(StringView const key, U32 seed = 0x6598f0a1);

} }