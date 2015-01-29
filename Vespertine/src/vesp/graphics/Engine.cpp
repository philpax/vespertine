#pragma warning(disable: 4005)
#include "vesp/graphics/Engine.hpp"
#include "vesp/graphics/Window.hpp"
#include "vesp/graphics/Shader.hpp"
#include "vesp/graphics/Buffer.hpp"

#include "vesp/math/Vector.hpp"
#include "vesp/math/Matrix.hpp"

#include "vesp/Log.hpp"

#include <d3d11.h>

namespace vesp { namespace graphics {

	struct VertexConstant
	{
		Mat4 model;
		Mat4 view;
		Mat4 projection;
	} vertexConstantData;

	VertexShader vertexShader("vs");
	PixelShader pixelShader("ps");
	VertexBuffer vertexBuffer;
	ConstantBuffer<VertexConstant> constantBuffer;

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
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT numElements = ARRAYSIZE( layout );

		Vector<StringByte> shaderSource;
	
		Filesystem::Get()->Read("data/shaders/default.vsh", shaderSource);
		vertexShader.Load(shaderSource.data(), layout, numElements);

		Filesystem::Get()->Read("data/shaders/default.psh", shaderSource);
		pixelShader.Load(shaderSource.data());

		Vertex vertices[] =
		{
			{Vec3(0.0f, 0.5f, 0.5f), Vec3(1.0f, 0.0f, 0.0f)},
			{Vec3(0.5f, -0.5f, 0.5f), Vec3(0.0f, 1.0f, 0.0f)},
			{Vec3(-0.5f, -0.5f, 0.5f), Vec3(0.0f, 0.0f, 1.0f)},
		};

		vertexBuffer.Create(vertices, util::SizeOfArray(vertices));

		vertexConstantData.model = glm::mat4();
		vertexConstantData.view = glm::mat4();
		vertexConstantData.projection = math::DXPerspective(
			60.0f, this->window_->GetAspectRatio(), 0.1f, 1000.0f);

		constantBuffer.Create(&vertexConstantData, 1);

		// Set primitive topology
		ImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	}

	void Engine::Pulse()
	{
		this->window_->Pulse();

		float clearColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		ImmediateContext->ClearRenderTargetView(RenderTargetView, clearColour);
		
		vertexConstantData.view = glm::mat4();
		vertexConstantData.view = glm::translate(vertexConstantData.view, 
			glm::vec3(0.0f, 0.0f, sin(this->timer_.GetSeconds()) + 0.75f));

		constantBuffer.Load(&vertexConstantData, 1);
		constantBuffer.UseVS(0);

		vertexShader.Activate();
		pixelShader.Activate();
		vertexBuffer.Use(0);
		ImmediateContext->Draw(3, 0);
		
		SwapChain->Present(0, 0);
	}

} }