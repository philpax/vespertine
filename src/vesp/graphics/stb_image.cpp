#include "vesp/Assert.hpp"

#define STBI_ASSERT(x) VESP_ASSERT(x)
#define STB_IMAGE_IMPLEMENTATION
#include "vesp/graphics/stb_image.h"