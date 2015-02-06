#pragma once

#include "vesp/Types.hpp"
#include "vesp/Log.hpp"

#include "vesp/graphics/Vertex.hpp"
#include "vesp/graphics/Engine.hpp"

#include <atlbase.h>
#include <d3d11.h>

namespace vesp { namespace graphics {

	template <typename T>
	class Buffer
	{
	public:
		bool Create(T* data, U32 count, U32 bindFlags, D3D11_USAGE usage = D3D11_USAGE_DEFAULT)
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Usage = usage;
			desc.ByteWidth = sizeof(T) * count;
			desc.BindFlags = bindFlags;
			desc.CPUAccessFlags = 
				bindFlags == D3D11_BIND_CONSTANT_BUFFER ? D3D11_CPU_ACCESS_WRITE : 0;

			D3D11_SUBRESOURCE_DATA initData;
			ZeroMemory(&initData, sizeof(initData));
			initData.pSysMem = data;
			
			HRESULT hr = Engine::Device->CreateBuffer(&desc, &initData, &this->buffer_);
			if (FAILED(hr))
			{
				LogError( 
					"Failed to create buffer (count: %d, flags: %d, usage: %d, error: %X)", 
					count, bindFlags, usage, hr);

				return false;
			}

			this->count_ = count;

			return true;
		}

		ID3D11Buffer* Get()
		{
			return this->buffer_;
		}

		U32 GetCount()
		{
			return this->count_;
		}

	protected:
		CComPtr<ID3D11Buffer> buffer_;
		U32 count_;
	};

	class VertexBuffer : public Buffer<Vertex>
	{
	public:
		bool Create(Vertex* data, U32 count, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);

		void Use(U32 slot);
	};

	template <typename T>
	class ConstantBuffer : public Buffer<T>
	{
	public:
		bool Create(T* data, U32 count)
		{
			return Buffer<T>::Create(data, count, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC);
		}

		void UseVS(U32 slot)
		{
			Engine::ImmediateContext->VSSetConstantBuffers(slot, 1, &buffer_.p);
		}

		void UsePS(U32 slot)
		{
			Engine::ImmediateContext->PSSetConstantBuffers(slot, 1, &buffer_.p);
		}

		void* Map()
		{
			D3D11_MAPPED_SUBRESOURCE mappedSubresource;
			Engine::ImmediateContext->Map(
				this->buffer_, 0, D3D11_MAP_WRITE_DISCARD, 
				0, &mappedSubresource);

			return mappedSubresource.pData;
		}

		void Unmap()
		{
			Engine::ImmediateContext->Unmap(this->buffer_, 0);
		}

		void Load(T* data, U32 count)
		{
			auto p = this->Map();
			memcpy(p, data, sizeof(T) * count);
			this->Unmap();
		}
	};

} }