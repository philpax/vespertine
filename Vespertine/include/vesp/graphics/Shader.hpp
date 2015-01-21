#pragma once

#include "vesp/Types.hpp"
#include <atlbase.h>
#include <d3dcommon.h>

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct D3D11_INPUT_ELEMENT_DESC;

namespace vesp { namespace graphics {

	enum class ShaderType : U8
	{
		Vertex,
		Pixel
	};

	class Shader
	{
	public:
		Shader(StringPtr name);

	protected:
		void* Compile(StringPtr shaderSource);
		ShaderType type_;
		StringByte name_[32];
	};

	class VertexShader : public Shader
	{
	public:
		VertexShader(StringPtr name);

		bool Load(StringPtr shaderSource, 
			D3D11_INPUT_ELEMENT_DESC* inputLayoutElements, U32 inputLayoutSize);
		void Activate();

	private:
		CComPtr<ID3D11VertexShader> shader_;
		CComPtr<ID3D11InputLayout> inputLayout_;
	};

	class PixelShader : public Shader
	{
	public:
		PixelShader(StringPtr name);

		bool Load(StringPtr shaderSource);
		void Activate();

	private:
		CComPtr<ID3D11PixelShader> shader_;
	};

} }