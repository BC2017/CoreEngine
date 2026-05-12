#include "Engine/Platform/Win32Window.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace HFEngine::Platform
{
    namespace
    {
        constexpr wchar_t WindowClassName[] = L"HFEngineWindowClass";

        LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
        {
            if (message == WM_CLOSE)
            {
                DestroyWindow(hwnd);
                return 0;
            }

            if (message == WM_DESTROY)
            {
                return 0;
            }

            return DefWindowProcW(hwnd, message, wparam, lparam);
        }

        void RegisterWindowClass(HINSTANCE instance)
        {
            static bool registered = false;
            if (registered)
            {
                return;
            }

            WNDCLASSEXW windowClass{};
            windowClass.cbSize = sizeof(windowClass);
            windowClass.style = CS_HREDRAW | CS_VREDRAW;
            windowClass.lpfnWndProc = WindowProc;
            windowClass.hInstance = instance;
            windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
            windowClass.lpszClassName = WindowClassName;

            RegisterClassExW(&windowClass);
            registered = true;
        }
    }

    Win32Window::~Win32Window()
    {
        Destroy();
    }

    bool Win32Window::Create(const WindowDesc& desc)
    {
        Destroy();

        HINSTANCE instance = GetModuleHandleW(nullptr);
        RegisterWindowClass(instance);

        RECT rect{ 0, 0, static_cast<LONG>(desc.width), static_cast<LONG>(desc.height) };
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

        HWND hwnd = CreateWindowExW(
            0,
            WindowClassName,
            desc.title.c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            rect.right - rect.left,
            rect.bottom - rect.top,
            nullptr,
            nullptr,
            instance,
            nullptr);

        if (hwnd == nullptr)
        {
            return false;
        }

        hwnd_ = hwnd;
        width_ = desc.width;
        height_ = desc.height;
        open_ = true;

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
        return true;
    }

    void Win32Window::Destroy() noexcept
    {
        if (hwnd_ != nullptr)
        {
            DestroyWindow(static_cast<HWND>(hwnd_));
            hwnd_ = nullptr;
        }

        open_ = false;
        width_ = 0;
        height_ = 0;
    }

    bool Win32Window::PumpMessages() noexcept
    {
        MSG message{};
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
            {
                open_ = false;
                return false;
            }

            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        if (hwnd_ != nullptr && IsWindow(static_cast<HWND>(hwnd_)) == FALSE)
        {
            hwnd_ = nullptr;
            open_ = false;
        }

        return open_;
    }

    void* Win32Window::NativeHandle() const noexcept
    {
        return hwnd_;
    }

    std::uint32_t Win32Window::Width() const noexcept
    {
        return width_;
    }

    std::uint32_t Win32Window::Height() const noexcept
    {
        return height_;
    }

    bool Win32Window::IsOpen() const noexcept
    {
        return open_;
    }
}
