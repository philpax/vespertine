#pragma once

#include "vesp/Types.hpp"
#include "vesp/Containers.hpp"
#include "vesp/Log.hpp"
#include "vesp/Assert.hpp"

#include "vesp/graphics/Vertex.hpp"
#include "vesp/graphics/Engine.hpp"

#pragma warning(push)
#pragma warning(disable: 4005)
#include <atlbase.h>
#include <d3d11.h>
#pragma warning(pop)

namespace vesp { namespace graphics {

	template <typename T>
	class Buffer
	{
	public:
		bool Create(ArrayView<T> const array, U32 bindFlags, D3D11_USAGE usage = D3D11_USAGE_DEFAULT)
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Usage = usage;
			desc.ByteWidth = sizeof(T) * array.size();
			desc.BindFlags = bindFlags;
			desc.CPUAccessFlags = 
				bindFlags == D3D11_BIND_CONSTANT_BUFFER ? D3D11_CPU_ACCESS_WRITE : 0;

			D3D11_SUBRESOURCE_DATA initData;
			ZeroMemory(&initData, sizeof(initData));
			initData.pSysMem = array.data();
			
			HRESULT hr = Engine::Device->CreateBuffer(&desc, &initData, &this->buffer_);
			if (FAILED(hr))
			{
				LogError( 
					"Failed to create buffer (count: %d, flags: %d, usage: %d, error: %X)", 
					array.size(), bindFlags, usage, hr);

				return false;
			}

			this->count_ = array.size();

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

		bool Initialized()
		{
			return GetCount() > 0;
		}

	protected:
		CComPtr<ID3D11Buffer> buffer_;
		U32 count_ = 0;
	};

	class VertexBuffer : public Buffer<Vertex>
	{
	public:
		bool Create(ArrayView<Vertex> const array, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);

		void Use(U32 slot);
	};

	class IndexBuffer : public Buffer<U32>
	{
	public:
		bool Create(ArrayView<U32> const array, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);

		void Use();
	};

	template <typename T>
	class ConstantBuffer : public Buffer<T>
	{
	public:
		bool Create(ArrayView<T> array)
		{
			return Buffer<T>::Create(array, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC);
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
			VESP_ASSERT(this->buffer_);

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

		void Load(ArrayView<T> const array)
		{
			auto p = this->Map();
			memcpy(p, array.data(), sizeof(T) * array.size());
			this->Unmap();
		}
	};

} }