#include "vesp/graphics/Window.hpp"
#include "vesp/graphics/Engine.hpp"

#include "vesp/math/Vector.hpp"
#include "vesp/math/Util.hpp"
#include "vesp/util/StringConversion.hpp"
#include "vesp/Assert.hpp"

#include "vesp/EventManager.hpp"

namespace vesp { namespace graphics {

	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
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
			ShowCursor(FALSE);
			break;

		case WM_KILLFOCUS:
			EventManager::Get()->Fire("Window.Unfocus");
			ShowCursor(TRUE);
			break;

		case WM_SIZE:
		{
			auto engine = Engine::Get();

			if (engine)
			{
				engine->HandleResize(
					IVec2(math::LowWord(lparam), math::HighWord(lparam)));
			}
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

		auto wideTitle = util::MultiToWide(title, CP_UTF8);
		this->hwnd_ = CreateWindowW(className, wideTitle.data(),
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			size.x, size.y, nullptr, nullptr, nullptr, nullptr);

		VESP_ENFORCE(this->hwnd_ != NULL && "Failed to create window");

		// Center the window
		RECT rect{};
		GetWindowRect(this->hwnd_, &rect);

		int x = (GetSystemMetrics(SM_CXSCREEN) - rect.right) / 2;
		int y = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom) / 2;
		this->SetPosition(IVec2(x, y));

		ShowWindow(this->hwnd_, SW_SHOW);
		UpdateWindow(this->hwnd_);
	}

	Window::~Window()
	{
		if (this->hwnd_)
			DestroyWindow(this->hwnd_);
	}

	void Window::SetTitle(RawStringPtr title)
	{
		auto wideString = util::MultiToWide(title, CP_UTF8);
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

	float Window::GetAspectRatio()
	{
		auto size = this->GetSize();
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
		return GetActiveWindow() == this->hwnd_;
	}

	void Window::Pulse()
	{
	}
} }