#include "vesp/graphics/Window.hpp"
#include "vesp/graphics/Engine.hpp"

#include "vesp/math/Vector.hpp"
#include "vesp/math/Util.hpp"
#include "vesp/util/StringConversion.hpp"

#include "vesp/Assert.hpp"
#include "vesp/InputManager.hpp"
#include "vesp/EventManager.hpp"

extern LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace vesp { namespace graphics {

	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (InputManager::Get()->HasGuiLock())
		{
			if (ImGui_ImplDX11_WndProcHandler(hwnd, msg, wparam, lparam))
				return true;
		}

		Window* self = nullptr;

		if (msg == WM_NCCREATE)
		{
			auto createStruct = reinterpret_cast<LPCREATESTRUCT>(lparam);
			self = reinterpret_cast<Window*>(createStruct->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
		}
		else
		{
			self = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		}

		switch (msg)
		{
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_SETFOCUS:
			EventManager::Get()->Fire("Window.Focus");
			break;

		case WM_KILLFOCUS:
			EventManager::Get()->Fire("Window.Unfocus");
			break;

		case WM_EXITSIZEMOVE:
		{
			auto engine = Engine::Get();

			if (engine && self)
				engine->HandleResize(self->GetSize());
			break;
		}
		default:
			return DefWindowProcW(hwnd, msg, wparam, lparam);
		}

		return 0;
	}

	Window::Window(RawStringPtr title, IVec2 size)
	{
		static const wchar_t className[] = L"VespertineD3D";

		WNDCLASSEXW wndClass{};
		wndClass.cbSize = sizeof(WNDCLASSEXW);
		wndClass.lpfnWndProc = &WndProc;
		wndClass.lpszClassName = className;
		ATOM atom = RegisterClassExW(&wndClass);

		VESP_ENFORCE(atom != 0 && "Failed to register window class");

		auto wideTitle = util::MultiToWide(title);
		this->hwnd_ = CreateWindowW(className, wideTitle.data(),
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			size.x, size.y, nullptr, nullptr, nullptr, this);

		VESP_ENFORCE(this->hwnd_ != NULL && "Failed to create window");

		// Center the window
		RECT rect{};
		GetWindowRect(this->hwnd_, &rect);

		int x = (GetSystemMetrics(SM_CXSCREEN) - rect.right) / 2;
		int y = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom) / 2;
		this->SetPosition(IVec2(x, y));

		ShowWindow(this->hwnd_, SW_SHOW);
		UpdateWindow(this->hwnd_);

		// Decrease the cursor counter by 1 if we're not focused; this counteracts
		// it going up by 1 in UpdateClip (as a result of the window not being focused)
		// This fixes the bug where the cursor would be permanently on if the window
		// launched without focus
		if (!this->HasFocus())
			ShowCursor(FALSE);
	}

	Window::~Window()
	{
		if (this->hwnd_)
			DestroyWindow(this->hwnd_);
	}

	void Window::SetTitle(RawStringPtr title)
	{
		auto wideString = util::MultiToWide(title);
		SetWindowTextW(this->hwnd_, wideString.data());
	}

	void Window::SetPosition(IVec2 position)
	{
		SetWindowPos(this->hwnd_, nullptr, position.x, position.y, 0, 0, SWP_NOSIZE);
	}

	void Window::SetSize(IVec2 size)
	{
		SetWindowPos(this->hwnd_, nullptr, 0, 0, GetSize().x, GetSize().y, SWP_NOREPOSITION);
	}

	void Window::UpdateClip()
	{
		bool clip = this->HasFocus() && !InputManager::Get()->HasGuiLock();
		static bool oldClip = !clip;

		if (clip == oldClip)
			return;

		if (clip)
		{
			RECT rect;
			GetClientRect(this->hwnd_, &rect);
			ClientToScreen(this->hwnd_, reinterpret_cast<POINT*>(&rect.left));
			ClientToScreen(this->hwnd_, reinterpret_cast<POINT*>(&rect.right));
			ClipCursor(&rect);
			ShowCursor(FALSE);
		}
		else
		{
			ClipCursor(nullptr);
			ShowCursor(TRUE);
		}

		oldClip = clip;
	}

	IVec2 Window::GetPosition()
	{
		RECT rect{};
		GetWindowRect(this->hwnd_, &rect);

		return IVec2(rect.left, rect.top);
	}

	IVec2 Window::GetSize()
	{
		RECT rect{};
		GetClientRect(this->hwnd_, &rect);

		return IVec2(rect.right - rect.left, rect.bottom - rect.top);
	}

	IVec2 Window::GetCentre()
	{
		RECT rect{};
		GetClientRect(this->hwnd_, &rect);

		return this->GetPosition() + this->GetSize()/2;
	}

	float Window::GetAspectRatio()
	{
		auto size = this->GetSize();

		if (size.x == 0 || size.y == 0)
			return 1.0f;

		return (float)size.x / (float)size.y;
	}

	void* Window::GetSystemRepresentation()
	{
		return this->hwnd_;
	}

	bool Window::IsFullscreen() const
	{
		// There is probably a much better way of handling this..
		return (GetWindowLong(this->hwnd_, GWL_STYLE) & WS_POPUP) == WS_POPUP;
	}

	bool Window::HasFocus() const
	{
		return GetForegroundWindow() == this->hwnd_;
	}

	void Window::Pulse()
	{
		this->UpdateClip();
	}
} }