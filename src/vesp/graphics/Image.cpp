#include "vesp/graphics/Image.hpp"
#include "vesp/graphics/stb_image.h"

#include "vesp/FileSystem.hpp"

namespace vesp { namespace graphics {

	Image Image::FromPath(StringView path)
	{
		auto file = FileSystem::Get()->Open(path, FileSystem::Mode::ReadBinary);
		auto imageData = file.Read<U8>();

		S32 sizeX = 0, sizeY = 0, comp = 0;

		Image image;
		image.data.reset(stbi_load_from_memory(imageData.data(), imageData.size(), &sizeX, &sizeY, &comp, 1));
		image.sizeX = sizeX;
		image.sizeY = sizeY;
		image.components = comp;
		
		VESP_ASSERT(image.data);

		return image;
	}

} }