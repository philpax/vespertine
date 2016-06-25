#include "vesp/graphics/ShaderManager.hpp"

#include "vesp/FileSystem.hpp"
#include "vesp/Assert.hpp"

#pragma warning(disable: 4005)
#include <D3D11.h>

namespace vesp { namespace graphics {

	void ShaderManager::LoadShader(StringView const name, ShaderType type)
	{
		auto basePath = Concat("data/shaders/", name);
		RawStringPtr extension;
		switch (type)
		{
		case ShaderType::Vertex:
			extension = ".vsh";
			break;
		case ShaderType::Pixel:
			extension = ".psh";
			break;
		}
		
		auto filePath = Concat(basePath, extension);
		auto file = FileSystem::Get()->Open(filePath, FileSystem::Mode::Read);
		VESP_ASSERT(file.Exists());

		auto shaderContents = file.Read<StringByte>();

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R16G16_UNORM, 0,
			D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R16G16_UNORM, 0,
			D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0,
			D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		UniquePtr<Shader> shader;
		switch (type)
		{
		case ShaderType::Vertex:
		{
			auto vertexShader = std::make_unique<VertexShader>(name);
			VESP_ENFORCE(vertexShader->Load(shaderContents, layout));
			shader = std::move(vertexShader);
			break;
		}
		case ShaderType::Pixel:
		{
			auto pixelShader = std::make_unique<PixelShader>(name);
			VESP_ENFORCE(pixelShader->Load(shaderContents));
			shader = std::move(pixelShader);
			break;
		}
		}

		this->shaders_[this->GetKey(name, type)] = std::move(shader);
	}

	Shader* ShaderManager::GetShader(StringView const name, ShaderType type)
	{
		auto it = this->shaders_.find(this->GetKey(name, type));
		VESP_ASSERT(it != this->shaders_.end());
		return it->second.get();
	}

	VertexShader* ShaderManager::GetVertexShader(StringView const name)
	{
		return static_cast<VertexShader*>(
			this->GetShader(name, ShaderType::Vertex));
	}

	PixelShader* ShaderManager::GetPixelShader(StringView const name)
	{
		return static_cast<PixelShader*>(
			this->GetShader(name, ShaderType::Pixel));
	}

	U32 ShaderManager::GetKey(StringView const name, ShaderType type)
	{
		// Arbitrary, may change
		return (util::MurmurHash(name) & 0xFFFFFF00) | U32(type);
	}

} }