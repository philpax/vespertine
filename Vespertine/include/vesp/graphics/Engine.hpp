#pragma once

#include "vesp/util/GlobalSystem.hpp"
#include "vesp/Types.hpp"

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

		void Pulse();

	private:
		std::unique_ptr<Window> window_;
		IDXGISwapChain* swapChain_;
		ID3D11Device* device_;
		ID3D11DeviceContext* immediateContext_;
		ID3D11RenderTargetView* renderTargetView_;
	};

} }