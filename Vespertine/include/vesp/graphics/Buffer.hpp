#pragma once

#include "vesp/Types.hpp"
#include "vesp/graphics/Vertex.hpp"

#include <atlbase.h>
#include <d3d11.h>

namespace vesp { namespace graphics {

	template <typename T>
	class Buffer
	{
	public:
		bool Create(T* data, U32 count, U32 bindFlags, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);

	protected:
		CComPtr<ID3D11Buffer> buffer_;
	};

	class VertexBuffer : public Buffer<Vertex>
	{
	public:
		bool Create(Vertex* data, U32 count, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);

		void Use(U32 slot);
	};

} }