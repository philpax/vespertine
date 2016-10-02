#pragma once

#include "vesp/String.hpp"

namespace vesp { namespace graphics {

	struct Image
	{
	public:
		UniquePtr<U8[]> data;
		U32 sizeX;
		U32 sizeY;
		U32 components;

		static Image FromPath(StringView path);
	};

	static_assert(sizeof(Image) == 16, "Image size incorrect");

} }