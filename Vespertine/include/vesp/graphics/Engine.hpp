#pragma once

#include "vesp/Util.hpp"
#include "vesp/Types.hpp"

#include <atlbase.h>
#include <memory>

struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;

namespace vesp { namespace graphics {

	class Window;
	class Camera;

	class Engine : public util::GlobalSystem<Engine>
	{
	public:
		Engine(StringPtr title);
		~Engine();

		void Initialize();
		void Pulse();

		Window* GetWindow();

		static IDXGISwapChain* SwapChain;
		static ID3D11Device* Device;
		static ID3D11DeviceContext* ImmediateContext;
		static ID3D11RenderTargetView* RenderTargetView;

	private:
		std::unique_ptr<Window> window_;
		std::unique_ptr<Camera> camera_;
		util::Timer timer_;
		util::Timer fpsTimer_;
		U32 frameCount_;
	};

} }