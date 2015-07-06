#pragma warning(disable: 4005)
#include "vesp/graphics/Shader.hpp"
#include "vesp/graphics/Engine.hpp"
#include "vesp/Log.hpp"

#include "vesp/Assert.hpp"

#include <d3d11.h>
#include <d3dcompiler.h>

namespace vesp { namespace graphics {

	// Shader
	Shader::Shader(StringPtr name)
	{
		strcpy_s(this->name_, name);
	}

	void* Shader::Compile(StringPtr shaderSource)
	{
		HRESULT hr = S_OK;
		const bool DebuggingEnabled = false;
		U32 shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
		if (DebuggingEnabled)
		{
			shaderFlags |= D3DCOMPILE_DEBUG;
			shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
		}

		StringPtr target = nullptr;
		switch (this->type_)
		{
		case ShaderType::Pixel:
			target = "ps_4_0";
			break;
		case ShaderType::Vertex:
			target = "vs_4_0";
			break;
		default:
			LogError("Unsupported shader type! Type: %d", this->type_);
			return nullptr;
		}

		CComPtr<ID3DBlob> errorBlob = nullptr;
		ID3DBlob* output = nullptr;

		hr = D3DCompile(
			shaderSource, strlen(shaderSource), 
			this->name_, nullptr, nullptr, "main", target, 
			shaderFlags, 0, &output, &errorBlob);

		if (FAILED(hr))
		{
			auto error = static_cast<StringPtr>(errorBlob->GetBufferPointer());
			LogError("Failed to compile shader %s! Error: %s",
				this->name_, error);

			if (output) output->Release();
		}

		return output;
	}

	// Vertex Shader
	VertexShader::VertexShader(StringPtr name)
		: Shader(name)
	{
		this->type_ = ShaderType::Vertex;
	}

	bool VertexShader::Load(
		StringPtr shaderSource, D3D11_INPUT_ELEMENT_DESC* inputLayoutElements, 
		U32 inputLayoutSize)
	{
		CComPtr<ID3DBlob> blob = static_cast<ID3DBlob*>(this->Compile(shaderSource));

		if (!blob) return false;

		auto device = Engine::Device;

		HRESULT hr = device->CreateVertexShader(
			blob->GetBufferPointer(), blob->GetBufferSize(), 
			nullptr, &this->shader_);

		if (FAILED(hr))
		{
			LogError("Failed to create shader %s! Error: %X",
				this->name_, hr);
			return false;
		}

		hr = device->CreateInputLayout(inputLayoutElements, inputLayoutSize,
			blob->GetBufferPointer(), blob->GetBufferSize(),
			&this->inputLayout_);

		if (FAILED(hr))
		{
			LogError("Failed to load shader %s! Error (CreateInputLayout): %X",
				this->name_, hr);
			return false;
		}

		return true;
	}

	void VertexShader::Activate()
	{
		VESP_ASSERT(this->shader_);
		auto immediateContext = Engine::ImmediateContext;
		immediateContext->IASetInputLayout(this->inputLayout_);
		immediateContext->VSSetShader(this->shader_, nullptr, 0);
	}
	
	// Pixel Shader
	PixelShader::PixelShader(StringPtr name)
		: Shader(name)
	{
		this->type_ = ShaderType::Pixel;
	}

	bool PixelShader::Load(StringPtr shaderSource)
	{
		CComPtr<ID3DBlob> blob = static_cast<ID3DBlob*>(this->Compile(shaderSource));

		if (!blob) return false;

		HRESULT hr = Engine::Device->CreatePixelShader(
			blob->GetBufferPointer(), blob->GetBufferSize(), 
			nullptr, &this->shader_);

		return SUCCEEDED(hr);
	}

	void PixelShader::Activate()
	{
		VESP_ASSERT(this->shader_);
		Engine::ImmediateContext->PSSetShader(
			this->shader_, nullptr, 0);
	}

} }