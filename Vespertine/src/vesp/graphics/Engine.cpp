#include "vesp/graphics/Engine.hpp"
#include "vesp/graphics/Window.hpp"
#include "vesp/graphics/Shader.hpp"
#include "vesp/graphics/Buffer.hpp"

#include "vesp/math/Vector.hpp"
#include "vesp/Log.hpp"
#include "vesp/Util.hpp"

#include <d3d11.h>

namespace vesp { namespace graphics {

	VertexShader* vertexShader = nullptr;
	PixelShader* pixelShader = nullptr;
	VertexBuffer* buffer = nullptr;

	IDXGISwapChain* Engine::SwapChain;
	ID3D11Device* Engine::Device;
	ID3D11DeviceContext* Engine::ImmediateContext;
	ID3D11RenderTargetView* Engine::RenderTargetView;

	Engine::Engine(StringPtr title)
	{
		this->window_ = std::make_unique<Window>(title, IVec2(1280, 800));
	}

	Engine::~Engine()
	{
		delete buffer;
		delete pixelShader;
		delete vertexShader;

		RenderTargetView->Release();
		ImmediateContext->Release();
		SwapChain->Release();
		Device->Release();
	}
	
	void Engine::Initialize()
	{
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
			0, nullptr, 0, D3D11_SDK_VERSION, &desc, &SwapChain,
			&Device, nullptr, &ImmediateContext);

		ID3D11Texture2D* backBuffer;
		// error handling
		SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
		Device->CreateRenderTargetView(backBuffer, nullptr, &RenderTargetView);
		backBuffer->Release();
		ImmediateContext->OMSetRenderTargets(1, &RenderTargetView, nullptr);
	
		D3D11_VIEWPORT vp;
		vp.Width = (float)size.x;
		vp.Height = (float)size.y;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		ImmediateContext->RSSetViewports(1, &vp);

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT numElements = ARRAYSIZE( layout );

		vertexShader = new VertexShader("vs");
		vertexShader->Load(
			"float4 main( float4 Pos : POSITION ) : SV_POSITION"
			"{"
			"	return Pos;"
			"}",
			layout, numElements);

		pixelShader = new PixelShader("ps");
		pixelShader->Load(
			"float4 main( float4 Pos : SV_POSITION ) : SV_Target"
			"{"
			"	return float4( 1.0f, 1.0f, 0.0f, 1.0f );"
			"}"
		);

		Vertex vertices[] =
		{
			Vec3( 0.0f, 0.5f, 0.5f ),
			Vec3( 0.5f, -0.5f, 0.5f ),
			Vec3( -0.5f, -0.5f, 0.5f ),
		};

		buffer = new VertexBuffer();
		buffer->Create(vertices, util::SizeOfArray(vertices));

		// Set primitive topology
		ImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	}

	void Engine::Pulse()
	{
		this->window_->Pulse();

		float clearColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		ImmediateContext->ClearRenderTargetView(RenderTargetView, clearColour);
		
		vertexShader->Activate();
		pixelShader->Activate();
		buffer->Use(0);
		ImmediateContext->Draw(3, 0);
		
		SwapChain->Present(0, 0);
	}

} }