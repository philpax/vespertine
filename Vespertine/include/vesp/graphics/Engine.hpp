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

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetImmediateContext();

	private:
		std::unique_ptr<Window> window_;
		CComPtr<IDXGISwapChain> swapChain_;
		CComPtr<ID3D11Device> device_;
		CComPtr<ID3D11DeviceContext> immediateContext_;
		CComPtr<ID3D11RenderTargetView> renderTargetView_;
	};

} }