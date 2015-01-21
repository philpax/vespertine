#include "vesp/graphics/Engine.hpp"
#include "vesp/graphics/Window.hpp"
#include "vesp/math/Vector.hpp"
#include "vesp/Log.hpp"

#include <d3d11.h>

namespace vesp { namespace graphics {

	Engine::Engine(StringPtr title)
	{
		this->window_ = std::make_unique<Window>(title, IVec2(1280, 800));

		auto size = this->window_->GetSize();

		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.BufferCount = 1;
		desc.BufferDesc.Width = size.x;
		desc.BufferDesc.Height = size.y;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.OutputWindow = (HWND)this->window_->GetSystemRepresentation();
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Windowed = !this->window_->IsFullscreen();

		// todo error handling
		D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
			0, nullptr, 0, D3D11_SDK_VERSION, &desc, &this->swapChain_,
			&this->device_, nullptr, &this->immediateContext_);

		ID3D11Texture2D* backBuffer;
		// error handling
		this->swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
		this->device_->CreateRenderTargetView(backBuffer, nullptr, &this->renderTargetView_);
		backBuffer->Release();
		this->immediateContext_->OMSetRenderTargets(1, &this->renderTargetView_, nullptr);
	
		D3D11_VIEWPORT vp;
		vp.Width = (float)size.x;
		vp.Height = (float)size.y;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		this->immediateContext_->RSSetViewports(1, &vp);
	}

	Engine::~Engine()
	{
	}

	void Engine::Pulse()
	{
		this->window_->Pulse();

		float clearColour[4] = { 0.75f, 0.0f, 1.0f, 1.0f };
		this->immediateContext_->ClearRenderTargetView(renderTargetView_, clearColour);
		this->swapChain_->Present(0, 0);
	}

} }