#pragma warning(disable: 4005)
#include "vesp/graphics/Engine.hpp"
#include "vesp/graphics/Window.hpp"
#include "vesp/graphics/Shader.hpp"
#include "vesp/graphics/Buffer.hpp"
#include "vesp/graphics/FreeCamera.hpp"
#include "vesp/graphics/Mesh.hpp"

#include "vesp/math/Vector.hpp"
#include "vesp/math/Matrix.hpp"
#include "vesp/math/Util.hpp"

#include "vesp/Log.hpp"

#include <glm/gtc/noise.hpp>

#include <d3d11.h>

namespace vesp { namespace graphics {

	VertexShader vertexShader("vs");
	PixelShader pixelShader("ps");
	PixelShader gridPixelShader("psGrid");
	Mesh floorMesh;
	Mesh gizmoMesh;

	IDXGISwapChain* Engine::SwapChain;
	ID3D11Device* Engine::Device;
	ID3D11DeviceContext* Engine::ImmediateContext;
	ID3D11RenderTargetView* Engine::RenderTargetView;

	Engine::Engine(StringPtr title)
	{
		this->window_ = std::make_unique<Window>(title, IVec2(1280, 800));
		this->frameCount_ = 0;
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
		this->CreateDevice();
		this->CreateDepthStencil();
		this->CreateRenderTargets();
		this->CreateBlendState();
		this->CreateTestData();		

		this->camera_ = std::make_unique<FreeCamera>(
			Vec3(0.0f, 2.0f, -4.0f), 
			Quat(Vec3(0.0f, 0.0f, 0.0f))
		);
	}

	void Engine::Pulse()
	{
		this->window_->Pulse();

		float clearColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		ImmediateContext->ClearRenderTargetView(RenderTargetView, clearColour);
		ImmediateContext->ClearDepthStencilView(
			this->depthStencilView_, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

		auto freeCamera = static_cast<FreeCamera*>(this->camera_.get());
		freeCamera->Update();

		vertexShader.Activate();
		gridPixelShader.Activate();
		floorMesh.Draw();

		// Draw rotating gizmo
		pixelShader.Activate();
		auto seconds = this->timer_.GetSeconds();
		gizmoMesh.SetPositionAngle(Vec3(1,1,1), Quat(Vec3(0, seconds, 0)));
		gizmoMesh.Draw();

		// Draw stationary gizmo
		gizmoMesh.SetPositionAngle(Vec3(0,1,0), Quat());
		gizmoMesh.Draw();
		
		SwapChain->Present(0, 0);

		this->frameCount_++;
		if (this->fpsTimer_.GetSeconds() > 5)
		{
			LogInfo("FPS: %.01f", this->frameCount_ / this->fpsTimer_.GetSeconds());
			this->fpsTimer_.Restart();
			this->frameCount_ = 0;
		}
	}

	Window* Engine::GetWindow()
	{
		return this->window_.get();
	}

	void Engine::SetBlendingEnabled(bool state)
	{
		ImmediateContext->OMSetBlendState(
			state ? this->blendState_ : nullptr, nullptr, 0xFFFFFFFF);
	}

	void Engine::CreateDevice()
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
	}

	void Engine::CreateDepthStencil()
	{
		auto size = this->window_->GetSize();

		// Create depth stencil state
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = false;

		Device->CreateDepthStencilState(&depthStencilDesc, &this->depthStencilState_);
		ImmediateContext->OMSetDepthStencilState(this->depthStencilState_, 1);

		// Create depth texture
		ID3D11Texture2D* depthTexture = nullptr;
		D3D11_TEXTURE2D_DESC depthTextureDesc;
		depthTextureDesc.Width = size.x;
		depthTextureDesc.Height = size.y;
		depthTextureDesc.MipLevels = 1;
		depthTextureDesc.ArraySize = 1;
		depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthTextureDesc.SampleDesc.Count = 1;
		depthTextureDesc.SampleDesc.Quality = 0;
		depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthTextureDesc.CPUAccessFlags = 0;
		depthTextureDesc.MiscFlags = 0;
		Device->CreateTexture2D( &depthTextureDesc, NULL, &depthTexture );

		// Create depth stencil view
		Device->CreateDepthStencilView(depthTexture, nullptr, &this->depthStencilView_);
		depthTexture->Release();
	}

	void Engine::CreateRenderTargets()
	{
		auto size = this->window_->GetSize();

		// Set render targets + depth stencil
		ID3D11Texture2D* backBuffer;
		SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
		Device->CreateRenderTargetView(backBuffer, nullptr, &RenderTargetView);
		backBuffer->Release();
		ImmediateContext->OMSetRenderTargets(1, &RenderTargetView, this->depthStencilView_);
	
		D3D11_VIEWPORT vp;
		vp.Width = (float)size.x;
		vp.Height = (float)size.y;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		ImmediateContext->RSSetViewports(1, &vp);
	}

	void Engine::CreateBlendState()
	{
		// Set up blend state
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory( &blendDesc, sizeof(blendDesc) );

		D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc;
		ZeroMemory( &renderTargetBlendDesc, sizeof(renderTargetBlendDesc) );

		renderTargetBlendDesc.BlendEnable			 = true;
		renderTargetBlendDesc.SrcBlend				 = D3D11_BLEND_SRC_COLOR;
		renderTargetBlendDesc.DestBlend				 = D3D11_BLEND_BLEND_FACTOR;
		renderTargetBlendDesc.BlendOp				 = D3D11_BLEND_OP_ADD;
		renderTargetBlendDesc.SrcBlendAlpha			 = D3D11_BLEND_ONE;
		renderTargetBlendDesc.DestBlendAlpha		 = D3D11_BLEND_ZERO;
		renderTargetBlendDesc.BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
		renderTargetBlendDesc.RenderTargetWriteMask	 = D3D10_COLOR_WRITE_ENABLE_ALL;

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.RenderTarget[0] = renderTargetBlendDesc;

		Device->CreateBlendState(&blendDesc, &this->blendState_);
	}

	void Engine::CreateTestData()
	{
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		Vector<StringByte> shaderSource;
	
		FileSystem::Get()->Read("data/shaders/default.vsh", shaderSource);
		vertexShader.Load(shaderSource.data(), layout, util::SizeOfArray(layout));

		FileSystem::Get()->Read("data/shaders/default.psh", shaderSource);
		pixelShader.Load(shaderSource.data());

		FileSystem::Get()->Read("data/shaders/grid.psh", shaderSource);
		gridPixelShader.Load(shaderSource.data());

		Vector<Vertex> floorVertices;
		for (int y = -10; y < 10; y++)
		{
			for (int x = -10; x < 10; x++)
			{
				auto p = Vec3(x - 0.5f, 0.0f, y - 0.5f);

				auto p1 = p + Vec3(0.5f, 0.0f, -0.5f);
				auto p2 = p + Vec3(-0.5f, 0.0f, -0.5f);
				auto p3 = p + Vec3(-0.5f, 0.0f, 0.5f);
				auto p4 = p + Vec3(0.5f, 0.0f, 0.5f);

				auto push_back = [&](Vec3 pf)
				{
					pf.y = glm::simplex(glm::vec2(pf.x, pf.z) / 8.0f);
					pf.y += glm::simplex(glm::vec2(pf.x, pf.z) / 3.75f) * 0.6f;
					auto col = Vec3((pf.x + 10) / 20.0f, 1.0f, (pf.z + 10) / 20.0f);
					col *= math::Clamp(pf.y + 1.0f, 0.0f, 1.0f);
					floorVertices.push_back({pf, col});
				};

				push_back(p1);
				push_back(p2);
				push_back(p3);

				push_back(p4);
				push_back(p1);
				push_back(p3);
			}
		}
		
		floorMesh.Create(floorVertices.data(), floorVertices.size());

		Vertex gizmoVertices[] =
		{
			{Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f)},
			{Vec3(1.0f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f)},

			{Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f)},
			{Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f)},

			{Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f)},
			{Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f)},
		};

		gizmoMesh.Create(gizmoVertices, util::SizeOfArray(gizmoVertices), 
			D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	}

} }