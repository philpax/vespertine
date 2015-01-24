#pragma once

#include "vesp/util/GlobalSystem.hpp"
#include "vesp/Types.hpp"

#include <atlbase.h>

struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;

namespace vesp { namespace graphics {

	class Window;

	class Engine : public util::GlobalSystem<Engine>
	{
	public:
		Engine(StringPtr title);
		~Engine();

		void Initialize();
		void Pulse();

		static IDXGISwapChain* SwapChain;
		static ID3D11Device* Device;
		static ID3D11DeviceContext* ImmediateContext;
		static ID3D11RenderTargetView* RenderTargetView;

	private:
		std::unique_ptr<Window> window_;
	};

} }