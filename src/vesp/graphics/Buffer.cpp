#pragma warning(disable: 4005)
#include "vesp/graphics/Buffer.hpp"

namespace vesp { namespace graphics {

	bool VertexBuffer::Create(ArrayView<Vertex> const array, D3D11_USAGE usage)
	{
		return Buffer<Vertex>::Create(array, D3D11_BIND_VERTEX_BUFFER, usage);
	}

	void VertexBuffer::Use(U32 slot)
	{
		U32 stride = sizeof(Vertex);
		U32 offset = 0;
		Engine::ImmediateContext->IASetVertexBuffers(
			slot, 1, &this->buffer_.p, &stride, &offset);
	}

	bool IndexBuffer::Create(ArrayView<U32> const array, D3D11_USAGE usage)
	{
		return Buffer<U32>::Create(array, D3D11_BIND_INDEX_BUFFER, usage);
	}

	void IndexBuffer::Use()
	{
		U32 stride = sizeof(U32);
		U32 offset = 0;
		Engine::ImmediateContext->IASetIndexBuffer(
			this->buffer_, DXGI_FORMAT_R32_UINT, 0);
	}

} }