#include "vesp/graphics/Engine.hpp"
#include "vesp/graphics/Window.hpp"
#include "vesp/graphics/Shader.hpp"
#include "vesp/graphics/Vertex.hpp"
#include "vesp/math/Vector.hpp"
#include "vesp/Log.hpp"

#include <d3d11.h>

namespace vesp { namespace graphics {

	VertexShader* vertexShader = nullptr;
	PixelShader* pixelShader = nullptr;
	ID3D11Buffer* buffer = nullptr;

	Engine::Engine(StringPtr title)
	{
		this->window_ = std::make_unique<Window>(title, IVec2(1280, 800));
	}

	Engine::~Engine()
	{
		buffer->Release();

		delete pixelShader;
		delete vertexShader;
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
			0, nullptr, 0, D3D11_SDK_VERSION, &desc, &this->swapChain_,
			&this->device_, nullptr, &this->immediateContext_);

		ID3D11Texture2D* backBuffer;
		// error handling
		this->swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
		this->device_->CreateRenderTargetView(backBuffer, nullptr, &this->renderTargetView_);
		backBuffer->Release();
		this->immediateContext_->OMSetRenderTargets(1, &this->renderTargetView_.p, nullptr);
	
		D3D11_VIEWPORT vp;
		vp.Width = (float)size.x;
		vp.Height = (float)size.y;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		this->immediateContext_->RSSetViewports(1, &vp);

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

		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( Vertex ) * 3;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory( &InitData, sizeof(InitData) );
		InitData.pSysMem = vertices;
		HRESULT hr = device_->CreateBuffer( &bd, &InitData, &buffer );
		if( FAILED( hr ) )
			return;

		// Set vertex buffer
		UINT stride = sizeof( Vertex );
		UINT offset = 0;
		immediateContext_->IASetVertexBuffers( 0, 1, &buffer, &stride, &offset );

		// Set primitive topology
		immediateContext_->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	}

	void Engine::Pulse()
	{
		this->window_->Pulse();

		float clearColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		this->immediateContext_->ClearRenderTargetView(renderTargetView_, clearColour);
		
		vertexShader->Activate();
		pixelShader->Activate();
		this->immediateContext_->Draw(3, 0);
		
		this->swapChain_->Present(0, 0);
	}

	ID3D11Device* Engine::GetDevice()
	{
		return this->device_;
	}

	ID3D11DeviceContext* Engine::GetImmediateContext()
	{
		return this->immediateContext_;
	}

} }