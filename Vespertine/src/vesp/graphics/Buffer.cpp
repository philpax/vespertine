#pragma warning(disable: 4005)
#include "vesp/graphics/Buffer.hpp"

namespace vesp { namespace graphics {

	bool VertexBuffer::Create(Vertex* data, U32 count, D3D11_USAGE usage)
	{
		return Buffer<Vertex>::Create(data, count, D3D11_BIND_VERTEX_BUFFER, usage);
	}

	void VertexBuffer::Use(U32 slot)
	{
		U32 stride = sizeof(Vertex);
		U32 offset = 0;
		Engine::ImmediateContext->IASetVertexBuffers(
			slot, 1, &this->buffer_.p, &stride, &offset);
	}

} }