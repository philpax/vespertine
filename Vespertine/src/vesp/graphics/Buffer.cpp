#include "vesp/graphics/Buffer.hpp"
#include "vesp/graphics/Engine.hpp"
#include "vesp/Log.hpp"

namespace vesp { namespace graphics {

	template <typename T>
	bool Buffer<T>::Create(T* data, U32 count, U32 bindFlags, D3D11_USAGE usage)
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = usage;
		desc.ByteWidth = sizeof(T) * count;
		desc.BindFlags = bindFlags;
		desc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(initData));
		initData.pSysMem = data;

		HRESULT hr = Engine::Device->CreateBuffer(&desc, &initData, &this->buffer_);
		if (FAILED(hr))
		{
			Log(LogType::Error, 
				"Failed to create buffer (count: %d, flags: %d, usage: %d)", 
				count, bindFlags, usage);

			return false;
		}

		return true;
	}

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