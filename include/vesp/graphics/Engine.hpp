#pragma once

#include "vesp/Util.hpp"
#include "vesp/Types.hpp"
#include "vesp/Containers.hpp"

#include <atlbase.h>
#include <memory>

struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilState;
struct ID3D11DepthStencilView;
struct ID3D11BlendState;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;
struct ID3D11RasterizerState;

namespace vesp { namespace graphics {

	class Window;
	class Camera;

	class Engine : public util::GlobalSystem<Engine>
	{
	public:
		Engine(RawStringPtr title);
		~Engine();

		void Initialize();
		void HandleResize(IVec2 size);
		void Pulse();

		Window* GetWindow();
		Camera* GetCamera();

		void SetBlendingEnabled(bool state);
		void SetDepthEnabled(bool state);

		static IDXGISwapChain* SwapChain;
		static ID3D11Device* Device;
		static ID3D11DeviceContext* ImmediateContext;

	private:
		void CreateDevice(IVec2 size);
		void CreateDepthStencil(IVec2 size);
		void CreateRenderTargets(IVec2 size);
		void CreateBlendState();
		void CreateSamplerState();
		void CreateTestData();

		void DestroyDepthStencil();
		void DestroyRenderTargets();

		std::unique_ptr<Window> window_;
		std::unique_ptr<Camera> camera_;

		// 0 - backbuffer
		// 1 - diffuse
		// 2 - normals
		Array<CComPtr<ID3D11RenderTargetView>, 3> renderTargetViews_;
		// 0 - diffuse
		// 1 - normals
		Array<CComPtr<ID3D11ShaderResourceView>, 2> renderTargetResourceViews_;
		CComPtr<ID3D11DepthStencilState> enabledDepthStencilState_;
		CComPtr<ID3D11DepthStencilState> disabledDepthStencilState_;
		CComPtr<ID3D11DepthStencilView> depthStencilView_;
		CComPtr<ID3D11BlendState> blendState_;
		CComPtr<ID3D11SamplerState> samplerState_;
		CComPtr<ID3D11RasterizerState> rasterizerState_;

		util::Timer timer_;
		util::Timer fpsTimer_;
	};

} }