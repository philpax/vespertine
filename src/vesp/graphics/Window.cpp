#include "vesp/graphics/Window.hpp"
#include "vesp/graphics/Engine.hpp"

#include "vesp/math/Vector.hpp"
#include "vesp/math/Util.hpp"
#include "vesp/util/StringConversion.hpp"
#include "vesp/Assert.hpp"

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
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}

		return 0;
	}

	Window::Window(StringPtr title, IVec2 size)
	{
		static const wchar_t className[] = L"VespertineD3D";

		WNDCLASSEXW wndClass{};
		wndClass.cbSize = sizeof(WNDCLASSEXW);
		wndClass.lpfnWndProc = &WndProc;
		wndClass.lpszClassName = className;
		ATOM atom = RegisterClassExW(&wndClass);

		VESP_ENFORCE(atom != 0 && "Failed to register window class");

		hwnd_ = CreateWindowW(className, util::MultiToWide(title, CP_UTF8).data(),
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			size.x, size.y, nullptr, nullptr, nullptr, nullptr);

		VESP_ENFORCE(hwnd_ != NULL && "Failed to create window");

		// Center the window
		RECT rect{};
		GetWindowRect(hwnd_, &rect);

		int x = (GetSystemMetrics(SM_CXSCREEN) - rect.right) / 2;
		int y = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom) / 2;
		SetPosition(IVec2(x, y));

		ShowCursor(FALSE);
		ShowWindow(hwnd_, SW_SHOW);
		UpdateWindow(hwnd_);
	}

	Window::~Window()
	{
		if (hwnd_)
			DestroyWindow(hwnd_);
	}

	void Window::SetTitle(StringPtr title)
	{
		auto  wideString = util::MultiToWide(title, CP_UTF8);
		SetWindowTextW(hwnd_, wideString.data());
	}

	void Window::SetPosition(IVec2 position)
	{
		SetWindowPos(hwnd_, nullptr, position.x, position.y, 0, 0, SWP_NOSIZE);
	}

	void Window::SetSize(IVec2 size)
	{
		SetWindowPos(hwnd_, nullptr, 0, 0, GetSize().x, GetSize().y, SWP_NOREPOSITION);
	}

	IVec2 Window::GetPosition()
	{
		RECT rect{};
		GetWindowRect(hwnd_, &rect);

		return IVec2(rect.left, rect.top);
	}

	IVec2 Window::GetSize()
	{
		RECT rect{};
		GetClientRect(hwnd_, &rect);

		return IVec2(rect.right - rect.left, rect.bottom - rect.top);
	}

	float Window::GetAspectRatio()
	{
		auto size = this->GetSize();
		return (float)size.x / (float)size.y;
	}

	void* Window::GetSystemRepresentation()
	{
		return hwnd_;
	}

	bool Window::IsFullscreen()
	{
		// There is probably a much better way of handling this..
		return (GetWindowLong(hwnd_, GWL_STYLE) & WS_POPUP) == WS_POPUP;
	}

	void Window::Pulse()
	{
	}
} }