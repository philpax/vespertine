#pragma once

#include "vesp/util/GlobalSystem.hpp"

#include "vesp/graphics/Shader.hpp"

#include "vesp/Containers.hpp"
#include "vesp/String.hpp"

namespace vesp { namespace graphics {

	class ShaderManager : public util::GlobalSystem<ShaderManager>
	{
	public:
		ShaderManager();

		void LoadShader(StringView const name, ShaderType type);
		Shader* GetShader(StringView const name, ShaderType type) const;

		VertexShader* GetVertexShader(StringView const name) const;
		PixelShader* GetPixelShader(StringView const name) const;

	private:
		void ReloadAll();
		U32 GetKey(StringView const name, ShaderType type) const;

		UnorderedMap<U32, UniquePtr<Shader>> shaders_;
	};

} }