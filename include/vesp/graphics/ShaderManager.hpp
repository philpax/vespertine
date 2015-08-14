#pragma once

#include "vesp/util/GlobalSystem.hpp"

#include "vesp/graphics/Shader.hpp"

#include "vesp/Containers.hpp"
#include "vesp/String.hpp"

namespace vesp { namespace graphics {

	class ShaderManager : public util::GlobalSystem<ShaderManager>
	{
	public:
		void LoadShader(StringView const name, ShaderType type);
		Shader* GetShader(StringView const name, ShaderType type);

		VertexShader* GetVertexShader(StringView const name);
		PixelShader* GetPixelShader(StringView const name);

	private:
		U32 GetKey(StringView const name, ShaderType type);

		UnorderedMap<U32, UniquePtr<Shader>> shaders_;
	};

} }