#pragma warning(disable: 4005)
#include "vesp/graphics/Engine.hpp"
#include "vesp/graphics/Window.hpp"
#include "vesp/graphics/Shader.hpp"
#include "vesp/graphics/Buffer.hpp"
#include "vesp/graphics/FreeCamera.hpp"
#include "vesp/graphics/Mesh.hpp"
#include "vesp/graphics/ImGui.hpp"
#include "vesp/graphics/ImGuiDX11.hpp"

#include "vesp/math/Vector.hpp"
#include "vesp/math/Matrix.hpp"
#include "vesp/math/Util.hpp"

#include "vesp/Log.hpp"
#include "vesp/Assert.hpp"
#include "vesp/Console.hpp"

#include <glm/gtc/noise.hpp>

#include <deque>
#include <d3d11.h>

namespace vesp { namespace graphics {

	VertexShader vertexShader("vs");
	PixelShader pixelShader("ps");
	PixelShader gridPixelShader("psGrid");

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
	}

	Engine::~Engine()
	{
		ImGui_ImplDX11_Shutdown();

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

		ImGui_ImplDX11_Init(
			this->window_->GetSystemRepresentation(), Device, ImmediateContext);

		this->camera_ = std::make_unique<FreeCamera>(
			Vec3(0.0f, 2.0f, -4.0f), 
			Quat(Vec3(0.0f, 0.0f, 0.0f))
		);
	}

	void Engine::HandleResize(IVec2 size)
	{
		ImGui_ImplDX11_InvalidateDeviceObjects();

		this->DestroyRenderTargets();
		this->DestroyDepthStencil();

		SwapChain->ResizeBuffers(0, size.x, size.y, DXGI_FORMAT_UNKNOWN, 0);

		this->CreateDepthStencil(size);
		this->CreateRenderTargets(size);

		ImGui_ImplDX11_CreateDeviceObjects();

		LogInfo("Resized to (%d, %d)", size.x, size.y);
	}

	void Engine::Pulse()
	{
		this->window_->Pulse();
		ImGui_ImplDX11_NewFrame();

		F32 clearColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		for (auto& rt : this->renderTargetViews_)
			ImmediateContext->ClearRenderTargetView(rt, clearColour);

		ImmediateContext->ClearDepthStencilView(
			this->depthStencilView_, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11ShaderResourceView* views[2] = {};
		ImmediateContext->PSSetSamplers(0, 1, &this->samplerState_.p);
		ImmediateContext->PSSetShaderResources(0, 2, views);
		ImmediateContext->RSSetState(this->rasterizerState_);

		this->SetBlendingEnabled(false);

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
		for (auto& mesh : this->meshes_)
			mesh.Draw();

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

		// Render stats
		ImGui::Begin("Stats", nullptr, ImVec2(), 0.5f, ImGuiWindowFlags_NoTitleBar);
		{
			ImGui::SetWindowPos(ImVec2(0, 0));

			auto frameRate = ImGui::GetIO().Framerate;
			auto frameTime = 1000.0f / frameRate;

			if (this->fpsTimer_.GetSeconds() >= 0.2f || this->fpsRecord_.empty())
			{
				this->fpsRecord_.push_back(frameRate);
				this->fpsTimer_.Restart();
			}

			if (this->fpsRecord_.size() > 50)
				this->fpsRecord_.pop_front();

			ImGui::PushItemWidth(-1.0f);
			ImGui::PlotLines("FPS",
				[](void* data, int index)
				{
					return (*reinterpret_cast<std::deque<float>*>(data))[index];
				},
				(void*)&this->fpsRecord_, this->fpsRecord_.size());
			ImGui::PopItemWidth();

			ImGui::Separator();

			ImGui::Text("Frametime: %.02f ms", frameTime);
			ImGui::Text("Framerate: %.01f FPS", frameRate);

			auto aspectRatio = this->window_->GetAspectRatio();
			auto guiWidth = ImGui::GetWindowWidth();
			auto rtSize = ImVec2(guiWidth, guiWidth / aspectRatio);

			ImGui::Separator();

			auto drawRenderTarget = [&](char const* name, size_t index)
			{
				ImGui::BeginGroup();
				ImGui::Text(name);
				ImGui::Image(this->renderTargetResourceViews_[index].p, rtSize);
				ImGui::EndGroup();
			};

			drawRenderTarget("Diffuse", 0);
			drawRenderTarget("View-space normals", 1);
		}
		ImGui::End();

		Console::Get()->Draw();

		ImGui::Render();
		
		this->SetDepthEnabled(true);
		SwapChain->Present(0, 0);
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

		D3D11_RASTERIZER_DESC rasterizerDesc;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.DepthClipEnable = TRUE;
		rasterizerDesc.ScissorEnable = FALSE;
		rasterizerDesc.MultisampleEnable = FALSE;
		rasterizerDesc.AntialiasedLineEnable = FALSE;

		hr = Device->CreateRasterizerState(&rasterizerDesc, &this->rasterizerState_);
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
		renderTargetBlendDesc.RenderTargetWriteMask	 = D3D11_COLOR_WRITE_ENABLE_ALL;

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

		// Load floor mesh
		Vector<Vertex> floorVertices;
		auto file = FileSystem::Get()->Open("data/FloorMesh.vspm", "rb");
		floorVertices.resize(file.Size() / sizeof(Vertex));
		file.Read(ArrayView<Vertex>(floorVertices));

		Mesh floorMesh;
		floorMesh.Create(floorVertices);
		this->meshes_.push_back(floorMesh);

		// Add commands to load and modify meshes
		Console::Get()->AddCommand("mesh-load",
			[&](ArrayView<String> args)
			{
				if (args.size < 2)
				{
					LogError("mesh-load path topology");
					return;
				}

				auto path = Concat("data/", args[0]);
				auto file = FileSystem::Get()->Open(path, "rb");

				if (!file.Exists())
				{
					LogError("%.*s doesn't exist", path.size(), path.data());
					return;
				}

				Vector<Vertex> vertices;

				vertices.resize(file.Size() / sizeof(Vertex));
				file.Read(ArrayView<Vertex>(vertices));

				D3D_PRIMITIVE_TOPOLOGY topology;
				auto& topologyString = args[1];

				if (topologyString == "triangle_list")
					topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
				else if (topologyString == "triangle_strip")
					topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
				else if (topologyString == "line_list")
					topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
				else if (topologyString == "line_strip")
					topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
				else if (topologyString == "point_list")
					topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
				else
				{
					LogError("%.*s is an invalid topology", 
						topologyString.size(), topologyString.data());
					return;
				}

				Mesh mesh;
				mesh.Create(vertices, topology);
				this->meshes_.push_back(mesh);

				LogInfo("Successfully loaded %.*s (index: %d)", 
					path.size(), path.data(), this->meshes_.size() - 1);
			}
		);

		Console::Get()->AddCommand("mesh-set-position",
			[&](ArrayView<String> args)
			{
				if (args.size < 4)
				{
					LogError("mesh-set-position mesh_index x y z");
					return;
				}

				auto meshIndexString = ToCString(args[0]);
				auto xString = ToCString(args[1]);
				auto yString = ToCString(args[2]);
				auto zString = ToCString(args[3]);

				auto meshIndex = std::atoi(meshIndexString.get());
				if (meshIndex < 0 || (size_t)meshIndex >= this->meshes_.size())
				{
					LogError("Invalid mesh index: %d", meshIndex);
					return;
				}

				auto x = std::atof(xString.get());
				auto y = std::atof(yString.get());
				auto z = std::atof(zString.get());

				this->meshes_[meshIndex].SetPosition(Vec3(x, y, z));

				LogInfo("Successfully moved mesh %d to (%f, %f, %f)", 
					meshIndex, x, y, z);
			}
		);

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
		for (auto& rtView : this->renderTargetResourceViews_)
			rtView.Release();

		for (auto& rt : this->renderTargetViews_)
			rt.Release();
	}

} }