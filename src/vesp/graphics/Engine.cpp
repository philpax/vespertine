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
#include "vesp/Assert.hpp"

#include <glm/gtc/noise.hpp>

#include <d3d11.h>

namespace vesp { namespace graphics {

	VertexShader vertexShader("vs");
	PixelShader pixelShader("ps");
	PixelShader gridPixelShader("psGrid");
	Mesh floorMesh;
	Mesh gizmoMesh;

	VertexShader identityVertexShader("vsIdentity");
	PixelShader compositePixelShader("psComposite");
	PixelShader texturePixelShader("psTexture");
	Mesh screenMesh;	

	IDXGISwapChain* Engine::SwapChain;
	ID3D11Device* Engine::Device;
	ID3D11DeviceContext* Engine::ImmediateContext;

	Engine::Engine(RawStringPtr title)
	{
		this->window_ = std::make_unique<Window>(title, IVec2(1280, 800));
		this->frameCount_ = 0;
	}

	Engine::~Engine()
	{
		this->DestroyRenderTargets();
		this->DestroyDepthStencil();

		ImmediateContext->Release();
		SwapChain->Release();
		Device->Release();
	}
	
	void Engine::Initialize()
	{
		auto size = this->window_->GetSize();

		this->CreateDevice(size);
		this->CreateDepthStencil(size);
		this->CreateRenderTargets(size);
		this->CreateBlendState();
		this->CreateSamplerState();
		this->CreateTestData();		

		this->camera_ = std::make_unique<FreeCamera>(
			Vec3(0.0f, 2.0f, -4.0f), 
			Quat(Vec3(0.0f, 0.0f, 0.0f))
		);
	}

	void Engine::HandleResize(IVec2 size)
	{
		this->DestroyRenderTargets();
		this->DestroyDepthStencil();

		SwapChain->ResizeBuffers(0, size.x, size.y, DXGI_FORMAT_UNKNOWN, 0);

		this->CreateDepthStencil(size);
		this->CreateRenderTargets(size);

		LogInfo("Resized to (%d, %d)", size.x, size.y);
	}

	void Engine::Pulse()
	{
		this->window_->Pulse();

		F32 clearColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		for (auto& rt : this->renderTargetViews_)
			ImmediateContext->ClearRenderTargetView(rt, clearColour);

		ImmediateContext->ClearDepthStencilView(
			this->depthStencilView_, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11ShaderResourceView* views[2] = {};
		ImmediateContext->PSSetSamplers(0, 1, &this->samplerState_.p);
		ImmediateContext->PSSetShaderResources(0, 2, views);

		// Activate g-buffer render targets
		ImmediateContext->OMSetRenderTargets(
			this->renderTargetViews_.size() - 1, 
			reinterpret_cast<ID3D11RenderTargetView**>(
				&this->renderTargetViews_[1].p), 
			this->depthStencilView_);

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

		// Activate backbuffer
		ImmediateContext->OMSetRenderTargets(
			1, &this->renderTargetViews_[0].p, this->depthStencilView_);
		
		ImmediateContext->PSSetShaderResources(0, this->renderTargetResourceViews_.size(), 
			reinterpret_cast<ID3D11ShaderResourceView**>(this->renderTargetResourceViews_.data()));

		// Draw composite view to backbuffer
		this->SetDepthEnabled(false);
		identityVertexShader.Activate();
		compositePixelShader.Activate();
		screenMesh.SetScale(1.0f);
		screenMesh.SetPosition(Vec3(0.0f, 0.0f, 0.0f));
		screenMesh.Draw();

		// Draw the source views on top
		texturePixelShader.Activate();
		for (U32 i = 0; i < this->renderTargetResourceViews_.size(); i++)
		{
			ImmediateContext->PSSetShaderResources(0, 1, &this->renderTargetResourceViews_[i].p);
			screenMesh.SetScale(0.25f);
			screenMesh.SetPosition(Vec3(-3.0f + i * 2.0f, 3.0f, 0.0f));
			screenMesh.Draw();
		}
		this->SetDepthEnabled(true);
		
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

	Camera* Engine::GetCamera()
	{
		return this->camera_.get();
	}

	void Engine::SetBlendingEnabled(bool state)
	{
		ImmediateContext->OMSetBlendState(
			state ? this->blendState_ : nullptr, nullptr, 0xFFFFFFFF);
	}

	void Engine::SetDepthEnabled(bool state)
	{
		ImmediateContext->OMSetDepthStencilState(
			state ? this->enabledDepthStencilState_ : this->disabledDepthStencilState_, 1);
	}

	void Engine::CreateDevice(IVec2 size)
	{
		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.BufferCount = 1;
		desc.BufferDesc.Width = size.x;
		desc.BufferDesc.Height = size.y;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.OutputWindow = 
			reinterpret_cast<HWND>(this->window_->GetSystemRepresentation());
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Windowed = !this->window_->IsFullscreen();

		auto hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
			0, nullptr, 0, D3D11_SDK_VERSION, &desc, &SwapChain,
			&Device, nullptr, &ImmediateContext);
		VESP_ENFORCE(SUCCEEDED(hr));
	}

	void Engine::CreateDepthStencil(IVec2 size)
	{
		// Create enabled depth stencil state
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = false;

		auto hr = Device->CreateDepthStencilState(&depthStencilDesc, &this->enabledDepthStencilState_);
		VESP_ENFORCE(SUCCEEDED(hr));

		// Create disabled depth stencil state
		depthStencilDesc.DepthEnable = false;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

		hr = Device->CreateDepthStencilState(&depthStencilDesc, &this->disabledDepthStencilState_);
		VESP_ENFORCE(SUCCEEDED(hr));

		this->SetDepthEnabled(true);

		// Create depth texture
		CComPtr<ID3D11Texture2D> depthTexture;
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
		hr = Device->CreateTexture2D(&depthTextureDesc, NULL, &depthTexture);
		VESP_ENFORCE(SUCCEEDED(hr));

		// Create depth stencil view
		hr = Device->CreateDepthStencilView(depthTexture, nullptr, &this->depthStencilView_);
		VESP_ENFORCE(SUCCEEDED(hr));
	}

	void Engine::CreateRenderTargets(IVec2 size)
	{
		// Create backbuffer render target
		CComPtr<ID3D11Texture2D> backBuffer;
		auto hr = SwapChain->GetBuffer(
			0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
		VESP_ENFORCE(SUCCEEDED(hr));

		hr = Device->CreateRenderTargetView(
			backBuffer, nullptr, &this->renderTargetViews_[0]);
		VESP_ENFORCE(SUCCEEDED(hr));

		// Create diffuse render target and shader resource view
		CComPtr<ID3D11Texture2D> diffuseTexture;
		D3D11_TEXTURE2D_DESC diffuseTextureDesc;
		diffuseTextureDesc.Width = size.x;
		diffuseTextureDesc.Height = size.y;
		diffuseTextureDesc.MipLevels = 1;
		diffuseTextureDesc.ArraySize = 1;
		diffuseTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		diffuseTextureDesc.SampleDesc.Count = 1;
		diffuseTextureDesc.SampleDesc.Quality = 0;
		diffuseTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		diffuseTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		diffuseTextureDesc.CPUAccessFlags = 0;
		diffuseTextureDesc.MiscFlags = 0;
		hr = Device->CreateTexture2D(&diffuseTextureDesc, NULL, &diffuseTexture);
		VESP_ENFORCE(SUCCEEDED(hr));

		hr = Device->CreateRenderTargetView(
			diffuseTexture, nullptr, &this->renderTargetViews_[1]);
		VESP_ENFORCE(SUCCEEDED(hr));

		D3D11_SHADER_RESOURCE_VIEW_DESC diffuseSRVDesc;
		diffuseSRVDesc.Format = diffuseTextureDesc.Format;
		diffuseSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		diffuseSRVDesc.Texture2D = {0, -1};

		hr = Device->CreateShaderResourceView(
			diffuseTexture, &diffuseSRVDesc, &this->renderTargetResourceViews_[0]);
		VESP_ENFORCE(SUCCEEDED(hr));

		// Create normals render target
		CComPtr<ID3D11Texture2D> normalTexture;
		D3D11_TEXTURE2D_DESC normalTextureDesc;
		normalTextureDesc.Width = size.x;
		normalTextureDesc.Height = size.y;
		normalTextureDesc.MipLevels = 1;
		normalTextureDesc.ArraySize = 1;
		normalTextureDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
		normalTextureDesc.SampleDesc.Count = 1;
		normalTextureDesc.SampleDesc.Quality = 0;
		normalTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		normalTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		normalTextureDesc.CPUAccessFlags = 0;
		normalTextureDesc.MiscFlags = 0;
		hr = Device->CreateTexture2D(&normalTextureDesc, NULL, &normalTexture);
		VESP_ENFORCE(SUCCEEDED(hr));

		hr = Device->CreateRenderTargetView(
			normalTexture, nullptr, &this->renderTargetViews_[2]);
		VESP_ENFORCE(SUCCEEDED(hr));

		D3D11_SHADER_RESOURCE_VIEW_DESC normalSRVDesc;
		normalSRVDesc.Format = normalTextureDesc.Format;
		normalSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		normalSRVDesc.Texture2D = {0, -1};

		hr = Device->CreateShaderResourceView(
			normalTexture, &normalSRVDesc, &this->renderTargetResourceViews_[1]);
		VESP_ENFORCE(SUCCEEDED(hr));
	
		D3D11_VIEWPORT vp;
		vp.Width = static_cast<F32>(size.x);
		vp.Height = static_cast<F32>(size.y);
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

		auto hr = Device->CreateBlendState(&blendDesc, &this->blendState_);
		VESP_ENFORCE(SUCCEEDED(hr));
	}

	void Engine::CreateSamplerState()
	{
		D3D11_SAMPLER_DESC samplerDesc;

		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MinLOD = -FLT_MAX;
		samplerDesc.MaxLOD = FLT_MAX;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		float borderColour[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		memcpy(samplerDesc.BorderColor, borderColour, 4*sizeof(float));

		auto hr = Device->CreateSamplerState(&samplerDesc, &this->samplerState_);
		VESP_ENFORCE(SUCCEEDED(hr));
	}

	void Engine::CreateTestData()
	{
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R16G16_UNORM, 0, 
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R16G16_UNORM, 0, 
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		String shaderSource;
	
		// g-buffer write
		FileSystem::Get()->Read("data/shaders/default.vsh", shaderSource);
		vertexShader.Load(shaderSource.data(), layout, util::SizeOfArray(layout));

		FileSystem::Get()->Read("data/shaders/default.psh", shaderSource);
		pixelShader.Load(shaderSource.data());

		FileSystem::Get()->Read("data/shaders/grid.psh", shaderSource);
		gridPixelShader.Load(shaderSource.data());		

		// deferred composite
		FileSystem::Get()->Read("data/shaders/identity.vsh", shaderSource);
		identityVertexShader.Load(shaderSource.data(), layout, util::SizeOfArray(layout));

		FileSystem::Get()->Read("data/shaders/composite.psh", shaderSource);
		compositePixelShader.Load(shaderSource.data());

		FileSystem::Get()->Read("data/shaders/texture.psh", shaderSource);
		texturePixelShader.Load(shaderSource.data());

		const int GridWidth = 21;
		const int HalfGridWidth = GridWidth / 2;
		F32 heightMap[GridWidth][GridWidth];
		Vertex floorVertices[GridWidth * GridWidth * 6];
		
		for (U32 yIndex = 0; yIndex < GridWidth; yIndex++)
		{
			for (U32 xIndex = 0; xIndex < GridWidth; xIndex++)
			{
				float x = xIndex + 0.5f - GridWidth/2.0f;
				float y = yIndex + 0.5f - GridWidth/2.0f;

				auto height = glm::simplex(glm::vec2(x, y) / 8.0f);
				height += glm::simplex(glm::vec2(x, y) / 3.75f) * 0.6f;

				heightMap[yIndex][xIndex] = height;
			}
		}

		auto index = 0u;		
		for (U32 yIndex = 0; yIndex < GridWidth; yIndex++)
		{
			for (U32 xIndex = 0; xIndex < GridWidth; xIndex++)
			{
				float x = xIndex + 0.5f - GridWidth/2.0f;
				float y = yIndex + 0.5f - GridWidth/2.0f;

				auto p = Vec3(x, 0.0f, y);

				auto p1 = p + Vec3(0.5f, 0.0f, -0.5f);
				auto p2 = p + Vec3(-0.5f, 0.0f, -0.5f);
				auto p3 = p + Vec3(-0.5f, 0.0f, 0.5f);
				auto p4 = p + Vec3(0.5f, 0.0f, 0.5f);

				auto PushBack = [&](Vec3 pf)
				{
					auto h = [&](float x, float z)
					{
						return heightMap
							[math::Clamp(static_cast<S32>(floor(z)), -HalfGridWidth, HalfGridWidth) + HalfGridWidth]
							[math::Clamp(static_cast<S32>(floor(x)), -HalfGridWidth, HalfGridWidth) + HalfGridWidth];
					};

					pf.y = h(pf.x, pf.z);

					float deltaX = h(pf.x + 1, pf.z) - h(pf.x - 1, pf.z);
					float deltaZ = h(pf.x, pf.z + 1) - h(pf.x, pf.z - 1);
					Vec3 normal = glm::normalize(Vec3(-deltaX, 2, -deltaZ));

					auto col = Vec4(
						(pf.x + HalfGridWidth) / static_cast<float>(GridWidth), 
						1.0f, 
						(pf.z + HalfGridWidth) / static_cast<float>(GridWidth),
						1.0f);

					Vertex vertex;
					vertex.position = pf;
					vertex.SetNormal(normal);
					vertex.colour = col;

					floorVertices[index] = vertex;
					index++;
				};

				PushBack(p1);
				PushBack(p2);
				PushBack(p3);

				PushBack(p4);
				PushBack(p1);
				PushBack(p3);
			}
		}

		floorMesh.Create(floorVertices);

		Vertex gizmoVertices[] =
		{
			{Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Colour::Red},
			{Vec3(1.0f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Colour::Red},

			{Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Colour::Green},
			{Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Colour::Green},

			{Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Colour::Blue},
			{Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Colour::Blue},
		};

		gizmoMesh.Create(gizmoVertices, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		Vertex screenVertices[] =
		{
			{Vec3(1, -1, 0), Vec3(0, 0, -1), Vec2(1, 1)},
			{Vec3(-1, -1, 0), Vec3(0, 0, -1), Vec2(0, 1)},
			{Vec3(-1, 1, 0), Vec3(0, 0, -1), Vec2(0, 0)},
			
			{Vec3(1, 1, 0), Vec3(0, 0, -1), Vec2(1, 0)},
			{Vec3(1, -1, 0), Vec3(0, 0, -1), Vec2(1, 1)},
			{Vec3(-1, 1, 0), Vec3(0, 0, -1), Vec2(0, 0)},
		};

		screenMesh.Create(screenVertices);
	}

	void Engine::DestroyDepthStencil()
	{
		ImmediateContext->OMSetDepthStencilState(nullptr, 0);
		this->enabledDepthStencilState_.Release();
		this->disabledDepthStencilState_.Release();
		this->depthStencilView_.Release();
	}

	void Engine::DestroyRenderTargets()
	{
		ImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);
		for (auto& rt : this->renderTargetViews_)
			rt.Release();
	}

} }