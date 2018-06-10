#pragma warning(disable: 4005)
#include "vesp/graphics/Engine.hpp"
#include "vesp/graphics/Window.hpp"
#include "vesp/graphics/Shader.hpp"
#include "vesp/graphics/Buffer.hpp"
#include "vesp/graphics/FreeCamera.hpp"
#include "vesp/graphics/Mesh.hpp"
#include "vesp/graphics/imgui.h"
#include "vesp/graphics/imgui_impl_dx11.h"
#include "vesp/graphics/ShaderManager.hpp"

#include "vesp/math/Vector.hpp"
#include "vesp/math/Matrix.hpp"
#include "vesp/math/Util.hpp"

#include "vesp/world/HeightMapTerrain.hpp"
#include "vesp/world/ScalarField.hpp"
#include "vesp/world/Script.hpp"

#include "vesp/Log.hpp"
#include "vesp/Assert.hpp"
#include "vesp/Console.hpp"
#include "vesp/EventManager.hpp"
#include "vesp/Profiler.hpp"

#include <glm/gtc/noise.hpp>

#include <deque>
#include <d3d11.h>

namespace vesp { namespace graphics {

	Mesh screenMesh;
	Mesh skyMesh;

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

		ShaderManager::Destroy();
		ImmediateContext->Release();
		SwapChain->Release();
		Device->Release();
	}

	// https://gist.github.com/dougbinks/8089b4bbaccaaf6fa204236978d165a9#file-imguiutils-h-L9-L93
	inline void SetupImGuiStyle( bool bStyleDark_, float alpha_  )
	{
		ImGuiStyle& style = ImGui::GetStyle();
		
		// light style from Pacôme Danhiez (user itamago) https://github.com/ocornut/imgui/pull/511#issuecomment-175719267
		style.Alpha = 1.0f;
		style.FrameRounding = 3.0f;
		style.Colors[ImGuiCol_Text]                  = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
		style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_PopupBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
		style.Colors[ImGuiCol_Border]                = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
		style.Colors[ImGuiCol_BorderShadow]          = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
		style.Colors[ImGuiCol_FrameBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
		style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
		style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
		style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
		style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
		style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
		style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_Button]                = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_Header]                = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
		style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_Column]                = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
		style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
		style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
		style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

		if( bStyleDark_ )
		{
			for (int i = 0; i <= ImGuiCol_COUNT; i++)
			{
				ImVec4& col = style.Colors[i];
				float H, S, V;
				ImGui::ColorConvertRGBtoHSV( col.x, col.y, col.z, H, S, V );

				if( S < 0.1f )
				{
				   V = 1.0f - V;
				}
				ImGui::ColorConvertHSVtoRGB( H, S, V, col.x, col.y, col.z );
				if( col.w < 1.00f )
				{
					col.w *= alpha_;
				}
			}
		}
		else
		{
			for (int i = 0; i <= ImGuiCol_COUNT; i++)
			{
				ImVec4& col = style.Colors[i];
				if( col.w < 1.00f )
				{
					col.x *= alpha_;
					col.y *= alpha_;
					col.z *= alpha_;
					col.w *= alpha_;
				}
			}
		}
	}
	
	void Engine::Initialize()
	{
		auto size = this->window_->GetSize();

		this->CreateDevice(size);
		ShaderManager::Create();

		this->CreateDepthStencil(size);
		this->CreateRenderTargets(size);
		this->CreateBlendState();
		this->CreateSamplerState();
		this->CreateTestData();

		ImGui_ImplDX11_Init(
			this->window_->GetSystemRepresentation(), Device, ImmediateContext);
		SetupImGuiStyle(true, 0.9f);

		this->camera_ = std::make_unique<FreeCamera>(
			Vec3(0.0f, 2.0f, -4.0f), 
			Quat(Vec3(0.0f, 0.0f, 0.0f))
		);
	}

	void Engine::HandleResize(IVec2 size)
	{
		if (size.x == 0 && size.y == 0)
			return;

		ImGui_ImplDX11_InvalidateDeviceObjects();

		this->DestroyRenderTargets();
		this->DestroyDepthStencil();

		SwapChain->ResizeBuffers(0, size.x, size.y, DXGI_FORMAT_UNKNOWN, 0);

		this->CreateDepthStencil(size);
		this->CreateRenderTargets(size);

		ImGui_ImplDX11_CreateDeviceObjects();

		LogInfo("Resized to (%d, %d)", size.x, size.y);
	}

	void Engine::PrePulse()
	{
		VESP_PROFILE_FN();
		ImGui_ImplDX11_NewFrame();
	}

	void Engine::Pulse()
	{
		VESP_PROFILE_FN();
		
		{
			VESP_PROFILE_BLOCK("Initial State Update"); 
			this->window_->Pulse();

			ID3D11ShaderResourceView* nullViews[
				D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { 0 };
			ImmediateContext->PSSetShaderResources(
				0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullViews);
		}

		{
			VESP_PROFILE_BLOCK("Clear RT");
			F32 clearColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

			for (auto& rt : this->renderTargetViews_)
				ImmediateContext->ClearRenderTargetView(rt, clearColour);
		
			ImmediateContext->ClearDepthStencilView(
				this->depthStencilView_, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
		
		{
			VESP_PROFILE_BLOCK("G-buffer production");
			{
				VESP_PROFILE_BLOCK("State updates");
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
			}

			auto freeCamera = static_cast<FreeCamera*>(this->camera_.get());
			freeCamera->Update();

			this->SetDepthEnabled(false);
			skyMesh.Draw();
			this->SetDepthEnabled(true);

			world::HeightMapTerrain::Get()->Draw();
			world::Script::Get()->Draw();

			{
				VESP_PROFILE_BLOCK("Mesh drawing");
				for (auto& mesh : this->meshes_)
					mesh.Draw();
			}
		}
		
		{
			VESP_PROFILE_BLOCK("Composite");
			// Activate backbuffer
			ImmediateContext->OMSetRenderTargets(
				1, &this->renderTargetViews_[0].p, nullptr);
			
			ImmediateContext->PSSetShaderResources(0, this->renderTargetResourceViews_.size(), 
				reinterpret_cast<ID3D11ShaderResourceView**>(this->renderTargetResourceViews_.data()));

			// Draw composite view to backbuffer
			this->SetDepthEnabled(false);
			screenMesh.SetScale(1.0f);
			screenMesh.SetPosition(Vec3(0.0f, 0.0f, 0.0f));
			screenMesh.Draw();
		}

		// Render stats
		ImGui::Begin("Stats", nullptr, ImVec2(), 0.5f, ImGuiWindowFlags_NoTitleBar);
		{
			VESP_PROFILE_BLOCK("Stats Window");
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

			ImGui::Separator();
			auto drawRenderTarget = [&](char const* name, size_t index)
			{
				ImGui::Text(name);
				ImGui::BeginGroup();
				auto guiWidth = ImGui::GetWindowWidth() / ImGui::GetColumnsCount();
				auto rtSize = ImVec2(guiWidth, guiWidth / aspectRatio);
				ImGui::Image(this->renderTargetResourceViews_[index].p, rtSize);
				ImGui::EndGroup();
				ImGui::NextColumn();
			};

			ImGui::Columns(3, 0, false);
			drawRenderTarget("Diffuse", 0);
			drawRenderTarget("View-space normals", 1);
			drawRenderTarget("Depth buffer", 2);
			ImGui::Columns();
		}
		ImGui::End();

		{ 
			VESP_PROFILE_BLOCK("GUI render event");
			EventManager::Get()->Fire("Render.Gui");
		}

		{
			VESP_PROFILE_BLOCK("ImGui render");
			ImGui::Render();
		}
		
		{
			VESP_PROFILE_BLOCK("Present");
			this->SetDepthEnabled(true);
			SwapChain->Present(0, 0);
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
		depthTextureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depthTextureDesc.SampleDesc.Count = 1;
		depthTextureDesc.SampleDesc.Quality = 0;
		depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		depthTextureDesc.BindFlags = 
			D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depthTextureDesc.CPUAccessFlags = 0;
		depthTextureDesc.MiscFlags = 0;
		hr = Device->CreateTexture2D(&depthTextureDesc, NULL, &depthTexture);
		VESP_ENFORCE(SUCCEEDED(hr));

		// Create depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		depthStencilViewDesc.Flags = 0;
		hr = Device->CreateDepthStencilView(
			depthTexture, &depthStencilViewDesc, &this->depthStencilView_);
		VESP_ENFORCE(SUCCEEDED(hr));

		// Create depth buffer shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D = { 0, static_cast<UINT>(-1) };

		hr = Device->CreateShaderResourceView(
			depthTexture, &srvDesc, &this->renderTargetResourceViews_[2]);
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
		diffuseSRVDesc.Texture2D = {0, static_cast<UINT>(-1)};

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
		normalSRVDesc.Texture2D = {0, static_cast<UINT>(-1)};

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
		auto shaderManager = ShaderManager::Get();
		shaderManager->LoadShader("default", ShaderType::Vertex);
		shaderManager->LoadShader("default", ShaderType::Pixel);
		shaderManager->LoadShader("grid", ShaderType::Pixel);
		shaderManager->LoadShader("identity", ShaderType::Vertex);
		shaderManager->LoadShader("composite", ShaderType::Pixel);
		shaderManager->LoadShader("texture", ShaderType::Pixel);
		shaderManager->LoadShader("sky", ShaderType::Pixel);

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
		screenMesh.SetVertexShader("identity");
		screenMesh.SetPixelShader("composite");

		skyMesh.Create(screenVertices);
		skyMesh.SetVertexShader("identity");
		skyMesh.SetPixelShader("sky");

		auto scalarField = MakeAlignedUnique<world::ScalarField>();
		scalarField->LoadFromFunction(32, 32, 32, [](Vec3 const& p) {
			return glm::length(p) - 15.0f;
		});
		auto scalarFieldVerts = scalarField->Polygonise(0.0f);

		graphics::Mesh scalarFieldMesh;
		scalarFieldMesh.Create(scalarFieldVerts);
		scalarFieldMesh.SetVertexShader("default");
		scalarFieldMesh.SetPixelShader("default");

		this->meshes_.push_back(scalarFieldMesh);
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