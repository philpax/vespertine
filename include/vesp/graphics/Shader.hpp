#pragma once

#include "vesp/Types.hpp"
#include "vesp/Containers.hpp"
#include "vesp/String.hpp"
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
		Shader(StringView const name);

	protected:
		void* Compile(StringView const shaderSource);
		ShaderType type_;
		StringByte name_[32] = {0};
	};

	class VertexShader : public Shader
	{
	public:
		VertexShader(StringView const name);

		bool Load(StringView const shaderSource,
			ArrayView<D3D11_INPUT_ELEMENT_DESC> inputLayoutElements);
		void Activate();

	private:
		CComPtr<ID3D11VertexShader> shader_;
		CComPtr<ID3D11InputLayout> inputLayout_;
	};

	class PixelShader : public Shader
	{
	public:
		PixelShader(StringView const name);

		bool Load(StringView const shaderSource);
		void Activate();

	private:
		CComPtr<ID3D11PixelShader> shader_;
	};

} }